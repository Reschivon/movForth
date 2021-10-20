#include "../../headers/Symbolic/Pass.h"

using namespace mov;

/**
 * Simulate the effects_without_push_pop of a single word on the stack
 * Push and pop from the given stack according to the Instruction provided
 * Generate registers between the elements on the stack and the Tnstruction
 * If the stack is empty, will add to the basic block's list of params

 * @param stack stack to be simulated and linked to/from
 * @param instruction the Instruction whose effects_without_push_pop will be used to simulate the stack
 * @param params if additional parameter are necessary, register then in this NodeList
 * @param register_gen the context for naming registers
 */
void Analysis::propagate_stack(NodeList &stack,
                     Instruction *instruction,
                     NodeList &params,
                     RegisterGen &register_gen) {

    const auto effects = instruction->linked_word->effects;

    // if the stack does not have at least (effects_without_push_pop.num_popped) items for us,
    // then we will create param nodes until there are enough items
    unsigned int nodes_from_stack = std::min(effects.num_popped, (int) stack.size());
    unsigned int nodes_from_params = std::max(effects.num_popped - (int)stack.size(), 0);

    // pop param nodes from stack to current instruction
    NodeList::move_top_elements(stack, instruction->pop_nodes,
                                (int) nodes_from_stack);

    while (nodes_from_params -- > 0)
        Node::link(params.new_front(),
                   instruction->pop_nodes.new_back(),
                   register_gen.get_param());

    // make empty output nodes
    for (int i = 0; i < effects.num_pushed; i++)
        instruction->push_nodes.new_back();

    // link output and param nodes that represent the same data
    for (auto out_in_pair : effects.out_in_pairs)
    {
        auto pop_node = instruction->pop_nodes[out_in_pair.second];
        auto push_node = instruction->push_nodes[out_in_pair.first];
        Node::link(pop_node, push_node, pop_node->edge_register);
    }

    // link output nodes to stack
    for (auto push_node : instruction->push_nodes)
        if(push_node->target != nullptr)
            Node::link_bidirection(push_node, stack.new_back(), push_node->edge_register);
        else
            Node::link_bidirection(push_node, stack.new_back(), register_gen.get());

    d("pops:");
    for (auto node : instruction->pop_nodes)
        d(" ", node->edge_register.to_string());
    dln();

    d("pushes:");
    for (auto thing : instruction->push_nodes)
        d(" ", thing->forward_edge_register.to_string());
    dln();
}

/**
 * Make a stack graph for an entire definition via multiple calls to propagate_stack()
 *
 * @param running_stack inital state of the stack before bb is supposed to run
 * @param bb the basic block whose definition with be graphed
 * @param register_gen context for naming registers
 * @return the running stack passed as parameter
 */
NodeList Analysis::basic_block_stack_graph(NodeList &running_stack, Block &bb,
                                           RegisterGen register_gen) {

    dln();
    dln("generate stack graph for all instructions in " , bb.name() );

    for (auto instruction : bb.instructions)
    {
        auto definee = instruction->linked_word;

        // update the bb's total Effects
        bb.effects_without_push_pop.acquire_side_effects_ignore_push_pop(definee->effects);

        // propagate the stack state
        dln();
        dln("[", definee->name, "]");
        Analysis::propagate_stack(running_stack, instruction, bb.params, register_gen);

        dln();
        dln("[stack:]");
        for (auto thing : running_stack)
            dln( thing->edge_register.to_string());
    }

    bb.outputs = running_stack;

    return running_stack;
}
