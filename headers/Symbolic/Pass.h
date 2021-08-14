

#ifndef MOVFORTH_mov_symbolicPASS_H
#define MOVFORTH_mov_symbolicPASS_H

#include "../Symbolic/Structures.h"
#include "../Interpretation/iWord.h"
#include "sWord.h"

namespace mov {
    class Analysis{
        // cached results of already computed words
        std::unordered_map<iWordptr, sWordptr> visited_words{};

        sData symbolize_data(iData data);
        sWordptr translate_to_basic_blocks(ForthWord *template_word);
        static void word_stack_graph(sWordptr wordptr);
    public:
        static void compute_matching_pairs(BasicBlock &bb);
        static NodeList basic_block_stack_graph(NodeList &running_stack, BasicBlock &bb, RegisterGen register_gen);

        sWordptr static_analysis(iWordptr original_word);
        static sWordptr show_word_info(sWordptr wordptr);
    };
}

#endif
