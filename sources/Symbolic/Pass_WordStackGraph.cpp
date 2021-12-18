#include "../../headers/Symbolic/Pass.h"

using namespace mov;

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
    explore_graph_dfs(stack, wordptr->blocks.front(), wordptr->param_gen);

    // mark remaining registers as return
    NodeList &return_nodes = wordptr->blocks.back().outputs;
    wordptr->my_graphs_returns = return_nodes;
    for(auto &node : return_nodes){
        Node::redefine_preceding_type(node, Register::registerType::RETURN);
    }

    // compute push/pop effects for this word
    auto &lastBB = wordptr->blocks.back();

    uint total_accumulated_params = lastBB.initial_accumulated_params + lastBB.params.size();
    uint total_accumulated_stack_size = lastBB.outputs.size();

    wordptr->effects.num_popped = total_accumulated_params;
    wordptr->effects.num_pushed = total_accumulated_stack_size;

}


void Analysis::explore_graph_dfs(NodeList stack, Block &bb, RegisterGen param_gen) {
    if(bb.outputs_aligned)
        return;
    else
        bb.initial_accumulated_stack_size = (int) stack.size();

    dln();
    d("[" , bb.name() , "] START making stack graph");
    indent();

    // transformed stack == stack, but I want to make
    // it clear that stack has been modified

    // param_gen passed by reference, we want to know if params
    // got used in there
    NodeList transformed_stack = Analysis::basic_block_stack_graph(stack, bb, bb.params,
                                                                   bb.register_gen, param_gen);

    Analysis::compute_matching_pairs(bb);

    d("next BBs:");
    for(auto next : bb.nextBBs())
        d(" " , next.get().index);
    dln();

    unindent();
    dln("[" , bb.name() , "] FINSIHED making stack graph");

    uint final_accumulated_params = bb.initial_accumulated_params + bb.params.size();
    uint final_accumulated_stack_size = transformed_stack.size();

    dln("accumulated total params: ", final_accumulated_params);

    for(auto next : bb.nextBBs()){
        if(next.get().outputs_aligned){
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
    }

    // aka align registers between control flow edges
    bb.align_registers();

    for(auto next : bb.nextBBs())
        // param gen is passed by value becasue each control flow
        // edge tracks the accumulated params at that point
        explore_graph_dfs(transformed_stack, next, param_gen);
}



