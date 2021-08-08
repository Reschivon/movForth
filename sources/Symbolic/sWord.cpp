
#include "../../headers/Symbolic/sWord.h"

#include <utility>

using namespace mov;

Effects Effects::neutral{};

void sWord::definition_to_string() {
    println("[", name, "]");
    println("input parameters: ", effects.num_popped, " output frames: ", effects.num_pushed);
    for(auto &thing : instructions)
        print(thing->linked_word->name, " ");
    println();
}

sWord::sWord(std::string  name, primitive_words id)
    : name(std::move(name)), id(id) {}

sWord::sWord(std::string  name, primitive_words id, Effects effects)
    : name(std::move(name)), id(id), effects(effects) {}

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

std::string Instruction::to_string(){
    return linked_word->name;
}

ReturnInstruction::ReturnInstruction()
: Instruction(new sWord("exit", primitive_words::EXIT), sData(nullptr)){}

std::string BranchInstruction::to_string() {
    return linked_word->name + "(" + std::to_string(jump_to->index) + ")";
}

std::string BranchIfInstruction::to_string() {
    return linked_word->name + "(" + std::to_string(jump_to_next->index) + ", " + std::to_string(jump_to_far->index) +")";
}
