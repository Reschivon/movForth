#include "../../headers/Symbolic/BasicBlock.h"

using namespace mov;


BasicBlock::BasicBlock(BBgen &gen) : index(gen.get()), register_gen((int) index) {}

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
    return instructions.size() > 0 && instructions.back()->id() == primitive_words::EXIT;
}

void BasicBlock::match_registers_of_unvisited(BasicBlock &prev, BasicBlock &post) {
    if(!prev.visited && post.visited){
        for(int i = 0; i< prev.my_graphs_outputs.size(); i++)
            Node::redefine_preceding_edge(
                    prev.my_graphs_outputs[i],
                    post.enter_registers[i]);
    }
    if(prev.visited && !post.visited){
        for(Node *thing : prev.my_graphs_outputs)
            post.enter_registers.push_back(thing->forward_edge_register);
    }
    if(!prev.visited && !post.visited){
        for(Node *thing : prev.my_graphs_outputs){
            auto reg = post.register_gen.get();
            post.enter_registers.push_back(reg);
            Node::redefine_preceding_edge(thing, reg);
        }
    }
}

std::string BasicBlock::name() {
    return "bb#" + std::to_string(index);
}
