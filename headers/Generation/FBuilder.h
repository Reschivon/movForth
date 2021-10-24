

#ifndef MOVFORTH_FBUILDER_H
#define MOVFORTH_FBUILDER_H

#include <llvm/IR/IRBuilder.h>
#include "../Symbolic/Structures.h"

using namespace llvm;

namespace mov {

    class FBuilder : public IRBuilder<> {

        Function *the_function;
        LLVMContext &the_context;

        std::unordered_map<Register, AllocaInst *, Register::RegisterHash> allocs;

        std::unordered_map<uint, BasicBlock *> blocks;
    public:
        explicit FBuilder(LLVMContext &context);

        void set_function(Function *func);

        Value* build_load_register(Register reg);

        void build_store_register(Value* value, Register reg);


        AllocaInst *create_entry_block_alloca(Function *func, const std::string &var_name);

        AllocaInst *create_alloc(Register reg);

        AllocaInst *get_alloc(Register reg);

        void create_block(uint index, BasicBlock *block);

        BasicBlock *get_block(uint index);

        Value *CreateForthConstant(int val) {
            return CreateAdd(
                    ConstantInt::get(Context, APInt(32, val)),
                    ConstantInt::get(Context, APInt(1, 0))
            );
        }

        Value *build_load_register_as_ref(Register reg);

        void insert_alloc(Register reg, AllocaInst *a_i);
    };
}

#endif //MOVFORTH_FBUILDER_H
