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

void explore_graph_dfs(NodeList stack, BasicBlock &bb, sWordptr base){
    if(bb.visited){
        if(stack.size() != bb.enter_stack_size){
            println("Control flow edge inconsistency on edge entering bb#" , bb.index);
            println("Past stack size: " , bb.enter_stack_size , " current: " , stack.size());
        }
        return;
    }else{
        bb.enter_stack_size = stack.size();
    }

    RegisterGen register_gen(bb.index);

    println();
    println("[bb#: " , bb.index , "] BEGIN stack graph");
    indent();

    NodeList transformed_stack = StackGrapher::stack_graph_pass_bb(stack, bb, register_gen);

    unindent();
    println("[bb#: " , bb.index , "] END stack graph");
    println("push:" , bb.my_graphs_outputs.size(),
            " pop:" , bb.my_graphs_inputs.size());

    print("next: ");
    for(auto next : bb.nextBBs())
        print(next.get().index);
    println();

    int exit_inputs = bb.enter_inputs + bb.effects.num_popped;

    for(auto next : bb.nextBBs()){
        if(next.get().visited){
            if(exit_inputs != next.get().enter_inputs){
                println("input size inconsistency on edge from bb#" , bb.index , " to bb#" , next.get().index);
                println("Past inputs: " , next.get().enter_inputs , " current: " , bb.enter_inputs);
                continue;
            }
            if(transformed_stack.size() != next.get().enter_stack_size){
                println("Control flow edge inconsistency on edge from bb#" , bb.index , " to bb#" , next.get().index);
                println("Past stack size: " , next.get().enter_stack_size , " current: " , transformed_stack.size());
                continue;
            }
        }

        next.get().enter_inputs = exit_inputs;
        next.get().enter_stack_size = transformed_stack.size();
        explore_graph_dfs(transformed_stack, next, base);
    }

    bb.visited = true;
}

void StackGrapher::bb_cyclic_pass(sWordptr wordptr) {
    println();
    println("BB cyclic pass");

    NodeList stack;
    explore_graph_dfs(stack, wordptr->basic_blocks.front(), wordptr);
}


