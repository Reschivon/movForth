

#ifndef MOVFORTH_mov_symbolicPASS_H
#define MOVFORTH_mov_symbolicPASS_H

#include "../Symbolic/Structures.h"
#include "../Interpretation/iWord.h"
#include "sWord.h"

namespace mov {
    class Analysis{
        /**
         * cached results of already analyzed words
         */
        std::unordered_map<iWordptr, sWordptr> visited_words{};

        /** converts an iData (interpretation data object)
         * into an sData (analysis data object). Will analyze word types
         * stored in the data if the word exists
         * @param data iData to be converted
         * @return sData
         */
        sData symbolize_data(iData data);

        /**
         * Takes a ForthWord object from the interpreter and converts
         * it to a mostly empty (graphless) sWord that has basic blocks
         * premade according to the ForthWord's BRANCH and BRANCHIF
         * @param template_word ForthWord from the interpreter for reference
         * @return new sWordptr with basic blocks precomputed (and nothing else)
         */
        sWordptr translate_to_basic_blocks(ForthWord *template_word);

        /**
         * Creates stack graph for the provided word
         * Prerequisite: word must have basic blocks computed
         * @param wordptr word that will have its stack graph computed
         */
        void word_stack_graph(sWordptr wordptr);

        /**
         * Takes a root BB and creates all basic block stack graphs that
         * are linked to the root BB in the control flow graph
         * Also links stack graphs between BBs
         * Called exclusively by word_stack_graph
         * @param stack inital stack state
         * @param bb
         */
        void explore_graph_dfs(NodeList stack, Block &bb);

        /**
         * Generates stack graph (and thus register edges) for all words in the
         * given basic block
         * @param running_stack initial stack state
         * @param bb basic block that will have a compted stack graph
         * @param register_gen register generator for new register edges
        */
        static NodeList basic_block_stack_graph(NodeList &running_stack, Block &bb, RegisterGen register_gen);

        /**
         * Builds register graph for the space between the given stack
         * and the given word, and the space between the given word
         * and a new stack
         * @param stack populated stack that serves as params stack elements
         * @param instruction instruction that represents the word to be simulated
         * @param base parent basic block for holding params nodes
         * @param register_gen register generator for new register edges
         */
        static void propagate_stack(NodeList &stack, Instruction *instruction, NodeList &params, RegisterGen &register_gen);

        void compute_matching_pairs(Block &bb);
    public:
        sWordptr static_analysis(iWordptr original_word);
        static sWordptr show_word_info(sWordptr wordptr);
    };
}

#endif
