
#include "Word.h"

using namespace sym;

void Word::definition_to_string() {
    println("[", name, "]");
    println("input parameters: ", effects.num_popped, " output frames: ", effects.num_pushed);
    for(auto &thing : instructions)
        print(thing->linked_word->name, " ");
    println();
}

Wordptr Word::nop = new Word{.name = "nop"};

BranchInstruction *Instruction::as_branch() {
    return (BranchInstruction*)(this);
}

BranchIfInstruction *Instruction::as_branchif() {
    return (BranchIfInstruction*)(this);
}
