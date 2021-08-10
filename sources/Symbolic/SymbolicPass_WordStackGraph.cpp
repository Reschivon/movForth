#include "../../headers/Symbolic/SymbolicPass.h"

using namespace mov;

void propagate_stack(NodeList &stack,
                     Instruction *instruction,
                     BasicBlock &base,
                     RegisterGen &register_gen) {

    const auto effects = instruction->linked_word->effects;

    // if the stack does not have at least (effects.num_popped) items for us,
    // then we will create input nodes until there are enough items
    unsigned int nodes_from_stack = std::min(effects.num_popped, (int) stack.size());
    unsigned int nodes_from_input = std::max(effects.num_popped - (int)stack.size(), 0);

    // pop input nodes from stack to current instruction
    NodeList::move_top_elements(stack, instruction->pop_nodes,
                                (int) nodes_from_stack);

    while (nodes_from_input --> 0)
        Node::link(base.my_graphs_inputs.new_front(),
                   instruction->pop_nodes.new_back(),
                   register_gen.get_input());

    // make empty output nodes
    for (int i = 0; i < effects.num_pushed; i++)
        instruction->push_nodes.new_back();

    // link output and input nodes that represent the same data
    for (auto out_in_pair : effects.out_in_pairs)
    {
        auto pop_node = instruction->pop_nodes[out_in_pair.second];
        auto push_node = instruction->push_nodes[out_in_pair.first];
        Node::link(pop_node, push_node, pop_node->edge_register);
    }

    // link output nodes to stack
    for (auto push_node : instruction->push_nodes)
        if(push_node->target)
            Node::link_bidirection(push_node, stack.new_back(), push_node->edge_register);
        else
            Node::link_bidirection(push_node, stack.new_back(), register_gen.get());

    print("pops:");
    for (auto node : instruction->pop_nodes)
        print(" ", node->edge_register.to_string());
    println();

    print("pushes:");
    for (auto thing : instruction->push_nodes)
        print(" ", thing->forward_edge_register.to_string());
    println();
}

NodeList StackGrapher::basic_block_stack_graph(NodeList &running_stack, BasicBlock &bb,
                                               RegisterGen register_gen) {

    for (auto instruction : bb.instructions)
    {
        auto definee = instruction->linked_word;

        // update the bb's total Effects
        bb.effects.acquire_side_effects(definee->effects);

        // propagate the stack state
        dln();
        dln("[", definee->name, "]");
        propagate_stack(running_stack, instruction, bb, register_gen);

        dln();
        dln("[stack:]");
        for (auto thing : running_stack)
            dln( thing->edge_register.to_string());
    }
    return running_stack;
}
