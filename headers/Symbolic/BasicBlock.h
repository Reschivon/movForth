

#ifndef MOVFORTH_BASICBLOCK_H
#define MOVFORTH_BASICBLOCK_H

#include "Instruction.h"
#include "Effects.h"

namespace mov{
    struct BBgen{
        unsigned int get(){
            return id++;
        }
    private:
        unsigned int id = 0;
    };

    struct Instruction;

    struct BasicBlock{
        typedef std::reference_wrapper<BasicBlock> bb_ref;

        unsigned int index = 0; // like Register but I was lazy

        std::vector<Instruction*> instructions{}; //TODO reference wrapper

        NodeList my_graphs_outputs{};
        NodeList my_graphs_inputs{};

        Effects effects;

        explicit BasicBlock(BBgen& gen);

        std::vector<bb_ref> nextBBs();
        bool is_exit();

        // cyclic pass use only (*sigh* there was no other way)
        bool visited = false;
        int enter_stack_size;
    };
}
#endif // MOVFORTH_BASICBLOCK_H
