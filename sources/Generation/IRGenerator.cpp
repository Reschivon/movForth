
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

using namespace llvm;
using namespace mov;

IRGenerator::IRGenerator()
    : the_context(LLVMContext()),
      the_module(std::make_unique<Module>("MovForth", the_context)),
      builder(llvm::IRBuilder(the_context)) {
}

void IRGenerator::generate(mov::sWord* root) {
//        if(root->effects.num_popped != 0) {
//            print("Word ", root , " must not pop from stack to be compiled");
//            return;
//        }

    make_main();

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
    the_module->print(out_stream, nullptr);
}

void IRGenerator::exec_module() {
    println();
    println("==========[Execution]===========");
    println(exec("lli-6.0 ../MovForth.ll"));
}

Function* IRGenerator::make_main(){
    // declare printf
    std::vector<Type*> printf_arg_types {Type::getInt8PtrTy(the_context)};
    FunctionType *printf_type = FunctionType::get(builder.getInt32Ty(), printf_arg_types, true);
    Function::Create(printf_type, Function::ExternalLinkage, "printf", the_module.get());

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



int hello_word() {
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




