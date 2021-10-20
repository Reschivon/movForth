#include "../../headers/Symbolic/Pass.h"

using namespace mov;

struct Conflict{
    Block *from, *to;
    Conflict(Block *from, Block *to)
        : from(from), to(to) {}
    [[nodiscard]] std::string to_string() const{
        return "conflict from " + from->name() +
                " to " + to->name();
    }

};

void Analysis::explore_graph_dfs(NodeList stack, Block &bb){
    if(bb.visited)
        return;
    else
        bb.initial_stack_size = (int) stack.size();

    dln();
    dln("[" , bb.name() , "] BEGIN stack graph");
    indent();

    // transformed stack == stack, but I want to make
    // it clear that stack has been modified
    NodeList transformed_stack = Analysis::basic_block_stack_graph(stack, bb, bb.register_gen);

    Analysis::compute_matching_pairs(bb);

    d("next BBs:");
    for(auto next : bb.nextBBs())
        d(" " , next.get().index);
    dln();

    unindent();
    dln("[" , bb.name() , "] END stack graph");

    int final_accumulated_params = bb.initial_accumulated_params + bb.params.size();
    int final_accumulated_stack_size = transformed_stack.size();

    for(auto next : bb.nextBBs()){
        if(next.get().visited){
            if(final_accumulated_params != next.get().initial_accumulated_params){
                println("[FATAL] input size mismatch on edge from " , bb.name() , " to " , next.get().name());
                println("Past input num: " , next.get().initial_accumulated_params , " current: " , bb.initial_accumulated_params);
            }
            if(final_accumulated_stack_size != next.get().initial_stack_size){
                println("[FATAL] Control flow edge mismatch on edge from " , bb.name() , " to " , next.get().name());
                println("Past stack size: " , next.get().initial_stack_size , " current: " , transformed_stack.size());
            }
        }else{
            next.get().initial_accumulated_params = final_accumulated_params;
            next.get().initial_stack_size = (int) transformed_stack.size();
        }

        Block::match_registers_of_unvisited(bb, next);
        bb.visited = true;

        explore_graph_dfs(transformed_stack, next);
    }
    bb.visited = true; // need it again; what if bb.nextBBs() is empty?
}

/**
 * Generate stack graph for every basic block within wordptr
 * Also make sure register names match between basic block edges
 * Ensure that merging Bbs have the same stack size
 * Set the new effects_without_push_pop of wordptr
 * @param wordptr the word whose definition will be graphed
 */
void Analysis::word_stack_graph(sWordptr wordptr) {
    dln();
    dln("Generate stack graph for all BBs of " , wordptr->name);

    // build stack graph
    NodeList stack;
    // This function does all the heavy lifting
    explore_graph_dfs(stack, wordptr->basic_blocks.front());

    // compute total effects_without_push_pop of word
    // propagate Effects through a single control path
    // TODO awkward *cur_bb since there's some mischevious reference behavior
    auto *curr_bb = wordptr->basic_blocks.begin().base();
    Effects net_effects;
    while (!curr_bb->is_exit()){
        net_effects.acquire_side_effects_ignore_push_pop(curr_bb->effects_without_push_pop);
        curr_bb = &curr_bb->nextBBs().begin().base()->get();
    }
    wordptr->effects = net_effects;

    // last BB guaranteed to be return
    auto &lastBB = wordptr->basic_blocks.back();
    wordptr->effects.num_popped = lastBB.initial_accumulated_params;
    wordptr->effects.num_pushed = lastBB.initial_stack_size;

    int total_accumulated_params = lastBB.initial_accumulated_params + lastBB.params.size();
    int total_accumulated_stack_size = lastBB.outputs.size();

//    dln("popped (from last BB): ", total_accumulated_params);
//    dln("pushed (from last BB): ", total_accumulated_stack_size);

    wordptr->effects.num_popped = total_accumulated_params;
    wordptr->effects.num_pushed = total_accumulated_stack_size;


}


