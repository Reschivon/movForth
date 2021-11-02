
#include <llvm-c/Core.h> // just for testing

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/Verifier.h>
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"

#include "llvm/Transforms/Scalar.h" // for some reason some passes live here
//#include "llvm/Transforms/Scalar/GVN.h"

// #include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/Inliner.h"

#include "../../headers/Generation/IRGenerator.h"
#include "../../headers/Interpretation/iWord.h"
#include "../../headers/SystemExec.h"

using namespace llvm;
using namespace mov;


IRGenerator::IRGenerator()
        : the_context(LLVMContext()),
          the_module(std::make_shared<Module>("MovForth", the_context)),
          fpm(std::make_unique<legacy::FunctionPassManager>(the_module.get()))
{

    // Promote allocas to registers.
    fpm->add(createPromoteMemoryToRegisterPass()); //SSA conversion
//    fpm->add(createCFGSimplificationPass()); //Dead code elimination
//    fpm->add(createSROAPass());
//    fpm->add(new InlinerPass()); // TODO not sure what threshold means
//    fpm->add(createFunctionInliningPass());
//    fpm->add(createLoopSimplifyCFGPass());
//    fpm->add(createConstantPropagationPass());
//    fpm->add(createNewGVNPass());//Global value numbering
//    fpm->add(createReassociatePass());
//    fpm->add(createPartiallyInlineLibCallsPass()); //Inline standard calls
//    fpm->add(createDeadCodeEliminationPass());
//    fpm->add(createCFGSimplificationPass()); //Cleanup

// merge successive `icmp %x, 0` which can happen becasue branchif compiles
// an icmp, which must be undone by another icmp
    fpm->add(createInstructionCombiningPass());
//    fpm->add(createFlattenCFGPass()); //Flatten the control flow graph.

// constant folding pass
    fpm->add(createSROAPass());
    fpm->add(createConstantPropagationPass());
    fpm->add(createReassociatePass());
    fpm->add(createCFGSimplificationPass());


    fpm->doInitialization();
}

std::pair<IRModule, bool> IRGenerator::generate(mov::sWord *root) {

    declare_printf();

    Function *func = generate_function(root, true);
    if(!func)
        return std::make_pair(IRModule(nullptr), true);

    // optimize_module_becasue_for_some_reason_FPM_isnt_doing_anything();
    fpm->doFinalization();

    return std::make_pair(IRModule(the_module), false);
}

Function *IRGenerator::get_function(sWordptr fword) {
    bool already_generated = visited_words.find(fword) != visited_words.end();
    if(!already_generated) {
        auto pair = visited_words.insert(std::pair(fword, generate_function(fword, false)));
        return pair.first->second;
    }else{
        return visited_words.at(fword);
    }
}
void IRGenerator::exec_module(const std::string& program_name) {
    dln();
    dln("=========[Exec]=========");
    println(exec("lli " + program_name + ".ll"));
}
Function *IRGenerator::generate_function(mov::sWord *fword, bool is_root) {

    dln();
    dln("=========[IR Generation]=========");

    if(is_root && fword->effects.num_popped != 0) {
        println("Word ", fword->name, " must not pop from stack to be compiled");
        return nullptr;
    }

    if(is_root && fword->effects.num_pushed != 0) {
        println("Word ", fword->name, " must not push to stack to be compiled");
        return nullptr;
    }

    // make_main();

    uint num_params = fword->effects.num_popped;
    uint num_returns = fword->effects.num_pushed;

    // create empty function corresponding to fword
    std::vector<Type*> arg_types;
    arg_types.insert(arg_types.end(), num_params, Type::getInt32Ty(the_context));
    arg_types.insert(arg_types.end(), num_returns,Type::getInt32PtrTy(the_context));

    FunctionType *func_type = FunctionType::get(Type::getVoidTy(the_context), arg_types, false);
    Function* the_function = Function::Create(func_type,
                     is_root? Function::ExternalLinkage : Function::PrivateLinkage,
                     fword->name,
                     the_module.get());

    FBuilder builder(the_context, the_function);

    // create an empty entry block -- sometimes forth code
    // will branch to entry, which is not allowed
    BasicBlock::Create(the_context, "entry", the_function);

    // create empty basic blocks - not added to function yet
    for(const auto& block : fword->blocks) {
        BasicBlock *newbb = BasicBlock::Create(
                the_context,
                std::to_string(block.index) + ".br",
                the_function);
        builder.create_block(block.index, newbb);
    }

    // insert at the entry block
    builder.SetInsertPoint(&the_function->getEntryBlock());
    builder.CreateBr(builder.get_block(1));

    // insert at the first Forth block
    builder.SetInsertPoint(builder.get_block(1));

    // set names for all arguments, alloca space for them,
    // store the arg register in the space,
    // and record the AllocaInst in the register map
    RegisterGen param_name_gen(0);
    for (int i = 0; i < fword->effects.num_popped; i++) {
        auto *arg = the_function->args().begin() + i;

        Register reg = param_name_gen.get_param();
        arg->setName(reg.to_string_allowed_chars() + "reg");

        builder.build_store_register(arg, reg);
    }
    // register all reference params as AllocInst* in the lookup
    uint word_ret = 0;
    for (uint i = fword->effects.num_popped; i < fword->effects.num_popped + fword->effects.num_pushed; i++) {
        // Value that is actually Alloca instance (pointer)
        Value *arg = the_function->args().begin() + i;

        Register reg = fword->my_graphs_returns[word_ret++]->backward_edge_register;
        arg->setName(reg.to_string_allowed_chars() + "reg");

        builder.insert_val_ptr(reg, arg);
    }

    // now lets iterate over every instruction in every Block
    for(const auto& block : fword->blocks) {
        //println("IR for Basic Block #", block.index);
        indent();

        BasicBlock *bb = builder.get_block(block.index);

        // the_function->getBasicBlockList().push_back(bb);
        builder.SetInsertPoint(bb);

        for(auto instr : block.instructions) {
            //println();

            switch (instr->id()) {
                case OTHER: {
                    //println("other");

                    std::vector<Value *> arg_values;

                    for (auto pop: instr->pop_nodes) {
                        //println("push value to arg");
                        arg_values.push_back(builder.build_load_register(pop->backward_edge_register));
                    }
                    for (auto push: instr->push_nodes) {
                        // alloca instruction (pointer) hidden as value
                        //println("push AllocaInstr to arg");
                        arg_values.push_back(builder.create_ptr_to_val(push->forward_edge_register));
                    }
                    sWordptr werd = instr->linked_word;
                    Function *funk = get_function(werd);
                    if(!funk)
                        return nullptr;

                    builder.CreateCall(funk, arg_values);
                    break;
                }

                case LITERAL: {
                    //println("Literal(", instr->data.as_num(), ")");

                    Value *constant = builder.CreateForthConstant(instr->data.as_num());

                    Register push_to_reg = instr->push_nodes[0]->forward_edge_register;

                    builder.build_store_register(constant, push_to_reg);
                    break;
                }

                case BRANCH: {
                    //println("Branch");

                    Block *jump_to = instr->as_branch()->jump_to;
                    BasicBlock *dest = builder.get_block(jump_to->index);

                    builder.CreateBr(dest);
                    break;
                }

                case BRANCHIF: {
                    //println("Branchif");

                    Register condition = instr->pop_nodes[0]->backward_edge_register;

                    Value *cond_value = builder.build_load_register(condition);

                    Value *TF = builder.CreateICmpEQ(
                            ConstantInt::get(the_context, APInt(32, 0)),
                            cond_value
                    );

                    Block *jump_true = instr->as_branchif()->jump_to_next;
                    Block *jump_false = instr->as_branchif()->jump_to_far;

                    BasicBlock *true_dest = builder.get_block(jump_true->index);
                    BasicBlock *false_dest = builder.get_block(jump_false->index);

                    // yes, true_dest and false_dest are flipped
                    builder.CreateCondBr(TF, false_dest, true_dest);
                    break;
                }

                case EQUALS: {
                    //println("Equals");

                    Register one = instr->pop_nodes[0]->backward_edge_register;
                    Register two = instr->pop_nodes[1]->backward_edge_register;

                    Value *one_v = builder.build_load_register(one);
                    Value *two_v = builder.build_load_register(two);

                    Value *equals_bool = builder.CreateICmpEQ(one_v, two_v);
                    Value *equals = builder.CreateIntCast(equals_bool, builder.getInt32Ty(), true);

                    Register diff_register = instr->push_nodes[0]->forward_edge_register;
                    builder.build_store_register(equals, diff_register);
                    break;
                }

                case EMIT: {
                    //println("Emit");

                    Register num = instr->pop_nodes[0]->backward_edge_register;

                    Value *num_value = builder.build_load_register(num);

                    std::vector<Value *> print_args{
                            builder.get_emit_string_ptr(),
                            num_value
                    };

                    builder.CreateCall(the_module->getFunction("printf"), print_args);
                    break;
                }

                case ADD: {
                    Register one = instr->pop_nodes[0]->backward_edge_register;
                    Register two = instr->pop_nodes[1]->backward_edge_register;

                    Value *one_v = builder.build_load_register(one);
                    Value *two_v = builder.build_load_register(two);

                    Value *sum = builder.CreateAdd(one_v, two_v);
                    Register sum_register = instr->push_nodes[0]->forward_edge_register;

                    builder.build_store_register(sum, sum_register);
                    break;
                }

                case SUBTRACT: {
                    Register one = instr->pop_nodes[0]->backward_edge_register;
                    Register two = instr->pop_nodes[1]->backward_edge_register;

                    Value *one_v = builder.build_load_register(one);
                    Value *two_v = builder.build_load_register(two);

                    Value *diff = builder.CreateSub(two_v, one_v);
                    Register diff_register = instr->push_nodes[0]->forward_edge_register;

                    builder.build_store_register(diff, diff_register);
                    break;
                }

                case MULTIPLY: {
                    Register one = instr->pop_nodes[0]->backward_edge_register;
                    Register two = instr->pop_nodes[1]->backward_edge_register;

                    Value *one_v = builder.build_load_register(one);
                    Value *two_v = builder.build_load_register(two);

                    Value *product = builder.CreateMul(one_v, two_v);
                    Register product_register = instr->push_nodes[0]->forward_edge_register;

                    builder.build_store_register(product, product_register);
                    break;
                }

                case DIVIDE: {
                    Register one = instr->pop_nodes[0]->backward_edge_register;
                    Register two = instr->pop_nodes[1]->backward_edge_register;

                    Value *one_v = builder.build_load_register(one);
                    Value *two_v = builder.build_load_register(two);

                    Value *factor = builder.CreateSDiv(two_v, one_v);
                    Register factor_register = instr->push_nodes[0]->forward_edge_register;

                    builder.build_store_register(factor, factor_register);
                    break;
                }

                case DUP: {
                    Register one = instr->pop_nodes[0]->backward_edge_register;

                    Value *one_v = builder.build_load_register(one);

                    Register out_one = instr->push_nodes[0]->forward_edge_register;
                    Register out_two = instr->push_nodes[1]->forward_edge_register;

                    builder.build_store_register(one_v, out_one);
                    builder.build_store_register(one_v, out_two);

                    break;
                }

                case SWAP: {
                    //println("Swap");

                    Register one = instr->pop_nodes[0]->backward_edge_register;
                    Register two = instr->pop_nodes[1]->backward_edge_register;

                    Value *one_v = builder.build_load_register(one);
                    Value *two_v = builder.build_load_register(two);

                    Register one_out = instr->push_nodes[0]->forward_edge_register;
                    Register two_out = instr->push_nodes[1]->forward_edge_register;

                    builder.build_store_register(one_v, one_out);
                    builder.build_store_register(two_v, two_out);
                    break;
                }

                case ROT: {
                    //println("Rot");

                    Register one = instr->pop_nodes[0]->backward_edge_register;
                    Register two = instr->pop_nodes[1]->backward_edge_register;
                    Register three = instr->pop_nodes[2]->backward_edge_register;

                    Value *one_v = builder.build_load_register(one);
                    Value *two_v = builder.build_load_register(two);
                    Value *three_v = builder.build_load_register(three);

                    Register one_out = instr->push_nodes[0]->forward_edge_register;
                    Register two_out = instr->push_nodes[1]->forward_edge_register;
                    Register three_out = instr->push_nodes[2]->forward_edge_register;

                    builder.build_store_register(one_v, two_out);
                    builder.build_store_register(two_v, one_out);
                    builder.build_store_register(three_v, three_out);

                    break;
                }

                case DROP: // becasue it actually does nothing in terms
                           // of new register values
                    break;

                case EXIT: // do nothing; IR gen has its own way of inserting returns
                    break;


                default:
                    println("Word ", instr->name(), " is not a word supported at runtime");
                    return nullptr;
            }
        }

        unindent();
        //println();
    }

    // insert return at end
    builder.SetInsertPoint(&the_function->getBasicBlockList().back());
    builder.CreateRetVoid();

    // do optimizations
    if(do_optimize) {
        println("Running optimization passes");
        fpm->run(*the_function);
    }else{
        println("Optimization disabled");
    }

    // see if it's all right?
    if (verifyFunction(*the_function, &outs())) {
        println("Error encountered in generation of ", fword->name);
        return nullptr;
    }

    // yay
    dln("Done building IR");

    return the_function;
}

class stdout_stream : public raw_ostream {
    uint64_t pos = 0;

    void write_impl(const char *Ptr, size_t Size) override {
        pos += Size;
        std::cout << Ptr;
    }

    void handle() override {

    }

    uint64_t current_pos() const override {
        return pos;
    }
};

void IRGenerator::print_module(const std::string &program_name, bool to_file) {
    println();
    println("==========[LLVM IR]===========");

    stdout_stream stdout;
    the_module->print(stdout, nullptr);
    outs().flush();

    // print to file
    if (to_file) {
        std::error_code ec;
        raw_fd_ostream file_out_stream(program_name + ".ll", ec, sys::fs::OpenFlags::F_None);
        the_module->print(file_out_stream, nullptr, true, true);
    }
}


void IRGenerator::declare_printf(){
    // declare printf
    std::vector<Type*> printf_arg_types {Type::getInt8PtrTy(the_context)};
    FunctionType *printf_type = FunctionType::get(Type::getInt32Ty(the_context), printf_arg_types, true);
    Function::Create(printf_type, Function::ExternalLinkage, "printf", the_module.get());
}

Function* IRGenerator::make_main(){

    // create empty main function
    FunctionType *main_type = FunctionType::get(Type::getInt32Ty(the_context), false);
    Function *main = Function::Create(main_type, Function::ExternalLinkage, "main", the_module.get());

    FBuilder builder(the_context, main);

    // create empty entry BB in main function
    BasicBlock *entry = BasicBlock::Create(the_context, "entry", main);
    builder.SetInsertPoint(entry);

    std::vector<Value *> print_args{
            builder.CreateGlobalStringPtr("%d\n"),
            ConstantInt::get(the_context, APInt(32, 20))
    };
    builder.CreateCall(the_module->getFunction("printf"), print_args);
    builder.CreateRet(ConstantInt::get(the_context, APInt(32, 0)));

    return main;
}


void IRGenerator::hello_world2() {
    declare_printf();
    make_main();
    print_module("hello world");
}


int IRGenerator::hello_world() {
    // create context, the_module and builder
    LLVMContextRef context = LLVMContextCreate();
    LLVMModuleRef  module = LLVMModuleCreateWithNameInContext("hello", context);
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);

    // types
    LLVMTypeRef int_8_type = LLVMInt8TypeInContext(context);
    LLVMTypeRef int_8_type_ptr = LLVMPointerType(int_8_type, 0);
    LLVMTypeRef int_32_type = LLVMInt32TypeInContext(context);

    // puts function
    LLVMTypeRef puts_function_args_type[] {
            int_8_type_ptr
    };

    LLVMTypeRef  puts_function_type = LLVMFunctionType(int_32_type, puts_function_args_type, 1, false);
    LLVMValueRef puts_function = LLVMAddFunction(module, "puts", puts_function_type);
    // end

    // main function
    LLVMTypeRef  main_function_type = LLVMFunctionType(int_32_type, nullptr, 0, false);
    LLVMValueRef main_function = LLVMAddFunction(module, "main", main_function_type);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(context, main_function, "entry");
    LLVMPositionBuilderAtEnd(builder, entry);

    LLVMValueRef puts_function_args[] = {
            LLVMBuildPointerCast(builder, // cast [14 x i8] type to int8 pointer
                                 LLVMBuildGlobalString(builder, "Hello, World!", "hello"), // build hello string constant
                                 int_8_type_ptr, "0")
    };

    LLVMBuildCall(builder, puts_function, puts_function_args, 1, "i");
    LLVMBuildRet(builder, LLVMConstInt(int_32_type, 0, false));
    // end

    LLVMDumpModule(module); // dump the_module to STDOUT
    LLVMPrintModuleToFile(module, "../hello.ll", nullptr);

    // clean memory
    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);
    LLVMContextDispose(context);

    println("done generation");

    return 0;
}




IRModule::IRModule(std::shared_ptr<Module> m)
    : module(m)
{}

std::shared_ptr<Module> IRModule::get_module() {
    return module;
}
