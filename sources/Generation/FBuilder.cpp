
#include "../../headers/Generation/FBuilder.h"

using namespace mov;
using namespace llvm;

FBuilder::FBuilder(LLVMContext &context) :
        IRBuilder<>(context), the_context(context)
{
    allocs.reserve(10); // there are usually less then 10 Variables in a word
    blocks.reserve(5); // there are usually less then 10 blocks in a word
}

void FBuilder::set_function(Function *func) {
    the_function = func;
}

AllocaInst * FBuilder::create_alloc(Register reg) {
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


Value* FBuilder::build_load_register(Register reg) {
    AllocaInst *alloc = get_alloc(reg);
    return CreateLoad(alloc);
}

Value* FBuilder::build_load_register_as_ref(Register reg){
    AllocaInst *alloc = get_alloc(reg);
    return (Value*) alloc;
}

void FBuilder::build_store_register(Value *value, Register reg) {
    AllocaInst *alloc = create_alloc(reg);
    CreateStore(value, alloc);
}

AllocaInst * FBuilder::get_alloc(Register reg) {
    AllocaInst *alloca;
    try {
        alloca = allocs.at(reg);
    } catch (std::out_of_range&){
        println("register " + reg.to_string_allowed_chars(), " does not exist");
        return nullptr;
    }
    println("retrived register " + reg.to_string_allowed_chars());
    return alloca;
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



