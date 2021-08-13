

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
        unsigned int id = 1;
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
        RegisterGen register_gen;
        std::vector<Register> enter_registers;
        int enter_stack_size = 0;
        int enter_inputs = 0;

        static void match_registers_of_unvisited(BasicBlock &prev, BasicBlock &post);
    };
}
#endif // MOVFORTH_BASICBLOCK_H
