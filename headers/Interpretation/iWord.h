#ifndef MOVFORTH_INTER_iWord_H
#define MOVFORTH_INTER_iWord_H

#include <unordered_map>
#include <functional>
#include <variant>
#include <iostream>
#include <string>
#include "Data.h"

namespace mfc{
    class Stack;
    class iWord;
    typedef iWord *iWordptr;

    class IP{
        const bool isActive;
    public:
        std::vector<iWordptr>::iterator me;

        explicit IP(std::vector<iWordptr>::iterator in) : me(in), isActive(true) {}
        IP() : isActive(false) {}

        IP operator+=(int i) { //outside the class
            if(!isActive) return *this;
            me += i;
            return *this;
        }

        bool operator<(const std::vector<iWordptr>::iterator& a) const{
            return me < a;
        }
    };


    class iWord{
        const std::string name;
    public:
        bool immediate;
        const bool stateful;
        Data data = Data(); // has value if stateful

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
        std::vector<iWordptr> definition;
    public:
        ForthWord(std::string name, bool immediate);

        void execute(Stack &stack, IP &ip) override;
        iWordptr clone() override;
        void add(Data data);
        void set(int index, Data value);
        void definition_to_string();
        int definition_size();
        std::vector<iWordptr> get_definition() {
            return definition;
        }
    };

    class Primitive : public iWord{
        std::function<void(Stack&, Data data, IP&)> action;
    public:
        Primitive(std::string name, bool immediate, std::function<void(Stack&, Data data, IP&)> action, bool stateful);
        void execute(Stack &stack, IP &ip) override;
        iWordptr clone() override;
    };
}

#endif
