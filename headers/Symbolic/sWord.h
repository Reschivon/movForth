
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

        RegisterGen param_gen = RegisterGen(0);

        std::unordered_map<Local, Register, LocalHasher> locals;

        std::vector<Block> blocks{};

        NodeList my_graphs_returns{};

        Effects effects;

        bool branchy();
    };

}

namespace std
{

}

#endif //MOVFORTH_WORD_H
