
#include <iostream>
#include <utility>
#include "Word.h"
#include "../Interpret/Interpreter.h"

#define say std::cout <<
#define over << std::endl;

using namespace mfc;

Word::Word(std::string name, bool immediate)
        : name_str(std::move(name)), immediate(immediate) {}
std::string Word::name() {
    return name_str;
}

LambdaPrimitive::LambdaPrimitive(std::string name, std::function<int()> action)
        : Word(std::move(name), false), action(std::move(action)) {}
LambdaPrimitive::LambdaPrimitive(std::string name, bool immediate, std::function<int()> action)
        : Word(std::move(name), immediate), action(std::move(action)) {
}
int LambdaPrimitive::execute() const {
    return action();
}

StatefulPrimitive::StatefulPrimitive(std::string name)
        : Word(std::move(name), false) {}
std::string StatefulPrimitive::name() {
    return name_str + "(" + std::to_string(data) + ")";
}


Literal::Literal(std::function<void(int)> add_to_stack)
    : StatefulPrimitive("literal"), add_to_stack(std::move(add_to_stack)) {}
int Literal::execute() const {
    add_to_stack(data);
    return 1;
}

Branch::Branch()
        : StatefulPrimitive("branch") {}
int Branch::execute() const {
    return data;
}

ForthWord::ForthWord(std::string name, bool immediate)
        : Word(std::move(name), immediate) {}
int ForthWord::execute() const {
    for(int i=0; i<definition.size();){
        Word_ptr entry = definition[i];
        //say "[exec] " << entry->name_str over
        i += entry->execute();
    }
    return 1;
}
void ForthWord::append_data(int data) {
    auto stateful_primitive = dynamic_cast<StatefulPrimitive*>(definition.back().get());
    if(stateful_primitive == nullptr)
        std::cout << "attempted to set data value of " <<
                  definition.back()->name() <<
                  ", which is not a stateful word" << std::endl;
    else
        stateful_primitive->data = data;
}

void ForthWord::append_xt(Word_ptr word_ptr) {
    definition.push_back(std::move(word_ptr));
}
void ForthWord::print_def() {
    for(auto w : definition)
        say " " + w->name();
}