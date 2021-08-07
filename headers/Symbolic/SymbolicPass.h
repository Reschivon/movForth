

#ifndef MOVFORTH_SYM_SYMBOLICPASS_H
#define MOVFORTH_SYM_SYMBOLICPASS_H

#include "../Symbolic/Structures.h"
#include "../Interpretation/iWord.h"
#include "iWord.h"

namespace sym {
    class StackGrapher{
        // cached results of already computed words
        std::unordered_map<mfc::iWordptr, Wordptr> visited_words;
        Data symbolize_data(mfc::Data data);
        Wordptr conversion_pass(mfc::ForthWord *wordptr);
        void stack_graph_pass(Wordptr word);
        void branching_pass(Wordptr word);
        void retrieve_push_pop_effects(Wordptr word);
    public:
        Wordptr compute_effects_flattened(mfc::iWordptr input);
        Wordptr compute_effects(mfc::iWordptr original_word);
        Wordptr show_word_info(Wordptr wordptr);
    };
}

#endif
