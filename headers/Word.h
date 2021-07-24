#ifndef MOVFORTH_WORD_H
#define MOVFORTH_WORD_H

#include <unordered_map>
#include <functional>
#include <variant>
#include <iostream>
#include "Types.h"

namespace mfc{
    class Stack;


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
        void add(Data data);
        void set(int index, Data value);
        void definition_to_string();

        int definition_size();
    };

    class LambdaPrimitive : public Word{
        std::function<void(Stack&, IP&)> action;
    public:
        LambdaPrimitive(std::string name, bool immediate, std::function<void(Stack&, IP&)> action);
        void execute(Stack &stack, IP &ip) override;
    };

    class Branch : public Word {
    public:
        Branch();
        void execute(Stack &stack, IP &ip) override;
    };

    class BranchIf : public Word {
    public:
        BranchIf();
        void execute(Stack &stack, IP &ip) override;
    };

    class Literal : public Word {
        void execute(Stack &stack, IP &ip) override;
    public:
        Literal();
    };

    static inline std::string data_to_string(Data &data){
        if(data.is_xt())
            return data.as_xt()->to_string();
        if(data.is_num())
            return std::to_string(data.as_num());
        return "shit";
    }
}

#endif //MOVFORTH_WORD_H
