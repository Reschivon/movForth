#ifndef MOVFORTH_IRGENERATOR_H
#define MOVFORTH_IRGENERATOR_H

#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include "../Symbolic/sWord.h"

using namespace llvm;
namespace mov {

    class IRGenerator {
    private:
        LLVMContext the_context;
        std::unique_ptr<Module> the_module;
        IRBuilder<> builder;

        Function* make_main();
        void print_module();
        void exec_module();
        BasicBlock* make_basic_block(std::string name, Value* body, Function *function);
        Function* make_function(std::string name,std::vector<Twine> param_names);
        Value* make_constant(int val);
        Value* make_add(Value *first, Value *second);
        Value* make_function_call(std::string name, std::vector<Value*> arguments);

            public:
        explicit IRGenerator();

        void generate(sWord *root);

    };
}


#endif //MOVFORTH_IRGENERATOR_H
