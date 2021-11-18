#ifndef MOVFORTH_INTER_iWord_H
#define MOVFORTH_INTER_iWord_H

#include <unordered_map>
#include <functional>
#include <variant>
#include <iostream>
#include <string>
#include <list>
#include <set>

#include "../PrimitiveEffects.h"
#include "Local.h"
#include "IP.h"

namespace mov{

    class iWord;
    class Interpreter;
    typedef iWord *iWordptr;

    /**
     * A word in the interpreter's dictionary
     */
    class iWord{

        const std::string _name;

    public:
        /**
         * Identifier for primitive words
         * Use this to compare *word types*, ie. all branch instances have equal ids
         * Defined words fall under a general id primitive_words::OTHER
         */
        primitive_words id = primitive_words::OTHER;

        bool immediate;

        /**
         * Whether the word relies on the Data in the next
         * definition entry during execution
         */
        const bool stateful;

        iWord(std::string name, bool immediate, bool stateful);
        iWord(std::string name, primitive_words id, bool immediate, bool stateful);

        virtual void execute(IP ip, Interpreter &interpreter) = 0;

        virtual std::string name();

        bool branchy();

    };


}



#endif
