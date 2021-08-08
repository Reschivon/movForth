
#ifndef MOVFORTH_EFFECTS_H
#define MOVFORTH_EFFECTS_H

namespace mov{
    struct Effects{
        int num_popped = 0;
        int num_pushed = (0);

        bool consume_token = false; // ONLY for single token consumers, leave out "

        unsigned int compiled_slots = 0; // TODO still unsure how to handle memory

        enum interpret_state{NONE, TOCOMP, TOIMM} interpret_state = interpret_state::NONE;

        bool define_new_word = false;

        // mapping of index for identical registers
        std::vector<std::pair<int, int>> out_in_pairs;

        void acquire_side_effects(Effects& other){
            // pop and push handled elsewhere

            if(other.consume_token) consume_token = true;

            compiled_slots += other.compiled_slots;

            if(other.interpret_state != interpret_state::NONE)
                interpret_state = other.interpret_state;

            if(other.define_new_word) define_new_word = true;
        }

        static Effects neutral;
    };

}

#endif //MOVFORTH_EFFECTS_H
