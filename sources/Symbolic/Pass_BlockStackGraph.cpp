#include "../../headers/Symbolic/Pass.h"

using namespace mov;


void Analysis::propagate_stack(NodeList &stack, Instruction *instruction, NodeList &params, RegisterGen &register_gen,
                               RegisterGen &param_gen) {

    const auto effects = instruction->linked_word->effects;
    instruction->pop_nodes.clear(); // not sure why it sometimes already has stuff

    // if the stack does not have at least (effects_without_push_pop.num_popped) items for us,
    // then we will create param nodes until there are enough items
    unsigned int nodes_from_stack = std::min(effects.num_popped, stack.size());
    // we must make this comparison in signed integers
    unsigned int nodes_from_params = std::max((int)effects.num_popped - (int)stack.size(), 0);

    // pop input nodes from stack to current instruction
    NodeList::move_top_elements(stack, instruction->pop_nodes,
                                (int) nodes_from_stack);

    while (nodes_from_params -- > 0)
        Node::link_bidirection(params.new_top(),
                   instruction->pop_nodes.new_top(),
                   param_gen.get_param());

    // make empty output nodes
    for (int i = 0; i < effects.num_pushed; i++)
        instruction->push_nodes.new_top();

    // link output and param nodes that represent the same data
    for (auto out_in_pair : effects.out_in_pairs)
    {
        auto pop_node = instruction->pop_nodes[out_in_pair.second];
        auto push_node = instruction->push_nodes[out_in_pair.first];
        Node::link(pop_node, push_node, pop_node->backward_edge_register);
    }

    // link output nodes to stack
    for (auto push_node : instruction->push_nodes)
        if(push_node->prev_node != nullptr)
            Node::link_bidirection(push_node, stack.new_top(), push_node->backward_edge_register);
        else
            Node::link_bidirection(push_node, stack.new_top(), register_gen.get());

    dln("pops: first -> ");
    for (auto node : instruction->pop_nodes)
        d(" ", node->backward_edge_register.to_string());

    dln("pushes: first -> ");
    for (auto thing : instruction->push_nodes)
        d(" ", thing->forward_edge_register.to_string());
}

void Analysis::propagate_stack_fromlocal(NodeList &stack, Instruction *instruction, NodeList &params, sWordptr parent) {

    instruction->pop_nodes.clear(); // not sure why it sometimes already has stuff

    Register locals_register = parent->locals.at(instruction->data.as_local());

    Node::link_bidirection(instruction->push_nodes.new_top(), stack.new_top(), locals_register);

    dln("fromLocal takes data from register ", locals_register.to_string());
}


void Analysis::propagate_stack_tolocal(NodeList &stack, Instruction *instruction,
                                       NodeList &params, RegisterGen &param_gen,
                                       RegisterGen &register_gen, sWordptr parent) {

    instruction->pop_nodes.clear(); // not sure why it sometimes already has stuff

    bool node_from_param = stack.empty();
    bool node_from_stack = !stack.empty();

    // pop input nodes from stack to current instruction
    if(node_from_stack)
        NodeList::move_top_elements(stack, instruction->pop_nodes, 1);

    if(node_from_param)
        Node::link_bidirection(params.new_top(),
                               instruction->pop_nodes.new_top(),
                               param_gen.get_param());

    const auto &curr_local = instruction->data.as_local();
    const auto &local_search = parent->locals.find(curr_local);
    if(local_search == parent->locals.end()) {
        // we haven't already allocated register space for this local
        const Register& local_reg = register_gen.get();
        parent->locals.insert(std::make_pair(curr_local, local_reg));

        dln("toLocal gets data from register ", instruction->pop_nodes[0]->backward_edge_register.to_string());
        dln("toLocal sends data to special register ", local_reg.to_string());
    }else{
        // we already encountered this local, its register is stored
        dln("toLocal gets data from register ", instruction->pop_nodes[0]->backward_edge_register.to_string());
        dln("toLocal sends data to special register ", local_search->second.to_string());
    }



}


NodeList Analysis::basic_block_stack_graph(NodeList &running_stack, Block &bb, NodeList &params,
                                           RegisterGen &register_gen, RegisterGen &param_gen) {

    dln();

    // a copy of the stack
    bb.inputs = running_stack;

    // set inputs to pregenerated register_names during rgister alignment
    dln("Loading initial registers");
    indent();
    int i = 0;
    for(auto *node : bb.inputs){
        dln("Load ", bb.initial_registers[i].to_string());
        node->backward_edge_register = bb.initial_registers[i];
        i++;
    }
    unindent();

    for (auto instruction : bb.instructions)
    {
        auto definee = instruction->linked_word;

        // update the bb's total Effects
        bb.effects_without_push_pop.acquire_side_effects_ignore_push_pop(definee->effects);

        // propagate the stack state
        dln();
        dln("[", definee->name, "]");

        if(definee->id == primitive_words::TOLOCAL)
            Analysis::propagate_stack_tolocal(running_stack, instruction,
                                              params, param_gen, register_gen, bb.parent);
        else if (definee->id == primitive_words::FROMLOCAL)
            Analysis::propagate_stack_fromlocal(running_stack, instruction, params, bb.parent);
        else
            Analysis::propagate_stack(running_stack, instruction,
                                      params, register_gen, param_gen);

        dln();
        dln("[stack:]");
        for (auto thing : running_stack)
            dln( thing->backward_edge_register.to_string());
        dln("^ top ^");
    }

    // a copy of the stack
    bb.outputs = running_stack;

    return running_stack;
}
