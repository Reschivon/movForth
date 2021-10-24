
#include <llvm-c/Core.h> // just for testing

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "../../headers/SystemExec.h"

#include "../../headers/Generation/IRGenerator.h"
#include "../../headers/Symbolic/Structures.h"

using namespace llvm;
using namespace mov;

IRGenerator::IRGenerator()
    : the_context(LLVMContext()),
      the_module(std::make_unique<Module>("MovForth", the_context)),
      builder(llvm::IRBuilder(the_context)) {
}


Variables::Variables(Function *the_function, LLVMContext &the_context)
        : the_function(the_function), the_context(the_context)
{
    allocs.reserve(10); // there are usually less then 10 Variables in a word
    blocks.reserve(5); // there are usually less then 10 blocks in a word
}

AllocaInst * Variables::create_alloc(Register reg) {
    bool already_has = allocs.find(reg) != allocs.end();
    if(!already_has) {
        println("inserted new alloca: ", reg.to_string_allowed_chars());
        AllocaInst* new_alloc = create_entry_block_alloca(the_function, reg.to_string_allowed_chars());
        allocs.insert(std::make_pair(reg, new_alloc));
        return new_alloc;
    } else {
        println("already have alloca: ", reg.to_string_allowed_chars());
        return allocs.at(reg);
    }
}

AllocaInst * Variables::get_alloc(Register reg) {
    AllocaInst *alloca;
    try {
        alloca = allocs.at(reg);
    } catch (std::out_of_range&){
        println("register " + reg.to_string_allowed_chars(), " does not exist");
    }
    println("retrived register " + reg.to_string_allowed_chars());
    return alloca;
}

void Variables::create_block(uint index, BasicBlock* block) {
    blocks.insert(std::make_pair(index, block));
}

BasicBlock* Variables::get_block(uint index) {
    return blocks.at(index);
}

AllocaInst* Variables::create_entry_block_alloca(Function *func, const std::string &var_name) {
    IRBuilder<> entry_builder(&func->getEntryBlock(),
                              func->getEntryBlock().begin());
    return entry_builder.CreateAlloca(Type::getInt32Ty(the_context), nullptr, var_name);
}


void IRGenerator::generate(mov::sWord* fword, bool is_root) {
    if(is_root && fword->effects.num_popped != 0) {
        print("Word ", fword , " must not pop from stack to be compiled");
        return;
    }

    declare_printf();
    // make_main();

    uint num_params = fword->effects.num_popped;
    uint num_returns = fword->effects.num_pushed;

    // create empty function corresponding to fword
    std::vector<Type*> arg_types (num_params + num_returns, builder.getInt32Ty());
    FunctionType *func_type = FunctionType::get(builder.getVoidTy(), arg_types, false);
    Function* the_function = Function::Create(func_type,
                     is_root? Function::ExternalLinkage : Function::PrivateLinkage,
                     fword->name,
                     the_module.get());


    Variables variables(the_function, the_context);

    // create empty basic blocks - not added to function yet
    for(const auto& block : fword->blocks) {
        bool is_first = block.index == 1;
        BasicBlock *newbb;
        if(is_first)
            newbb = BasicBlock::Create(the_context,"entry", the_function);
        else
            newbb = BasicBlock::Create(the_context, std::to_string(block.index) + ".br");

        variables.create_block(block.index, newbb);
    }

    builder.SetInsertPoint(&the_function->getEntryBlock());

    // set names for all arguments, alloca space for them,
    // store the arg register in the space,
    // and record the AllocaInst in the register map
    uint word_arg = 0;
    for (auto &arg : the_function->args()) {
        Register reg = fword->my_graphs_inputs[word_arg++]->forward_edge_register;
        arg.setName(reg.to_string_allowed_chars() + "reg");

        AllocaInst *alloc = variables.create_alloc(reg);
        builder.CreateStore(&arg, alloc);

        println("insert alloc: ", reg.to_string_allowed_chars(), " ", alloc->getName().str());
    }


    // now lets iterate over every instruction in every Block
    for(const auto& block : fword->blocks) {
        BasicBlock *bb = variables.get_block(block.index);
        println("IR for Basic Block #", block.index);
        indent();

        the_function->getBasicBlockList().push_back(bb);
        builder.SetInsertPoint(bb);

        for(auto instr : block.instructions) {
            if(instr->id() == primitive_words::LITERAL) {
                println("Literal");
                println("data: ", instr->data.as_num());

                Value *constant = builder.CreateSelect(
                        ConstantInt::get(the_context, APInt(1, 1)),
                        ConstantInt::get(the_context, APInt(32, instr->data.as_num())),
                        ConstantInt::get(the_context, APInt(32, 0))
                );
                Register push_to_reg = instr->push_nodes[0]->forward_edge_register;
                AllocaInst *push_to_alloc = variables.create_alloc(push_to_reg);
                // might already have one
                builder.CreateStore(constant, push_to_alloc);

                println("push to: ", push_to_reg.to_string_allowed_chars());
            }
            if(instr->id() == primitive_words::BRANCH) {
                println("Branch");

                Block *jump_to = instr->as_branch()->jump_to;
                BasicBlock *dest = variables.get_block(jump_to->index);

                builder.CreateBr(dest);
            }
            if(instr->id() == primitive_words::BRANCHIF) {
                println("Branchif");

                Register condition = instr->pop_nodes[0]->backward_edge_register;
                println("pull from: ", condition.to_string_allowed_chars());

                AllocaInst *alloca = variables.get_alloc(condition);
                Value *cond_value = builder.CreateLoad(alloca);

                Value *TF = builder.CreateICmpEQ(
                        ConstantInt::get(the_context, APInt(32, 0)),
                        cond_value
                        );

                Block *jump_true = instr->as_branchif()->jump_to_next;
                Block *jump_false = instr->as_branchif()->jump_to_far;
                BasicBlock *true_dest = variables.get_block(jump_true->index);
                BasicBlock *false_dest = variables.get_block(jump_false->index);

                // yes, true_dest and false_dest are flipped becasue we are
                // comparing to  =0
                builder.CreateCondBr(TF, false_dest, true_dest);
            }

            if(instr->id() == primitive_words::EMIT) {
                println("Emit");

                Register condition = instr->pop_nodes[0]->backward_edge_register;
                println("pull from: ", condition.to_string_allowed_chars());

                AllocaInst *alloc = variables.get_alloc(condition);
                Value *cond_value = builder.CreateLoad(alloc);

                std::vector<Value *> print_args{
                        builder.CreateGlobalStringPtr("%d\n"),
                        cond_value
                };
                builder.CreateCall(the_module->getFunction("printf"), print_args);

                println("done with emit");
            }
        }

        unindent();
        println();
    }

    println("Done building IR");

    builder.CreateRetVoid();

    print_module();

    exec_module();
}


void IRGenerator::print_module() {

    println();
    println("==========[LLVM IR]===========");
    the_module->print(outs(), nullptr);

    // print to file
    std::error_code EC;
    raw_fd_ostream out_stream("../MovForth.ll", EC, sys::fs::OpenFlags::F_None);
    the_module->print(out_stream, nullptr, true, true);
}



void IRGenerator::exec_module() {
    println();
    println("==========[Execution]===========");
    println(exec("lli-6.0 ../MovForth.ll"));
}

void IRGenerator::declare_printf(){
    // declare printf
    std::vector<Type*> printf_arg_types {Type::getInt8PtrTy(the_context)};
    FunctionType *printf_type = FunctionType::get(builder.getInt32Ty(), printf_arg_types, true);
    Function::Create(printf_type, Function::ExternalLinkage, "printf", the_module.get());
}

Function* IRGenerator::make_main(){

    // create empty main function
    FunctionType *main_type = FunctionType::get(builder.getInt32Ty(), false);
    Function *main = Function::Create(main_type, Function::ExternalLinkage, "main", the_module.get());

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

Value* IRGenerator::make_constant(int val){
    return ConstantInt::get(Type::getInt32Ty(the_context), val);
}

Value* IRGenerator::make_add(Value *first, Value *second){
    return builder.CreateAdd(first, second, "addtmp");
}

Value* IRGenerator::make_function_call(std::string name, std::vector<Value*> arguments){
    Function *calle = the_module->getFunction(name);
    return builder.CreateCall(calle, arguments, "calltmp");
}

Function* IRGenerator::make_function(std::string name,std::vector<Twine> param_names){
    std::vector<Type*> parameters(param_names.size(), Type::getInt32Ty(the_context));
    FunctionType *function_type = FunctionType::get(Type::getInt32Ty(the_context), parameters, false);
    Function *the_function = Function::Create(function_type, Function::ExternalLinkage, name, the_module.get());

    auto param_name = param_names.begin();
    for(auto arg = the_function->arg_begin(); arg != the_function->arg_end(); arg++, param_name++){
        arg->setName(*param_name);
    }

    return the_function;
}


bool verify_function(Function *function) {
    return verifyFunction(*function);
}

BasicBlock* IRGenerator::make_basic_block(std::string name, Value* body, Function *function) {
    BasicBlock *bb = BasicBlock::Create(the_context, name, function);
    builder.SetInsertPoint(bb);
    builder.CreateRet(body);

    return bb;
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