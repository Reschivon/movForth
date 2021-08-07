
#include "iWord.h"

using namespace sym;

Effects Effects::neutral{};

void Word::definition_to_string() {
    println("[", name, "]");
    println("input parameters: ", effects.num_popped, " output frames: ", effects.num_pushed);
    for(auto &thing : instructions)
        print(thing->linked_word->name, " ");
    println();
}

BranchInstruction *Instruction::as_branch() {
    return (BranchInstruction*)(this);
}

BranchIfInstruction *Instruction::as_branchif() {
    return (BranchIfInstruction*)(this);
}

ReturnInstruction *Instruction::as_return() {
    return (ReturnInstruction*)(this);
}

 bool Instruction::is_jumpy(Instruction* ins) {
    return (ins->linked_word->name == "branch" || ins->linked_word->name == "branchif");
}

ReturnInstruction::ReturnInstruction() : Instruction(new Word("return", Effects::neutral)){}
