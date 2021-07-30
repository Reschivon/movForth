#include "../../headers/Symbolic/Structures.h"

using namespace sym;

/*
 * Visualization of the definition of base
 *
 *             1             2
 *       this <-> next_word <-> next_stack
 *
 * This function computes the stack data edges (1) and (2)
 * It also creates and returns next_stack with the correct
 * number of stack frames.
 */

void Stack::link_next_standard(Stack *next_stack, sym::Wordptr next_word, RegisterGenerator& register_generator){
    for(int put = 0; put < next_word->num_pushed; put++)
    {
        next_stack->nodes.push_back(
                !next_word->data_push.empty() ? new Node{.data = copy_of(
                        &(next_word->data_push[0]))} : new Node());
        next_word->push_nodes.push_back(new Node());

        Node *my_node = next_word->push_nodes.back();
        Node *their_node = next_stack->nodes.back();

        Node::link(my_node, their_node, register_generator.get());
    }
}

// assuming next_word is of type swap
void Stack::link_next_swap(Stack *next_stack, sym::Wordptr next_word, RegisterGenerator &register_generator) {
    Node* their_penult = next_stack->nodes.push_back(new Node());
    Node* their_top    = next_stack->nodes.push_back(new Node());

    Node* my_penult_push = next_word->push_nodes.push_back(new Node());
    Node* my_top_push    = next_word->push_nodes.push_back(new Node());

    RegisterID my_penult_pop = next_word->pop_nodes.penultimate()->backward_id;
    RegisterID my_top_pop = next_word->pop_nodes.back()->backward_id;

    Node::link(my_top_push, their_top, my_penult_pop);
    Node::link(my_penult_push, their_penult, my_top_pop);
}

// assuming next_word is of type dup
void Stack::link_next_dup(Stack *next_stack, sym::Wordptr next_word, RegisterGenerator& register_generator){
    Node* my_penult_push = next_word->push_nodes.push_back(new Node());
    Node* my_top_push    = next_word->push_nodes.push_back(new Node());

    Node* their_penult = next_stack->nodes.push_back(new Node());
    Node* their_top    = next_stack->nodes.push_back(new Node());

    RegisterID my_top_pop = next_word->pop_nodes.back()->backward_id;

    Node::link(my_top_push, their_penult, my_top_pop);
    Node::link(my_penult_push, their_top, my_top_pop);
}


Stack* Stack::propagate(sym::Wordptr base, sym::Wordptr next_word, RegisterGenerator& register_generator){
    auto next_stack = new Stack;

    int to_be_popped = next_word->num_popped;
    int not_popped = nodes.size() - to_be_popped;

    if(not_popped < 0){
        for(int i = 0; i < -not_popped; i++){

            auto input_word = new Word{.name = "__input"};
            input_word->push_nodes.push_back(new Node());
            Node *input_node = input_word->push_nodes.back();
            Node *this_node = new Node;

            Node::link(input_node, this_node, register_generator.get_param());

            nodes.push_back(this_node);

            base->num_popped++;
        }
    }

    int last_index = nodes.size() - 1;

    // link this stack and next word (top - middle)

    // Do not optimize out `take <= last_index` because the vector size
    // changes during iteration
    for(int take = nodes.size() - to_be_popped; take <= last_index; take++)
    {
        next_word->pop_nodes.push_back(new Node());

        Node *my_node = nodes[take];
        Node *their_node =next_word->pop_nodes.back();

        Node::link(my_node, their_node, my_node->backward_id);
    }

    // link this stack and next stack (middle - bottom)
    while (not_popped --> 0)
    {
        next_stack->nodes.push_back(new Node());

        Node *my_node = nodes[not_popped];
        Node *their_node = next_stack->nodes.back();

        Node::link(my_node, their_node, my_node->backward_id);
    }

    // link next stack and next word
    if(next_word->name == "swap")
        link_next_swap(next_stack, next_word, register_generator);
    else if(next_word->name == "dup")
        link_next_dup(next_stack, next_word, register_generator);
    else
        link_next_standard(next_stack, next_word, register_generator);

    return next_stack;
}
