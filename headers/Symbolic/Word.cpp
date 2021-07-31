
#include "Word.h"

using namespace sym;

void Word::definition_to_string() {
    println("[", name, "]\n");
    println("input parameters: ", effects.num_popped, "output frames: ", effects.num_pushed, "\n");
    for(auto thing : definition)
        print(thing->name, " ");
    print();
}

void Word::propagate(Wordptr sub_word, NodeList &stack, RegisterGenerator &register_generator) {
    dln();
    dln("[", sub_word->name, "]");
    dln("pops: ", sub_word->effects.num_popped, " pushes: ", sub_word->effects.num_pushed);

    // add necessary input nodes
    unsigned int nodes_from_input = 0;
    unsigned int nodes_from_stack = sub_word->effects.num_popped;
    if(sub_word->effects.num_popped > stack.size()){
        nodes_from_input = sub_word->effects.num_popped - stack.size();
        nodes_from_stack = stack.size();
    }
    while (nodes_from_input --> 0){
        Node *input_node = new Node;
        Register input_register = register_generator.get_param();
        sub_word->pop_nodes.push_back(new Node{
                .target = input_node,
                .edge_register = input_register});

        my_graphs_inputs.push_back(input_node);
        dln("needs extra input; id: ", input_register.to_string());
    }

    // pop input nodes from stack
    NodeList::move_top_elements(stack, sub_word->pop_nodes, nodes_from_stack);

    // make empty output nodes
    sub_word->push_nodes.clear();
    for(int i = 0; i < sub_word->effects.num_pushed; i++){
        sub_word->push_nodes.push_back(new Node);
    }

    // cross internally
    for(auto out_in_pair : sub_word->effects.output_input_pairs){

        auto pop_node = sub_word->pop_nodes[out_in_pair.second];
        auto push_node = sub_word->push_nodes[out_in_pair.first];

        Node::link(pop_node, push_node, pop_node->edge_register);
    }

    // push output nodes to stack
    for(auto push_node : sub_word->push_nodes){
        Register aRegister;
        if(push_node->target != nullptr){ // it was linked in the [cross internally] step
            aRegister = push_node->edge_register;
        }else{
            aRegister = register_generator.get();
        }

        stack.push_back(new Node);
        Node::link_bidirection(push_node, stack.back(), aRegister);
    }
}
