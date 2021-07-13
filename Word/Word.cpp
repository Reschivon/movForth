
#include <iostream>
#include <utility>
#include "Word.h"
#include "../Interpret/Interpreter.h"

#define say std::cout <<
#define over << std::endl;

using namespace mfc;

Word::Word(std::string name, bool immediate)
        : name(std::move(name)), immediate(immediate) {}


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

Literal::Literal(Stack *stack)
    : StatefulPrimitive("literal"), stack(stack) {}
int Literal::execute() const {
    stack->push(data);
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
        i += entry->execute();
    }
    return 1;
}
void ForthWord::append_data(int data) {
    auto stateful_primitive = dynamic_cast<StatefulPrimitive*>(definition.back());
    if(stateful_primitive == nullptr)
        std::cout << "attempted to set data value of " <<
        definition.back()->name <<
        ", which is not a stateful word" << std::endl;
    else
        stateful_primitive->data = data;
}

void ForthWord::append_xt(Word_ptr word_ptr) {
    definition.push_back(word_ptr);
}
void ForthWord::print_def() {
    for(auto w : definition)
        say " " + w->name;
}

Branch::Branch(int relative_jmp)
    : Word("branch", false), relative_jmp(relative_jmp) {}

int Branch::execute() const {
    return relative_jmp + 1;
}
