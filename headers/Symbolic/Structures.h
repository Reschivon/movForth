
#ifndef MOVFORTH_SYM_TYPES_H
#define MOVFORTH_SYM_TYPES_H

#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include "../Util.h"
#include "../Print.h"

/*
 * So many graphs! Graphs, graphs, everywhere!
 * Crisscrossed pointers and recursive structures,
 * multiple operations and general D I S S O N A N C E
 *
 * tl;dr it was a lot easier to use raw pointers than
 * to deal with references and shared_ptr
 */

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

    struct RegisterID{
        unsigned int ID = -1;
        enum registerType {UNDEF, NORMAL, PARAM} register_type = UNDEF;

        RegisterID operator++(int val){
            ID++;
            return *this;
        }

        std::string to_string(){
            switch (register_type)
            {
                case NORMAL:
                    return "(register "+ std::to_string(ID) + ")";
                case PARAM:
                    return "(input "+ std::to_string(ID) + ")";
                case UNDEF:
                    return "(undefined "+ std::to_string(ID) + ")";
                default:
                    return "(fucked "+ std::to_string(ID) + ")";
            }
        }
    };

    struct RegisterGenerator{
        RegisterID get(){
            return current_id++;
        }
        RegisterID get_param(){
            return current_param_id++;
        }
    //private:
        RegisterID current_id = {1, RegisterID::NORMAL};
        RegisterID current_param_id = {1, RegisterID::PARAM};
    };

    struct Node{
        Node* backward = nullptr;
        Node* forward = nullptr;
        RegisterID forward_id;
        RegisterID backward_id;

        Data* data = nullptr;

        static void link(Node* back, Node* front, RegisterID id){
            back->forward = front;
            front->backward = back;
            back->forward_id = id;
            front->backward_id = id;
        }
    };

    struct NodeList : std::vector<Node*>{
        Node* push_back(Node* push){
            std::vector<Node*>::push_back(push);
            return back();
        }
    };


    struct Stack{
        NodeList nodes;

        Stack* propagate(sym::Wordptr base, sym::Wordptr next_word, RegisterGenerator& register_generator);

    private:
        void link_next_standard(Stack *next_stack, sym::Wordptr next_word, RegisterGenerator& register_generator);
        void link_next_swap    (Stack *next_stack, sym::Wordptr next_word, RegisterGenerator& register_generator);
        void link_next_dup     (Stack *next_stack, sym::Wordptr next_word, RegisterGenerator& register_generator);
    };

    class Word {
    public:
        std::vector<Wordptr> definition;
        std::vector<Stack*> stacks;
        std::string name;
        NodeList pop_nodes;
        NodeList push_nodes;

        void acquire_side_effects(Word* other){
            // pop and push handled elsewhere

            if(other->consume_token) consume_token = true;

            append_to_vector(data_push, other->data_push);

            compiled_slots += other->compiled_slots;

            if(other->interpret_state != NONE)
                interpret_state = other->interpret_state;

            if(other->define_new_word) define_new_word = true;
        }

        // input
        unsigned int num_popped = 0;
        bool consume_token = false; // ONLY for single token consumers, leave out "

        // output
        unsigned int num_pushed = (0);
        std::vector<Data> data_push;
        unsigned int compiled_slots = 0; // TODO still unsure how to handle memory
        enum interpret_state{ NONE, TOCOMPILE, TOINTERPRET} interpret_state = NONE;
        bool define_new_word = false;

        void definition_to_string(){
            println("[", name, "]");
            println("input parameters: ", num_popped);
            println("output frames: ", num_pushed);
            print();
            for(auto thing : definition)
                print(thing->name, " ");
           print();
        }
    };

}

#endif
