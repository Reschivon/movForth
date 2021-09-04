
#include "../../headers/Symbolic/Instruction.h"
#include "../../headers/Symbolic/sWord.h"

using namespace mov;

BranchInstruction *Instruction::as_branch() {
    return (BranchInstruction*)(this);
}

BranchIfInstruction *Instruction::as_branchif() {
    return (BranchIfInstruction*)(this);
}

ReturnInstruction *Instruction::as_return() {
    return (ReturnInstruction*)(this);
}

bool Instruction::branchy() const{
    return
        linked_word->id == primitive_words::BRANCH ||
        linked_word->id == primitive_words::BRANCHIF ||
        linked_word->id == primitive_words::EXIT;
}

std::string Instruction::name(){
    return linked_word->name +
    (data.is_empty()?"": "(" + data.to_string() + ")");
}

Instruction::Instruction(sWordptr linked_word, sData data)
        : linked_word(linked_word), data(data) {}

primitive_words Instruction::id() const {
    return linked_word->id;
}

BranchInstruction::BranchInstruction(sWordptr linked_word, sData data,
                                     BasicBlock *jump_to)
        : Instruction(linked_word, data), jump_to(jump_to) {}

BranchIfInstruction::BranchIfInstruction(sWordptr linked_word, sData data,
                                         BasicBlock *jump_to_close,
                                         BasicBlock *jump_to_far)
        : Instruction(linked_word, data), jump_to_next(jump_to_close), jump_to_far(jump_to_far) {}

ReturnInstruction::ReturnInstruction()
        : Instruction(new sWord("exit", primitive_words::EXIT), sData(nullptr)){}


std::string BranchInstruction::name() {
    return linked_word->name + "(to " + std::to_string(jump_to->index) + ")";
}

std::string BranchIfInstruction::name() {
    return linked_word->name + "(true: " + std::to_string(jump_to_next->index) + ", false: " + std::to_string(jump_to_far->index) +")";
}

std::string ReturnInstruction::name() {
    return "exit";
}


