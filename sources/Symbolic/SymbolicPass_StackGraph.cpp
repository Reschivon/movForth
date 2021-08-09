#include "../../headers/Symbolic/SymbolicPass.h"

using namespace mov;


void propagate_stack(NodeList &stack, Instruction *instruction, BasicBlock &base,
                     RegisterGenerator &register_generator) {
    auto effects = instruction->linked_word->effects;

    dln("pops: ", effects.num_popped, " pushes: ", effects.num_pushed);

    // add necessary input nodes
    unsigned int nodes_from_input = 0;
    unsigned int nodes_from_stack = effects.num_popped;
    if (effects.num_popped > stack.size())
    {
        nodes_from_input = effects.num_popped - stack.size();
        nodes_from_stack = stack.size();
    }
    while (nodes_from_input-- > 0)
    {
        Register input_register = register_generator.get_param();
        auto input_node = new Node;
        instruction->pop_nodes.push_back(new Node{
            .target = input_node,
            .edge_register = input_register});
        base.my_graphs_inputs.push_front(input_node);
        dln("needs extra input ", input_register.to_string());
    }

    // pop input nodes from stack
    NodeList::move_top_elements(stack, instruction->pop_nodes, (int) nodes_from_stack);

    // make empty output nodes
    for (int i = 0; i < effects.num_pushed; i++)
        instruction->push_nodes.push_back(new Node);

    // cross internally
    for (auto out_in_pair : effects.out_in_pairs)
    {
        // dln("cross internal o", out_in_pair.second, " with ",out_in_pair.first);

        auto pop_node = instruction->pop_nodes[out_in_pair.second];
        auto push_node = instruction->push_nodes[out_in_pair.first];

        Node::link(pop_node, push_node, pop_node->edge_register);
    }

    // push output nodes to stack
    for (auto push_node : instruction->push_nodes)
    {
        Register aRegister;
        if (push_node->target != nullptr) // it was linked in the [cross internally] step
            aRegister = push_node->edge_register;
        else
            aRegister = register_generator.get();

        stack.push_back(new Node);
        Node::link_bidirection(push_node, stack.back(), aRegister);
    }

    println("pop from ids:");
    for (auto node : instruction->pop_nodes)
        println("   ", node->edge_register.to_string());
    dln("push to ids");
    for (auto thing : instruction->push_nodes)
        dln("   ", thing->forward_edge_register.to_string());
}


void stack_graph_pass_bb(BasicBlock &bb) {
    // the stack is a constantly updated list of
    // loose pop nodes that the next bb might need
    auto &running_stack = *(new NodeList);
    RegisterGenerator register_generator;

    for (auto instruction : bb.instructions)
    {
        auto definee = instruction->linked_word;

        // update the bb's total Effects
        bb.effects.acquire_side_effects(definee->effects);

        // propagate the stack state
        dln();
        dln("[", definee->name, "]");
        propagate_stack(running_stack, instruction, bb, register_generator);

        dln();
        dln("intermediate stack: ");
        for (auto thing : running_stack)
        {
            dln("\t", thing->edge_register.to_string());
        }
    }

    // the pushed side effect is the same as remaining stack frames
    bb.my_graphs_outputs = running_stack;
}

void StackGrapher::stack_graph_pass(sWordptr wordptr) {
    for(auto &bb : wordptr->basic_blocks){
        println();
        println("Stack graph for [" , wordptr->name , ": " , bb.index , "]");
        indent();
        stack_graph_pass_bb(bb);
        unindent();
        println("[" , wordptr->name , "]" ,
                " push:" , bb.my_graphs_inputs.size() ,
                " pop:" , bb.my_graphs_outputs.size());
    }
}
