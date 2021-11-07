
#include <llvm/Support/raw_ostream.h>
#include "../../headers/Generation/FBuilder.h"

using namespace mov;
using namespace llvm;

FBuilder::FBuilder(LLVMContext &context, Function *the_function) :
        IRBuilder<>(context), the_context(context), the_function(the_function)
{
    val_ptrs.reserve(10); // there are usually less then 10 Variables in a word
    blocks.reserve(5); // there are usually less then 10 blocks in a word
}


Value* FBuilder::build_load_register(Register reg) {
    Value *alloc = get_ptr_to_val(reg);
    return CreateLoad(alloc);
}

void FBuilder::build_store_register(Value *value, Register reg) {
    Value *alloc = create_ptr_to_val(reg);
    CreateStore(value, alloc);
}


Value* FBuilder::create_ptr_to_val(Register reg) {
    bool already_has = val_ptrs.find(reg) != val_ptrs.end();
    if(!already_has) {
        //println("inserted new alloca: ", reg.to_string_allowed_chars());
        AllocaInst* new_alloc = create_entry_block_alloca(the_function, reg.to_string_allowed_chars());
        val_ptrs.insert(std::make_pair(reg, new_alloc));
        return new_alloc;
    } else {
        //println("already have alloca: ", reg.to_string_allowed_chars());
        return val_ptrs.at(reg);
    }
}

Value* FBuilder::get_ptr_to_val(Register reg) {
    Value *ptr_to_val;
    try {
        ptr_to_val = val_ptrs.at(reg);
    } catch (std::out_of_range&){
        println("register " + reg.to_string_allowed_chars(), " does not exist");
        return nullptr;
    }
    //println("retrived register " + reg.to_string_allowed_chars());
    return ptr_to_val;
}

void FBuilder::insert_val_ptr(Register reg, Value *a_i){
    val_ptrs.insert(std::make_pair(reg, a_i));
}


void FBuilder::create_block(uint index, BasicBlock* block) {
    blocks.insert(std::make_pair(index, block));
}

BasicBlock* FBuilder::get_block(uint index) {
    return blocks.at(index);
}

AllocaInst* FBuilder::create_entry_block_alloca(Function *func, const std::string &var_name) {
    IRBuilder<> entry_builder(&func->getEntryBlock(),
                              func->getEntryBlock().begin());
    return entry_builder.CreateAlloca(Type::getInt32Ty(the_context), nullptr, var_name);
}



