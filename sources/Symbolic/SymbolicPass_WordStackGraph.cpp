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

    println();
    println("[bb#: " , bb.index , "] BEGIN stack graph");
    indent();

    // transformed stack == stack, but I want to make
    // it clear that stack has been modified
    NodeList transformed_stack = StackGrapher::basic_block_stack_graph(stack,
                                                                       bb, bb.register_gen);
    StackGrapher::compute_matching_pairs(bb);

    unindent();
    println("[bb#: " , bb.index , "] END stack graph");
    println("push:" , bb.effects.num_pushed,
            " pop:" , bb.effects.num_popped);

    print("next BBs:");
    for(auto next : bb.nextBBs())
        print(" " , next.get().index);
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
            if(!bb.visited){
                println("bb#" , bb.index , "not visited, bb#", next.get().index, "visited");
                // prev not visited, post visited
                auto reg_it = next.get().enter_registers.begin();
                auto out_it = bb.my_graphs_outputs.begin();
                while (reg_it != next.get().enter_registers.end()){
                    Node::redefine_preceding_edge(*out_it, *reg_it);
                    reg_it++; out_it++;
                }
            }
        }else{
            next.get().enter_inputs = exit_inputs;
            next.get().enter_stack_size = (int) transformed_stack.size();
            if(bb.visited){
                println("bb#" , bb.index , "visited, bb#", next.get().index, "not visited");
                // prev visited, post not visited
                for(Node *thing : bb.my_graphs_outputs){
                    next.get().enter_registers.push_back(thing->forward_edge_register);
                }
            }else{
                println("bb#" , bb.index , "not visited, bb#", next.get().index, "not visited");
                bb.visited = true;
                println("BB#" , bb.index , " VISITED");
                // prev not visited, post not visited
                for(Node *thing : bb.my_graphs_outputs){
                    auto reg = next.get().register_gen.get();
                    next.get().enter_registers.push_back(reg);
                    Node::redefine_preceding_edge(thing, reg);
                }
            }
        }
        explore_graph_dfs(transformed_stack, next);
    }
    bb.visited = true;
}

void StackGrapher::word_stack_graph(sWordptr wordptr) {
    println();
    println("Generate stack graph for all BBs of " , wordptr->name);

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


