

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
         * Make a stack graph for an entire definition via multiple calls to propagate_stack().
         * Also compute and set all non-stack effects for bb
         *
         * @param running_stack inital state of the stack
         * @param bb the basic block whose instructions with be graphed
         * @param register_gen to generate the names of new registers
         * @return the running stack
         */
        static NodeList basic_block_stack_graph(NodeList &running_stack, Block &bb, RegisterGen register_gen);

        /**
         * Appends new register edges to the existing register graph, based on
         * the push and pop effects of the provided instruction. Also updates
         * stack parameter to the state of stack after this instruction
         *
         * @param stack current state of stack
         * @param instruction dictates the number of graph edges to be added
         * @param params if additional parameter nodes are required, they will be added here
         * @param register_gen to generate the names of new registers
         */
        static void propagate_stack(NodeList &stack, Instruction *instruction, NodeList &params, RegisterGen &register_gen);

        /**
         * If there are stack elements that remain unchanged (but could be shuffled)
         * between the start and end of this BB, then note the unchanged register pairs
         * in bb.effects.matching pairs
         *
         * @param bb the basic block in which to find matching pairs
         */
        void compute_matching_pairs(Block &bb);


    public:
        sWordptr static_analysis(iWordptr original_word);
        static sWordptr show_word_info(sWordptr wordptr);
    };
}

#endif
