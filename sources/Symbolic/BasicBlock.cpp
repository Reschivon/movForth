#include "../../headers/Symbolic/BasicBlock.h"

using namespace mov;


BasicBlock::BasicBlock(BBgen &gen) : index(gen.get()) {}

std::vector<BasicBlock::bb_ref> BasicBlock::nextBBs() {
    if (instructions.back()->id() == primitive_words::BRANCH)

        return {std::ref(*instructions.back()->as_branch()->jump_to)};

    else if (instructions.back()->id() == primitive_words::BRANCHIF)
    {
        std::vector<BasicBlock::bb_ref> ret;
        ret.emplace_back(*instructions.back()->as_branchif()->jump_to_next),
        ret.emplace_back(*instructions.back()->as_branchif()->jump_to_far);
        return ret;
    }
    else if (instructions.back()->id() == primitive_words::EXIT)

        return {};

    // should never happen
    println("FUCK");
    return {};
}

bool BasicBlock::is_exit() {
    return instructions.size() == 1 && instructions.back()->id() == primitive_words::EXIT;
}
