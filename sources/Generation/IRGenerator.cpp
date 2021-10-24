
#include <llvm-c/Core.h> // just for testing

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DIBuilder.h>
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "../../headers/SystemExec.h"

#include "../../headers/Generation/IRGenerator.h"

using namespace llvm;
using namespace mov;


IRGenerator::IRGenerator()
        : the_context(LLVMContext()),
          the_module(std::make_unique<Module>("MovForth", the_context)),
          builder(the_context) {
}


Function *IRGenerator::get_function(sWordptr fword) {
    auto pair = visited_words.insert(std::pair(fword, generate(fword, false)));
    return pair.first->second;
}

Function *IRGenerator::generate(mov::sWord* fword, bool is_root) {
    if(is_root && fword->effects.num_popped != 0) {
        print("Word ", fword , " must not pop from stack to be compiled");
        return nullptr;
    }

    if(is_root && fword->effects.num_pushed != 0) {
        print("Word ", fword , " must not push to stack to be compiled");
        return nullptr;
    }

    declare_printf();
    // make_main();
/*
    // jank fix for module::print bug
    if(fword->blocks.size() == 1){
        // remove exit instruction
        fword->blocks[0].instructions.pop_back();

        // make new end block
        BBgen dummy;
        Block true_end(dummy);
        // add exit instruction to new block
        true_end.instructions.push_back(new ReturnInstruction());
        // copyless move the block to fword
        fword->blocks.emplace_back(true_end);

        // add branch instruction
        fword->blocks[0].instructions.push_back(new BranchInstruction(
                new sWord("branch", primitive_words::BRANCH),
                new sData(nullptr),
                fword->blocks[1].
                ));
    }*/


    uint num_params = fword->effects.num_popped;
    uint num_returns = fword->effects.num_pushed;

    // create empty function corresponding to fword
    std::vector<Type*> arg_types (num_params + num_returns, builder.getInt32Ty());
    FunctionType *func_type = FunctionType::get(builder.getVoidTy(), arg_types, false);
    Function* the_function = Function::Create(func_type,
                     is_root? Function::ExternalLinkage : Function::PrivateLinkage,
                     fword->name,
                     the_module.get());


    builder.set_function(the_function);

    // create empty basic blocks - not added to function yet
    for(const auto& block : fword->blocks) {
        bool is_first = block.index == 1;
        BasicBlock *newbb;
        if(is_first)
            newbb = BasicBlock::Create(the_context,"entry", the_function);
        else
            newbb = BasicBlock::Create(the_context, std::to_string(block.index) + ".br");

        builder.create_block(block.index, newbb);
    }

    builder.SetInsertPoint(&the_function->getEntryBlock());

    // set names for all arguments, alloca space for them,
    // store the arg register in the space,
    // and record the AllocaInst in the register map
    uint word_arg = 0;
    for (int i = 0; i < fword->effects.num_popped; i++) {
        auto *arg = the_function->args().begin() + i;
        Register reg = fword->my_graphs_params[word_arg++]->forward_edge_register;
        arg->setName(reg.to_string_allowed_chars() + "reg");

        builder.build_store_register(arg, reg);
    }

    // register all reference params as AllocInst* in the lookup
    uint word_ret = 0;
    for (int i = fword->effects.num_popped; i < fword->effects.num_popped + fword->effects.num_pushed; i++) {
        // Value that is actually Alloca instance (pointer)
        Value *arg = the_function->args().begin() + i;
        AllocaInst* arg_ref = cast<AllocaInst>(arg);

        Register reg = fword->my_graphs_returns[word_ret++]->backward_edge_register;
        arg_ref->setName(reg.to_string_allowed_chars() + "reg");

        builder.insert_alloc(reg, arg_ref);
    }

    // now lets iterate over every instruction in every Block
    for(const auto& block : fword->blocks) {
        println("IR for Basic Block #", block.index);
        indent();

        BasicBlock *bb = builder.get_block(block.index);

        the_function->getBasicBlockList().push_back(bb);
        builder.SetInsertPoint(bb);

        for(auto instr : block.instructions) {
            println();
            if(instr->id() == primitive_words::LITERAL) {
                println("Literal(", instr->data.as_num(), ")");

                Value *constant = builder.CreateForthConstant(instr->data.as_num());

                Register push_to_reg = instr->push_nodes[0]->forward_edge_register;

                builder.build_store_register(constant, push_to_reg);
            }
            if(instr->id() == primitive_words::BRANCH) {
                println("Branch");

                Block *jump_to = instr->as_branch()->jump_to;
                BasicBlock *dest = builder.get_block(jump_to->index);

                builder.CreateBr(dest);
            }
            if(instr->id() == primitive_words::BRANCHIF) {
                println("Branchif");

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
            }

            if(instr->id() == primitive_words::EMIT) {
                println("Emit");

                Register num = instr->pop_nodes[0]->backward_edge_register;

                Value *num_value = builder.build_load_register(num);

                std::vector<Value *> print_args{
                        builder.CreateGlobalStringPtr("%d\n"),
                        num_value
                };

                builder.CreateCall(the_module->getFunction("printf"), print_args);
            }

            if(instr->id() == primitive_words::ADD) {
                Register one = instr->pop_nodes[0]->backward_edge_register;
                Register two = instr->pop_nodes[1]->backward_edge_register;

                Value *one_v = builder.build_load_register(one);
                Value *two_v = builder.build_load_register(two);

                Value* sum = builder.CreateAdd(one_v, two_v);
                Register sum_register = instr->push_nodes[0]->forward_edge_register;

                builder.build_store_register(sum, sum_register);
            }

            if(instr->id() == primitive_words::SUBTRACT) {
                Register one = instr->pop_nodes[0]->backward_edge_register;
                Register two = instr->pop_nodes[1]->backward_edge_register;

                Value *one_v = builder.build_load_register(one);
                Value *two_v = builder.build_load_register(two);

                Value* diff = builder.CreateSub(one_v, two_v);
                Register diff_register = instr->push_nodes[0]->forward_edge_register;

                builder.build_store_register(diff, diff_register);
            }

            if(instr->id() == primitive_words::MULTIPLY) {
                Register one = instr->pop_nodes[0]->backward_edge_register;
                Register two = instr->pop_nodes[1]->backward_edge_register;

                Value *one_v = builder.build_load_register(one);
                Value *two_v = builder.build_load_register(two);

                Value* product = builder.CreateMul(one_v, two_v);
                Register product_register = instr->push_nodes[0]->forward_edge_register;

                builder.build_store_register(product, product_register);
            }

            if(instr->id() == primitive_words::DIVIDE) {
                Register one = instr->pop_nodes[0]->backward_edge_register;
                Register two = instr->pop_nodes[1]->backward_edge_register;

                Value *one_v = builder.build_load_register(one);
                Value *two_v = builder.build_load_register(two);

                Value* factor = builder.CreateSDiv(one_v, two_v);
                Register factor_register = instr->push_nodes[0]->forward_edge_register;

                builder.build_store_register(factor, factor_register);
            }

            if(instr->id() == primitive_words::OTHER) {
                println("other");

                std::vector<Value*> arg_values;

                for(auto pop : instr->pop_nodes)
                    arg_values.push_back(builder.build_load_register(pop->backward_edge_register));
                for(auto push : instr->push_nodes)
                    // alloca instance (pointer) hidden as value
                    arg_values.push_back(builder.create_alloc(push->forward_edge_register));

                sWordptr werd = instr->linked_word;
                Function *funk = get_function(werd);

                builder.CreateCall(the_module->getFunction(werd->name), arg_values);

            }
        }

        unindent();
        println("Done IR for Basic Block #", block.index);
        println();

    }

    builder.SetInsertPoint(&the_function->getBasicBlockList().back());
    builder.CreateRetVoid();

    println("Done building IR");

    print_module();

    exec_module();

    return the_function;
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


void IRGenerator::hello_world2() {
    declare_printf();
    make_main();
    print_module();
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


