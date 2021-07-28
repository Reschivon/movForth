#include "../../headers/Symbolic/Structures.h"

using namespace sym;

Stack* Stack::propagate(sym::Wordptr base, sym::Wordptr next_word, RegisterGenerator& register_generator){
    auto next_stack = new Stack;

    print();
    println("simulate ", next_word->name, " on ", base->name);

    int to_be_popped = next_word->num_popped;
    int not_popped = nodes.size() - to_be_popped;

    if(not_popped < 0){
        for(int i = 0; i < -not_popped; i++){
            println(next_word->name + " needed one more input");

            auto input_word = new Word{.name = "__input"};
            input_word->push_nodes.push_back(new Node());
            Node *input_node = input_word->push_nodes.back();
            Node *this_node = new Node;

            println("link new param ", next_word->name, " ", register_generator.current_param_id.to_string());
            Node::link(input_node, this_node, register_generator.get_param());

            nodes.push_back(this_node);

            base->num_popped++;
        }
    }

    int last_index = nodes.size() - 1;

    // link next word and this stack (top - middle)
    // Do not optimize out `take <= last_index` because the vector size
    // changes during iteration
    for(int take = nodes.size() - to_be_popped; take <= last_index; take++)
    {
        next_word->pop_nodes.push_back(new Node());

        println(next_word->name, " acquired new pop node");

        Node *my_node = nodes[take];
        Node *their_node =next_word->pop_nodes.back();

        println("link pop ", next_word->name, " ", my_node->backward_id.to_string());
        Node::link(my_node, their_node, my_node->backward_id);
    }

    // link next stack and this stack (middle - bottom)
    while (not_popped --> 0)
    {
        next_stack->nodes.push_back(new Node());

        Node *my_node = nodes[not_popped];
        Node *their_node = next_stack->nodes.back();

        println("link maintain ", my_node->backward_id.to_string());
        Node::link(my_node, their_node, my_node->backward_id);
    }

    // next stack to next word
    for(int put = 0; put < next_word->num_pushed; put++) {
        next_stack->nodes.push_back(!next_word->data_push.empty() ? new Node{.data = copy_of(&(next_word->data_push[0]))} : new Node());
        next_word->push_nodes.push_back(new Node());

        println(next_word->name, " acquired new push node");

        Node* my_node = next_word->push_nodes.back();
        Node* their_node = next_stack->nodes.back();

        println("link push ", next_word->name, " ", register_generator.current_id.to_string());
        Node::link(my_node, their_node, register_generator.get());
    }

    return next_stack;
}

