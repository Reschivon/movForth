

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

        Value* build_load_register(Register reg);

        void build_store_register(Value* value, Register reg);

        AllocaInst *create_entry_block_alloca(Function *func, const std::string &var_name);

        Value * create_ptr_to_val(Register reg);

        Value * get_ptr_to_val(Register reg);

        void create_block(uint index, BasicBlock *block);

        BasicBlock *get_block(uint index);

        Value *CreateForthConstant(int val) {
            return CreateAdd(
                    ConstantInt::get(Context, APInt(32, val)),
                    ConstantInt::get(Context, APInt(1, 0))
            );
        }

        void insert_val_ptr(Register reg, Value *a_i);
    };
}

#endif //MOVFORTH_FBUILDER_H
