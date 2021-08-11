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
             else  bb.enter_stack_size = (int) stack.size();

    RegisterGen register_gen((int) bb.index);

    println();
    println("[bb#: " , bb.index , "] BEGIN stack graph");
    indent();

    NodeList transformed_stack = StackGrapher::basic_block_stack_graph(stack,
                                                                       bb,
                                                                       register_gen);
    StackGrapher::compute_matching_pairs(bb);

    unindent();
    println("[bb#: " , bb.index , "] END stack graph");
    println("push:" , bb.effects.num_pushed,
            " pop:" , bb.effects.num_popped);

    print("next: ");
    for(auto next : bb.nextBBs())
        print(next.get().index);
    println();

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
        }

        next.get().enter_inputs = exit_inputs;
        next.get().enter_stack_size = (int) transformed_stack.size();
        explore_graph_dfs(transformed_stack, next);
    }

    bb.visited = true;
}

void StackGrapher::word_stack_graph(sWordptr wordptr) {
    println();
    println("BB cyclic pass");

    NodeList stack;
    explore_graph_dfs(stack, wordptr->basic_blocks.front());

    // propagate Effects through a single control path
    auto &curr_bb = wordptr->basic_blocks.front();
    Effects net_effects;
    while (!curr_bb.is_exit()){
        net_effects.acquire_side_effects(curr_bb.effects);
        curr_bb = curr_bb.nextBBs().front();
    }
    wordptr->effects = net_effects;

    // last BB guaranteed to be return
    auto &lastBB = wordptr->basic_blocks.back();
    wordptr->effects.num_popped = lastBB.enter_inputs;
    wordptr->effects.num_pushed = lastBB.enter_stack_size;

}


