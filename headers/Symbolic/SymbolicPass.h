

#ifndef MOVFORTH_mov_symbolicPASS_H
#define MOVFORTH_mov_symbolicPASS_H

#include "../Symbolic/Structures.h"
#include "../Interpretation/iWord.h"
#include "sWord.h"

namespace mov {
    class StackGrapher{
        // cached results of already computed words
        std::unordered_map<iWordptr, sWordptr> visited_words{};
        sData symbolize_data(iData data);
        //sWordptr conversion_pass(ForthWord *wordptr);
        sWordptr translate_to_basic_blocks(ForthWord *template_word);
        static void bb_cyclic_pass(sWordptr wordptr);
        static void retrieve_push_pop_effects(sWordptr word);
    public:
        static NodeList stack_graph_pass_bb(NodeList &running_stack, BasicBlock &bb, RegisterGen register_gen);
        sWordptr static_analysis(iWordptr original_word);
        static sWordptr show_word_info(sWordptr wordptr);
    };
}

#endif
