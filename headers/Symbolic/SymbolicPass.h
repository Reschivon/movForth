

#ifndef MOVFORTH_SYM_SYMBOLICPASS_H
#define MOVFORTH_SYM_SYMBOLICPASS_H

#include "../Symbolic/Structures.h"
#include "../Interpretation/iWord.h"
#include "sWord.h"

namespace sym {
    class StackGrapher{
        // cached results of already computed words
        std::unordered_map<mfc::iWordptr, sWordptr> visited_words;
        Data symbolize_data(mfc::Data data);
        sWordptr conversion_pass(mfc::ForthWord *wordptr);
        void stack_graph_pass(sWordptr word);
        void branching_pass(sWordptr word);
        void retrieve_push_pop_effects(sWordptr word);
    public:
        sWordptr compute_effects_flattened(mfc::iWordptr input);
        sWordptr compute_effects(mfc::iWordptr original_word);
        sWordptr show_word_info(sWordptr wordptr);
    };
}

#endif
