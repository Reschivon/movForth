

#ifndef MOVFORTH_FBUILDER_H
#define MOVFORTH_FBUILDER_H

#include <llvm/IR/IRBuilder.h>
#include "../Symbolic/Structures.h"

using namespace llvm;

namespace mov {

    class FBuilder : public IRBuilder<> {

        Function *the_function;
        LLVMContext &the_context;

        std::unordered_map<Register, Value*, Register::RegisterHash> val_ptrs;

        std::unordered_map<uint, BasicBlock *> blocks;

        Value* emit_string_ptr = nullptr;

    public:
        explicit FBuilder(LLVMContext &context, Function *the_function);

        Value* get_emit_string_ptr(){
            if(!emit_string_ptr)
                emit_string_ptr = CreateGlobalStringPtr("%d\n");
            return emit_string_ptr;
        }

        /**
         * Build a load instruction and return the result of that load
         * @param reg
         * @return
         */
        Value* build_load_register(Register reg);

        /**
         * Build a store instruction to the reg variable
         * @param value
         * @param reg
         */
        void build_store_register(Value* value, Register reg);

        /**
         * Inform the builder ahead of time that certain
         * parameter variables point to special parameter Values
         * @param reg
         * @param a_i
         */
        void insert_val_ptr(Register reg, Value *a_i);

        void create_block(uint index, BasicBlock *block);

        BasicBlock *get_block(uint index);

        Value *CreateForthConstant(element val) {
            return CreateAdd(
                    ConstantInt::get(Context, APInt(64, val, true)),
                    ConstantInt::get(Context, APInt(64, 0, true))
            );
        }

        /**
         * Make sure this variable gets allocated at the beginning, then return the Value*
         * pointing to that allocation
         * @param reg
         * @param val_size
         * @return
         */
        Value * create_ptr_to_val(Register reg);

    private:

        /**
         * Return the Value* pointing to the allocation of this variable
         * @param reg
         * @return
         */
        Value * get_ptr_to_val(Register reg);

        /**
         * Utility for building alloca instructions in entry block
         * @param func
         * @param var_name
         * @param size
         * @return
         */
        AllocaInst *create_entry_block_alloca(Function *func, const std::string &var_name);

    };
}

#endif //MOVFORTH_FBUILDER_H
