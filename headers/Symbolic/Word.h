
#include "Structures.h"

#ifndef MOVFORTH_WORD_H
#define MOVFORTH_WORD_H

namespace sym{

    struct Effects{
        int num_popped = 0;
        int num_pushed = (0);

        bool consume_token = false; // ONLY for single token consumers, leave out "

        std::vector<Data> data_push;

        unsigned int compiled_slots = 0; // TODO still unsure how to handle memory

        enum interpret_state{NONE, TOCOMPILE, TOINTERPRET} interpret_state = NONE;

        bool define_new_word = false;

        // mapping of index for identical registers
        std::vector<std::pair<int, int>> output_input_pairs;

        void acquire_side_effects(Effects& other){
            // pop and push handled elsewhere

            if(other.consume_token) consume_token = true;

            append_to_vector(data_push, other.data_push);

            compiled_slots += other.compiled_slots;

            if(other.interpret_state != NONE)
                interpret_state = other.interpret_state;

            if(other.define_new_word) define_new_word = true;
        }

    };

    struct Instruction{
        NodeList pop_nodes;
        NodeList push_nodes;
        Wordptr linked_word;
    };

    class Word {
    public:
        std::string name;
        Effects effects;

        // components for graph
        //std::vector<Wordptr> definition;
        NodeList my_graphs_outputs;
        NodeList my_graphs_inputs;

        void definition_to_string();

        std::vector<Instruction> stack_effectors;
    };
}

#endif //MOVFORTH_WORD_H
