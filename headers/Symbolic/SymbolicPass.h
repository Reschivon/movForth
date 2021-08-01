

#ifndef MOVFORTH_SYM_SYMBOLICPASS_H
#define MOVFORTH_SYM_SYMBOLICPASS_H

#include "../Symbolic/Structures.h"
#include "../Interpretation/Types.h"
#include "../Interpretation/Word.h"
#include "Word.h"

namespace sym {
    class StackGrapher{
        // cached results of already computed words
        std::unordered_map<mfc::Wordptr, Wordptr> visited_words;
        Data symbolize_data(mfc::Data data);
        Wordptr conversion_pass(mfc::ForthWord *wordptr);
        void graph_pass(Wordptr word);
        void branching_pass(Wordptr word);
        void retrieve_push_pop_effects(Wordptr word);
    public:
        Wordptr compute_effects_flattened(mfc::Wordptr input);
        Wordptr compute_effects(mfc::Wordptr original_word);
        Wordptr generate_ir(Wordptr wordptr);
    };
}

#endif
