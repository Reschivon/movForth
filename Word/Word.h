
#ifndef MOVFORTH_WORD_H
#define MOVFORTH_WORD_H

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace mfc {

    class Word;
    typedef std::shared_ptr<Word> Word_ptr;

    class Word {
    protected:
        const std::string name_str;
    public:

        bool immediate;

        Word(std::string name, bool immediate);

        // returns a branch jump (1 is regular branchless instruction)
        virtual int execute() const = 0;

        virtual ~Word() = default;

        virtual std::string name();
    };


    class LambdaPrimitive : public Word {
    public:
        LambdaPrimitive(std::string name, std::function<int()> action);
        LambdaPrimitive(std::string name, bool immediate, std::function<int()> action);

        int execute() const override;
    protected:
        std::function<int()> action;
    };

    class StatefulPrimitive : public Word {
    public:
        explicit StatefulPrimitive(std::string name);
        int execute() const override = 0;
        std::string name() override;

        int data;
    private:
    };

    class Literal : public StatefulPrimitive {
    public:
        explicit Literal(std::function<void(int)> add_to_stack);
        int execute() const override;
    private:
        std::function<void(int)> add_to_stack;
    };

    class Branch : public StatefulPrimitive {
    public:
        Branch();
        int execute() const override;
    };

    class ForthWord : public Word {
    public:
        int execute() const override;
        ForthWord(std::string name, bool immediate);

        void append_xt(Word_ptr word_ptr);
        void append_data(int data);
        int definition_size(){
            return definition.size();
        }
        void print_def();
    private:
        std::vector<Word_ptr> definition;
    };


} // namespace

#endif
