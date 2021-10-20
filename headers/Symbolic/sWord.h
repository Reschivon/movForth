
#ifndef MOVFORTH_WORD_H
#define MOVFORTH_WORD_H

#include <utility>
#include "Structures.h"
#include "../PrimitiveEnums.h"
#include "Effects.h"
#include "Block.h"

namespace mov{

    class sWord{
    public:
        const std::string name;
        primitive_words id = primitive_words::OTHER;

        explicit sWord(std::string name, primitive_words id);
        explicit sWord(std::string name, primitive_words id, Effects effects);

        std::vector<Block> basic_blocks{};

        NodeList my_graphs_outputs{};
        NodeList my_graphs_inputs{};

        Effects effects;

        bool branchy();
    };

}

#endif //MOVFORTH_WORD_H
