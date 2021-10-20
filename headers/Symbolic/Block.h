

#ifndef MOVFORTH_BLOCK_H
#define MOVFORTH_BLOCK_H

#include "Instruction.h"
#include "Effects.h"

namespace mov{
    struct BBgen{
        unsigned int get(){
            return id++;
        }
    private:
        unsigned int id = 1;
    };

    struct Instruction;

    struct Block{
        typedef std::reference_wrapper<Block> bb_ref;

        unsigned int index = 0; // like Register but I was lazy

        std::vector<Instruction*> instructions{}; //TODO reference wrapper

        // Nodes of this basic block after the last instruction
        NodeList outputs{};
        // Parameter nodes; added to by propagate stack
        NodeList params{};

        Effects effects_without_push_pop;

        explicit Block(BBgen& gen);

        std::vector<bb_ref> nextBBs();
        bool is_exit();
        std::string name();

        // cyclic pass use only (*sigh* there was no other way)
        bool visited = false; // whether the graph has been generated yet
        RegisterGen register_gen; // to track registers for this BB
        std::vector<Register> initial_registers; // temporary store
        int initial_stack_size = 0;
        int initial_accumulated_params = 0;

        static void match_registers_of_unvisited(Block &prev, Block &post);
    };
}
#endif // MOVFORTH_BLOCK_H
