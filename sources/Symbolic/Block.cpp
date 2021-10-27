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
    println("Expected next BB to be return or branch; was not. \n",
            "Instead got ", instructions.back()->id());
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
static void align_register_edge(Block &prev, Block &post);
void Block::align_registers() {
    auto nexts = nextBBs();
    if(nexts.size() == 2){
        if(nexts[0].get().inputs_aligned) {
            align_register_edge(*this, nexts[0]);
            align_register_edge(*this, nexts[1]);
        }
        else if(nexts[1].get().inputs_aligned) {
            align_register_edge(*this, nexts[1]);
            align_register_edge(*this, nexts[0]);
        }
        else {
            align_register_edge(*this, nexts[1]);
            align_register_edge(*this, nexts[0]);
        }
    }
    if(nexts.size() == 1) {
        align_register_edge(*this, nexts[0].get());
    }
}

static void align_register_edge(Block &prev, Block &post) {
    dln("Align registers from #", prev.index, " to #", post.index);
    if(!prev.outputs_aligned && post.inputs_aligned){
        println("Gen nodes for prev");
        indent();
        for(int i = 0; i < prev.outputs.size(); i++) {
            print(post.initial_registers[i].to_string());
            Node::redefine_preceding_edge(
                    prev.outputs[i],
                    post.initial_registers[i]);
        }
        unindent();
        prev.outputs_aligned = true;
    }

    if(prev.outputs_aligned && !post.inputs_aligned){
        print("Gen nodes for post");
        indent();
        for(Node *thing : prev.outputs) {
            println(thing->backward_edge_register.to_string());
            post.initial_registers.push_back(thing->backward_edge_register);
        }
        unindent();
        post.inputs_aligned = true;
    }

    if(!prev.outputs_aligned && !post.inputs_aligned){
        println("Gen nodes for both");
        indent();
        for(Node *prev_node : prev.outputs){
            // new common register for the new edge
            auto new_reg = post.register_gen.get();
            print(new_reg.to_string());
            // modify forward edge of preceding node to be the new register
            Node::redefine_preceding_edge(prev_node, new_reg);
            // note new register in a field of the next BB
            post.initial_registers.push_back(new_reg);
        }
        unindent();
        prev.outputs_aligned = true;
        post.inputs_aligned = true;
    }
}

std::string Block::name() {
    return "bb#" + std::to_string(index);
}
