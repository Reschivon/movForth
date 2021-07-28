#include "../../headers/Cyclic/Structures.h"

using namespace sym;

Stack* Stack::propagate(sym::Wordptr base, sym::Wordptr seq){
    auto next_stack = new Stack;

    std::cout << "add " << seq->name << " to " << base->name << "'s stack" << std::endl;

    int to_pop = seq->stack_pop;
    int not_popped = data.size() - to_pop;
    int last_index = data.size() - 1;
    if(not_popped < 0){
        for(int i = 0; i < -not_popped; i++){
            std::cout << seq->name << " needed one more input" << std::endl;
            data.push_back(Node{.backward = new Word{.name = "__input"}});
            base->stack_pop++;
        }
    }

    // next word to this stack
    for (int take = last_index; take > last_index - to_pop; take--)
        seq->node_array.push_back(data[take]);

    // next stack to this
    while (not_popped --> 0)
        next_stack->data.push_back(Node{.backward = &data[not_popped - 1]});

    // next stack to next word
    for(int put = 0; put < seq->stack_push; put++) {
        Node* point = seq;
        next_stack->data.push_back(Node{point});
    }

    std::cout << "stack size: " << next_stack->data.size() << std::endl;

    return next_stack;
}

