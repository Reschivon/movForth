
#ifndef MOVFORTH_CYC_TYPES_H
#define MOVFORTH_CYC_TYPES_H

#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include "../Util.h"

namespace sym {

    class Word;

    typedef Word *Wordptr;

    class Data {
        std::variant<int, Wordptr, std::nullptr_t> data{};
    public:
        bool is_num() {return data.index() == 0;}
        bool is_xt() {return data.index() == 1;}
        bool is_unknown() {return data.index() == 2;};

        int as_num() {return std::get<int>(data);}
        Wordptr as_xt() {return std::get<Wordptr>(data);}

        explicit Data(std::variant<int, Wordptr, std::nullptr_t> data) : data(data) {}
    };

    struct Node{
        Node* backward = nullptr;
    };

    struct Stack{
        std::vector<Node> data;

        Stack* propagate(sym::Wordptr base, sym::Wordptr seq);
    };


    class Word : public Node{
    public:
        std::vector<Wordptr> definition;
        std::string name;
        std::vector<Node> node_array;

        Stack* first_stack;
        Stack* last_stack;

        void acquire_side_effects(Word* other){
            // pop and push handled elsewhere

            if(other->consume_token) consume_token = true;

            append_to_vector(data_push, other->data_push);

            compiled_slots += other->compiled_slots;

            if(other->interpret_state != NONE)
                interpret_state = other->interpret_state;

            if(other->new_word) new_word = true;
        }

        // input
        int stack_pop = 0;
        bool consume_token = false; // ONLY for single token consumers, leave out "

        // output
        int stack_push = (0);
        std::vector<Data> data_push;
        int compiled_slots = 0; // TODO still unsure how to handle memory
        enum interpret_state{ NONE, TOCOMPILE, TOINTERPRET} interpret_state = NONE;
        bool new_word = false;

        void definition_to_string(){
            std::cout << "[" << name << "]" << std::endl;
            std::cout << "input parameters: " << stack_pop << std::endl;
            std::cout << "output frames: " << stack_push << std::endl;
            std::cout << std::endl;
            for(auto thing : definition)
                std::cout << thing->name << " ";
            std::cout << std::endl;
        }
    };

}

#endif
