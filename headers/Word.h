#ifndef MOVFORTH_WORD_H
#define MOVFORTH_WORD_H

#include <unordered_map>
#include <functional>
#include <variant>
#include <iostream>
#include "Types.h"

namespace mfc{
    class Stack;
    class IP{
        bool isActive = false;
    public:
        std::vector<Data>::iterator me;

        IP(std::vector<Data>::iterator in) : me(in) {isActive = true;}
        IP() : me(nullptr) {}
        IP operator+=(int i) { //outside the class
            if(isActive) {
                return me += i;
            }
            return me;
        }
        bool operator<(const std::vector<Data>::iterator& a){
            return me < a;
        }
    };

    // static
    static std::unordered_map<std::string, std::function<Word*()>> generators;

    // TODO does dyno cast work like <Cast> (no pointer)
    template <typename Cast>
    static Cast* instance_of(Wordptr word_pointer){
        return dynamic_cast<Cast*>(word_pointer);
    }

    // classes
    class Word{
        std::string name;
    public:
        bool immediate;

       virtual void execute(Stack &stack, IP &ip) = 0;

        Word(std::string name);
        Word(std::string name, bool immediate);
        virtual std::string to_string();
        virtual std::string base_string();
    };

    class ForthWord : public Word{
        std::vector<Data> definition;
    public:
        ForthWord(std::string name, bool immediate);
        void execute(Stack &stack, IP &ip) override;
        void append(Data data);
        void print_definition();
        int definition_size();
    };

    class LambdaPrimitive : public Word{
        std::string name;
        std::function<void(Stack&, IP&)> action;
    public:
        LambdaPrimitive(std::string name, bool immediate, std::function<void(Stack&, IP&)> action);
        void execute(Stack &stack, IP &ip) override;
    };

    class StatefulPrimitive : public Word{
    protected:
        bool already_set = false;
    public:
        StatefulPrimitive(std::string name, bool immediate);
        virtual void execute(Stack &stack, IP &ip) = 0;
    };

    class Branch : public StatefulPrimitive {
    public:
        Branch();
        void execute(Stack &stack, IP &ip) override;
    };

    class BranchIf : public StatefulPrimitive {
    public:
        BranchIf();
        void execute(Stack &stack, IP &ip) override;
    };

    class Literal : public StatefulPrimitive {
        void execute(Stack &stack, IP &ip) override;
    public:
        Literal();
    };

    static inline std::string data_to_string(Data &data){
        if(data.is_xt())
            data.as_xt()->to_string();
        if(data.is_num())
            return std::to_string(data.as_num());
        return "shit";
    }
}



#endif //MOVFORTH_WORD_H
