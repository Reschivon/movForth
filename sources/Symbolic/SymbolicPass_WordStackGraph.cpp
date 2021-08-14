#include "../../headers/Symbolic/SymbolicPass.h"

using namespace mov;

struct Conflict{
    BasicBlock *from, *to;
    Conflict(BasicBlock *from, BasicBlock *to)
        : from(from), to(to) {}
    [[nodiscard]] std::string to_string() const{
        return "conflict from bb#" +std::to_string(from->index) +
                " to bb#" + std::to_string(to->index);
    }

};

void explore_graph_dfs(NodeList stack, BasicBlock &bb){
    if(bb.visited) return;
    else           bb.enter_stack_size = (int) stack.size();

    dln();
    dln("[bb#: " , bb.index , "] BEGIN stack graph");
    indent();

    // transformed stack == stack, but I want to make
    // it clear that stack has been modified
    NodeList transformed_stack =
            StackGrapher::basic_block_stack_graph(stack, bb, bb.register_gen);

    StackGrapher::compute_matching_pairs(bb);

    unindent();
    dln("[bb#: " , bb.index , "] END stack graph");

    d("next BBs:");
    for(auto next : bb.nextBBs())
        d(" " , next.get().index);
    dln();

    int exit_inputs = bb.enter_inputs + bb.effects.num_popped;

    for(auto next : bb.nextBBs()){
        if(next.get().visited){
            if(exit_inputs != next.get().enter_inputs){
                println("[FATAL] input size mismatch on edge from bb#" , bb.index , " to bb#" , next.get().index);
                println("Past inputs: " , next.get().enter_inputs , " current: " , bb.enter_inputs);
            }
            if(transformed_stack.size() != next.get().enter_stack_size){
                println("[FATAL] Control flow edge mismatch on edge from bb#" , bb.index , " to bb#" , next.get().index);
                println("Past stack size: " , next.get().enter_stack_size , " current: " , transformed_stack.size());
            }
        }else{
            next.get().enter_inputs = exit_inputs;
            next.get().enter_stack_size = (int) transformed_stack.size();
        }

        BasicBlock::match_registers_of_unvisited(bb, next);
        bb.visited = true;

        explore_graph_dfs(transformed_stack, next);
    }
    bb.visited = true; // need it again; what if bb.next is empty?
}

void StackGrapher::word_stack_graph(sWordptr wordptr) {
    dln();
    dln("Generate stack graph for all BBs of " , wordptr->name);

    // build stack graph
    NodeList stack;
    explore_graph_dfs(stack, wordptr->basic_blocks.front());

    // compute total effects of word
    // propagate Effects through a single control path
    // TODO awkward *cur_bb since there's some mischevious reference behavior
    auto *curr_bb = wordptr->basic_blocks.begin().base();
    Effects net_effects;
    while (!curr_bb->is_exit()){
        net_effects.acquire_side_effects(curr_bb->effects);
        curr_bb = &curr_bb->nextBBs().begin().base()->get();
    }
    wordptr->effects = net_effects;

    // last BB guaranteed to be return
    auto &lastBB = wordptr->basic_blocks.back();
    wordptr->effects.num_popped = lastBB.enter_inputs;
    wordptr->effects.num_pushed = lastBB.enter_stack_size;
}


