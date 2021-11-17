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

namespace mov{
    class Stack;
    class iWord;
    class iData;
    class Interpreter;

    typedef iWord *iWordptr;

    /**
     * Iterator over a word's definition
     */
    struct IP : public std::list<iData>::iterator{
        iWordptr parent;
        explicit IP(iWordptr parent, std::list<iData>::iterator);
        explicit IP();
    };

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

        virtual void execute(IP &ip, Interpreter &interpreter) = 0;

        std::string name();

        bool branchy();

    };

    /**
     * Defined word, holds a definition
     */
    class ForthWord : public iWord {
        std::list<iData> definition;

    public:

        ForthWord(std::string name, bool immediate);
        void execute(IP &ip, Interpreter &interpreter) override;

        std::set<std::string> locals;

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
        std::list<iData>& def() {
            return definition;
        }
    };

    /**
     * Primitive word
     * Get singletons from primitive_lookup
     */
    class Primitive : public iWord{
        std::function<void(IP&, Interpreter&)> action;

        friend class iWordGenerator;

    public:
        Primitive(std::string name, primitive_words id, bool immediate, std::function<void(IP&, Interpreter&)> action, bool stateful);

        void execute(IP &ip, Interpreter &interpreter) override;
    };


    class ToLocal : public Primitive {
    public:
        std::string localname;
        ToLocal(std::string local_name, std::set<std::string> &locals);
    };

    class FromLocal : public Primitive {
    public:
        std::string localname;
        explicit FromLocal(std::string local_name);
    };

}



#endif
