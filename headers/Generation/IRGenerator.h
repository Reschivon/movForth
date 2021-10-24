#ifndef MOVFORTH_IRGENERATOR_H
#define MOVFORTH_IRGENERATOR_H

#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include "../Symbolic/sWord.h"

using namespace llvm;
namespace mov {

    class Variables {

        // prepare lookup for Register -> Value and Block::index -> BasicBlock
        std::unordered_map<Register, AllocaInst*, Register::RegisterHash> allocs;

        std::unordered_map<uint, BasicBlock*> blocks;

        Function *the_function;
        LLVMContext &the_context;

        AllocaInst* create_entry_block_alloca(Function *func, const std::string &var_name);

    public:
        explicit Variables(Function *the_function, LLVMContext &the_context);
        AllocaInst *create_alloc(Register reg);
        AllocaInst *get_alloc(Register reg);
        void create_block(uint index, BasicBlock* block);
        BasicBlock* get_block(uint index);

    };

    class IRGenerator {
    private:
        LLVMContext the_context;
        std::unique_ptr<Module> the_module;
        IRBuilder<> builder;

        Function* make_main();
        void print_module();
        void exec_module();

        void declare_printf();

        BasicBlock* make_basic_block(std::string name, Value* body, Function *function);
        Function* make_function(std::string name,std::vector<Twine> param_names);
        Value* make_constant(int val);
        Value* make_add(Value *first, Value *second);
        Value* make_function_call(std::string name, std::vector<Value*> arguments);

            public:
        explicit IRGenerator();

        void generate(sWord *fword, bool is_root);

    };
}


#endif //MOVFORTH_IRGENERATOR_H
