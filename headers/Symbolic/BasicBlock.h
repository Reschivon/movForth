

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

    struct BasicBlock {
        unsigned int index = 0; // like Register but I was lazy

        std::vector<Instruction*> instructions;//TODO reference wrapper

        NodeList my_graphs_outputs;
        NodeList my_graphs_inputs;

        Effects effects;

        explicit BasicBlock(BBgen& gen);

        std::vector<BasicBlock*> nextBBs();
        bool is_exit();

        // cyclic pass use only (*sigh* there was no other way)
        bool visited = false;
    };
}
#endif // MOVFORTH_BASICBLOCK_H
