#ifndef MOVFORTH_INTER_iWord_H
#define MOVFORTH_INTER_iWord_H

#include <unordered_map>
#include <functional>
#include <variant>
#include <iostream>
#include <string>
#include "iData.h"
#include "../PrimitiveEffects.h"

namespace mov{
    class Stack;
    class iWord;
    typedef iWord *iWordptr;

    /**
     * Iterator over a word's definition
     */
    typedef std::vector<iData>::iterator IP;

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

        virtual void execute(Stack &stack, IP &ip) = 0;

        std::string name();
    };

    /**
     * Cast a iWordptr to one of its derived types
     * @tparam Cast desired type, *non-pointer*
     * @param word_pointer
     * @return word_pointer casted, or nullptr if unsuccessful
     */
    template <typename Cast>
    static Cast* try_cast(iWordptr word_pointer){
        return dynamic_cast<Cast*>(word_pointer);
    }

    /**
     * Defined word, holds a definition
     */
    class ForthWord : public iWord {
        std::vector<iData> definition;

    public:
        ForthWord(std::string name, bool immediate);

        void execute(Stack &stack, IP &ip) override;

        /**
         * Append definition
         */
        void add(iData data);
        /**
         * Set index of definition to value
         * @throws std::__out_of_range if invalid index
         */
        void set(int index, iData value);
        /**
         * For debuggin only. Does what you think
         */
        void definition_to_string();
        /**
         * @return reference to definition
         */
        std::vector<iData>& def() {
            return definition;
        }
    };

    /**
     * Primitive word
     * Get singletons from primitive_lookup
     */
    class Primitive : public iWord{
        std::function<void(Stack&, IP&)> action;
        Primitive(std::string name, primitive_words id, bool immediate, std::function<void(Stack&, IP&)> action, bool stateful);
        friend class iWordGenerator;
    public:
        void execute(Stack &stack, IP &ip) override;
    };
}

#endif
