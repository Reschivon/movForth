#ifndef MOVFORTH_INTER_iWord_H
#define MOVFORTH_INTER_iWord_H

#include <unordered_map>
#include <functional>
#include <variant>
#include <iostream>
#include <string>
#include "iData.h"

namespace mov{
    class Stack;
    class iWord;
    typedef iWord *iWordptr;

    typedef std::vector<iData>::iterator IP;

    class iWord{
        const std::string name;
    public:
        bool immediate;
        const bool stateful;
        iData data = iData(); // has value if stateful

        iWord(std::string name, bool immediate, bool stateful);

        virtual void execute(Stack &stack, IP &ip) = 0;
        virtual iWordptr clone() = 0;

        virtual std::string to_string();
        virtual std::string base_string();
    };

    template <typename Cast>
    static Cast* try_cast(iWordptr word_pointer){
        return dynamic_cast<Cast*>(word_pointer);
    }

    class ForthWord : public iWord {
        std::vector<iData> definition;
    public:
        ForthWord(std::string name, bool immediate);

        void execute(Stack &stack, IP &ip) override;
        iWordptr clone() override;
        void add(iData data);
        void set(int index, iData value);
        void definition_to_string();
        int definition_size();
        std::vector<iData> get_definition() {
            return definition;
        }
    };

    class Primitive : public iWord{
        std::function<void(Stack&, iData data, IP&)> action;
    public:
        Primitive(std::string name, bool immediate, std::function<void(Stack&, iData data, IP&)> action, bool stateful);
        void execute(Stack &stack, IP &ip) override;
        iWordptr clone() override;
    };
}

#endif
