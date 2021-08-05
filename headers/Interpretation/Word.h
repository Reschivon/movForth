#ifndef MOVFORTH_INTER_WORD_H
#define MOVFORTH_INTER_WORD_H

#include <unordered_map>
#include <functional>
#include <variant>
#include <iostream>
#include <string>
#include "Types.h"

namespace mfc{
    class Stack;

    template <typename Cast>
    static Cast* try_cast(Wordptr word_pointer){
        return dynamic_cast<Cast*>(word_pointer);
    }

    // classes
    class Word{
        std::string name;
    public:
        bool immediate;
        const bool stateful;
        Data data = Data(nullptr); // has value if stateful

        virtual void execute(Stack &stack, IP &ip) = 0;
        virtual Wordptr clone() = 0;

        Word(std::string name, bool immediate, bool stateful);
        virtual std::string to_string();
        virtual std::string base_string();
    };

    class ForthWord : public Word {
        std::vector<Data> definition;
    public:
        ForthWord(std::string name, bool immediate);

        void execute(Stack &stack, IP &ip) override;
        Wordptr clone() override;
        void add(Data data);
        void set(int index, Data value);
        void definition_to_string();
        int definition_size();
        std::vector<Data> get_definition() {
            return definition;
        }
    };
    class Primitive : public Word{
        std::function<void(Stack&, Data data, IP&)> action;
    public:
        Primitive(std::string name, bool immediate, std::function<void(Stack&, Data data, IP&)> action, bool stateful);
        void execute(Stack &stack, IP &ip) override;
        Wordptr clone() override;
    };
}

#endif
