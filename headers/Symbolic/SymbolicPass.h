

#ifndef MOVFORTH_mov_symbolicPASS_H
#define MOVFORTH_mov_symbolicPASS_H

#include "../symbolic/Structures.h"
#include "../Interpretation/iWord.h"
#include "sWord.h"

namespace mov {
    class StackGrapher{
        // cached results of already computed words
        std::unordered_map<mov::iWordptr, sWordptr> visited_words;
        sData symbolize_data(mov::iData data);
        sWordptr conversion_pass(mov::ForthWord *wordptr);
        void stack_graph_pass(sWordptr word);
        void branching_pass(sWordptr word);
        void retrieve_push_pop_effects(sWordptr word);
    public:
        sWordptr compute_effects_flattened(mov::iWordptr input);
        sWordptr compute_effects(mov::iWordptr original_word);
        sWordptr show_word_info(sWordptr wordptr);
    };
}

#endif
