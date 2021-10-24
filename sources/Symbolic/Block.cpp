#include "../../headers/Symbolic/Block.h"

using namespace mov;


Block::Block(BBgen &gen) : index(gen.get()), register_gen((int) index) {}

std::vector<Block::bb_ref> Block::nextBBs() {
    if (instructions.back()->id() == primitive_words::BRANCH)

        return {std::ref(*instructions.back()->as_branch()->jump_to)};

    else if (instructions.back()->id() == primitive_words::BRANCHIF)
    {
        return {
            *instructions.back()->as_branchif()->jump_to_next,
            *instructions.back()->as_branchif()->jump_to_far
        };
    }
    else if (instructions.back()->id() == primitive_words::EXIT)

        return {};

    // should never happen
    println("FUCK");
    return {};
}

bool Block::is_exit() {
    return !instructions.empty() && instructions.back()->id() == primitive_words::EXIT;
}

/**
 * This function makes sure that registers between control flow splits and
 * merges always line up. It's "the grass is always greener" algorithm.
 *
 * The registers pushed to by @prev are guaranteed to be popped by @post. These
 * registers are always indexed according to @post. So if post has index %4 and
 * there are 3 registers spanning across @prev and @post, then they will be named
 * 4-1 4-2 4-3.
 *
 * So if @prev has not been visited (thus this register-aligning function has not
 * been run on it) its final nodes will be set to take the registers 4-1 4-2 4-3. If @post
 * has not been visited, we DO NOT have to create actual nodes. We merely store
 * 4-1 4-2 4-3 in @post's initial_registers so that future @prev blocks can take these
 * registers
 *
 * @param prev
 * @param post
 */
void Block::align_registers(Block &prev, Block &post) {
    if(!prev.visited && post.visited){
        for(int i = 0; i< prev.outputs.size(); i++)
            Node::redefine_preceding_edge(
                    prev.outputs[i],
                    post.initial_registers[i]);
    }
    if(prev.visited && !post.visited){
        for(Node *thing : prev.outputs)
            post.initial_registers.push_back(thing->forward_edge_register);
    }
    if(!prev.visited && !post.visited){
        for(Node *prev_node : prev.outputs){
            // new common register for the new edge
            auto new_reg = post.register_gen.get();
            // modify forward edge of preceding node to be the new register
            Node::redefine_preceding_edge(prev_node, new_reg);
            // note new register in a field of the next BB
            post.initial_registers.push_back(new_reg);
        }
    }
}

std::string Block::name() {
    return "bb#" + std::to_string(index);
}