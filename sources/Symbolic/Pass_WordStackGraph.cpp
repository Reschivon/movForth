#include "../../headers/Symbolic/Pass.h"

using namespace mov;

void Analysis::explore_graph_dfs(NodeList stack, Block &bb, NodeList &params){
    if(bb.visited)
        return;
    else
        bb.initial_accumulated_stack_size = (int) stack.size();

    dln();
    d("[" , bb.name() , "] START making stack graph");
    indent();

    // transformed stack == stack, but I want to make
    // it clear that stack has been modified
    NodeList transformed_stack = Analysis::basic_block_stack_graph(stack, bb, params, bb.register_gen);

    Analysis::compute_matching_pairs(bb);

    d("next BBs:");
    for(auto next : bb.nextBBs())
        d(" " , next.get().index);
    dln();

    unindent();
    dln("[" , bb.name() , "] FINSIHED making stack graph");

    uint final_accumulated_params = params.size();
    uint final_accumulated_stack_size = transformed_stack.size();

    dln("accumulated total params: ", final_accumulated_params);

    for(auto next : bb.nextBBs()){
        if(next.get().visited){
            if(final_accumulated_params != next.get().initial_accumulated_params){
                println("[FATAL] input size mismatch on edge from " , bb.name() , " to " , next.get().name());
                println("Past input num: " , next.get().initial_accumulated_params , " current: " , bb.initial_accumulated_params);
            }
            if(final_accumulated_stack_size != next.get().initial_accumulated_stack_size){
                println("[FATAL] Control flow edge mismatch on edge from " , bb.name() , " to " , next.get().name());
                println("Past stack size: " , next.get().initial_accumulated_stack_size , " current: " , transformed_stack.size());
            }
        }else{
            next.get().initial_accumulated_params = final_accumulated_params;
            next.get().initial_accumulated_stack_size = final_accumulated_stack_size;
        }

        // aka align registers between control flow edges
        Block::align_registers(bb, next);

        // need to set this after align_registers but before
        // calling explore_graph_dfs
        bb.visited = true;

        explore_graph_dfs(transformed_stack, next, params);
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

    // build stack graph
    NodeList stack;
    // This function does all the heavy lifting
    explore_graph_dfs(stack, wordptr->blocks.front(), wordptr->my_graphs_params);

    // compute total effects_without_push_pop of word
    // propagate Effects through a single control path
    // TODO awkward *cur_bb since there's some mischevious reference behavior
    auto *curr_bb = wordptr->blocks.begin().base();
    Effects net_effects;
    while (!curr_bb->is_exit()){
        net_effects.acquire_side_effects_ignore_push_pop(curr_bb->effects_without_push_pop);
        curr_bb = &curr_bb->nextBBs().begin().base()->get();
    }
    wordptr->effects = net_effects;

    // mark remaining registers as return
    NodeList &return_nodes = wordptr->blocks.back().outputs;
    for(auto node : return_nodes){
        Node::redefine_preceding_type(node, Register::registerType::RETURN);
    }


    // last BB guaranteed to be return
    auto &lastBB = wordptr->blocks.back();
    wordptr->effects.num_popped = lastBB.initial_accumulated_params;
    wordptr->effects.num_pushed = lastBB.initial_accumulated_stack_size;

    uint total_accumulated_params = lastBB.initial_accumulated_params + lastBB.params.size();
    uint total_accumulated_stack_size = lastBB.outputs.size();

//    dln("popped (from last BB): ", total_accumulated_params);
//    dln("pushed (from last BB): ", total_accumulated_stack_size);

    wordptr->effects.num_popped = total_accumulated_params;
    wordptr->effects.num_pushed = total_accumulated_stack_size;


}


