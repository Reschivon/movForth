
#ifndef MOVFORTH_WORD_H
#define MOVFORTH_WORD_H

#include <utility>
#include "Structures.h"
#include "../PrimitiveEnums.h"
#include "Effects.h"
#include "Instruction.h"

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
        explicit BasicBlock(BBgen& gen) : index(gen.get()) {}
        std::vector<Instruction*> instructions;//TODO reference wrapper
        unsigned int index = 0; // like Register but I was lazy
    };

    class sWord {
    public:
        primitive_words id = primitive_words::OTHER;
        const std::string name;
        Effects effects;

        explicit sWord(std::string name, primitive_words id);

        explicit sWord(std::string name, primitive_words id, Effects effects);

        NodeList my_graphs_outputs;
        NodeList my_graphs_inputs;

        void definition_to_string();

        std::vector<BasicBlock> basic_blocks;

        std::vector<Instruction*> instructions;

        bool branchy(){
            return
            id == primitive_words::BRANCH ||
            id == primitive_words::BRANCHIF ||
            id == primitive_words::EXIT;
        }
    };

}

#endif //MOVFORTH_WORD_H
