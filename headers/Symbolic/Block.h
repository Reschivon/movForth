

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

        sWordptr parent;

        unsigned int index = 0; // like Register but I was lazy

        std::vector<Instruction*> instructions{}; //TODO reference wrapper

        // Nodes of this basic block before the first instruction
        NodeList inputs{};
        // Nodes of this basic block after the last instruction
        NodeList outputs{};
        // Parameter nodes used in ONLY this block;
        // added by propagate stack
        NodeList params{};

        // Do not store push/pop effects for Block
        // in graph generation, the stack may come in
        // with more than 0 elements, and this makes
        // calculating push/pop complicated
        // We never need this statistic anyways
        Effects effects_without_push_pop;

        explicit Block(BBgen& gen, sWordptr parent);

        std::vector<bb_ref> nextBBs();
        bool is_exit();
        std::string name();

        // cyclic pass use only (*sigh* there was no other way)
        bool inputs_aligned = false;
        bool outputs_aligned = false;

        RegisterGen register_gen; // to track registers for this BB
        std::vector<Register> initial_registers; // temporary store

        uint initial_accumulated_stack_size = 0;
        uint initial_accumulated_params = 0;

        void align_registers();

    };
}
#endif // MOVFORTH_BLOCK_H
