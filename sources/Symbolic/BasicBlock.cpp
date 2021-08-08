#include "../../headers/Symbolic/BasicBlock.h"

using namespace mov;


BasicBlock::BasicBlock(BBgen &gen) : index(gen.get()) {}

std::vector<BasicBlock *> BasicBlock::nextBBs() {
    if(instructions.back()->id() == primitive_words::BRANCH){
        return {instructions.back()->as_branch()->jump_to};
    }else if(instructions.back()->id() == primitive_words::BRANCHIF){
        return {instructions.back()->as_branchif()->jump_to_next,
                instructions.back()->as_branchif()->jump_to_far};
    }else if(instructions.back()->id() == primitive_words::EXIT){
        return {};
    }
}
