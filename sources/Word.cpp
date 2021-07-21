#include "../headers/Word.h"
#include "../headers/Stack.h"
#include "../headers/Interpreter.h"
using namespace mfc;

Word::Word(std::string name) : Word(name, false) {}
Word::Word(std::string name, bool immediate) : name(name), immediate(immediate) {}

std::string Word::to_string() {
    return name;
}

std::string Word::base_string() {
    return name;
}


ForthWord::ForthWord(std::string name, bool immediate) : Word(name, immediate){}
void ForthWord::execute(Stack &stack, IP &ip) {
    for(IP it(definition.begin()); it < definition.end(); it+=1) {
        std::cout << "       [exec] " << ((*(it.me)).is_xt()?(*(it.me)).as_xt()->to_string():std::to_string((*(it.me)).as_num())) << " " << std::endl;
        if(it.me->is_xt())
            it.me->as_xt()->execute(stack, it);
        else
            std::cout << "Number was not consumed by a word before it, and was erroneously executed" << std::endl;
    }
}
void ForthWord::append(Data data){
    definition.push_back(data);
}

void ForthWord::print_definition() {
    for(Data thing : definition) {
        std::string h;
        if(thing.is_num())
            h = std::to_string(thing.as_num());
        if(thing.is_xt())
            h = thing.as_xt()->to_string();
        std::cout << h << " ";
    }
}

int ForthWord::definition_size() {
    return definition.size();
}


LambdaPrimitive::LambdaPrimitive(std::string name, bool immediate, std::function<void(Stack&, IP&)> action)
    : Word(name, immediate), action(action) {}

void LambdaPrimitive::execute(Stack &stack, IP &ip) {
    action(stack, ip);
}


StatefulPrimitive::StatefulPrimitive(std::string name, bool immediate) : Word(name, immediate) {}


Branch::Branch() : StatefulPrimitive("branch", false){}
void Branch::execute(Stack &stack, IP &ip) {
    ip += 1;
    Data data = *(ip.me);
    try {
        ip += data.as_num() + 1;
    }catch (...){
        std::cout << "fuck" << std::endl;
    }
}

BranchIf::BranchIf() : StatefulPrimitive("branchif", false){}

void BranchIf::execute(Stack &stack, IP &ip) {
    ip += 1;
    Data data = *(ip.me);

    if(stack.pop_number() > 0){
        ip += 1;
    }else {
        if (data.is_num())
            ip += data.as_num() + 1;
        else
            std::cout << "fuck" << std::endl;
    }
}

Literal::Literal() : StatefulPrimitive("literal", false){}

void Literal::execute(Stack &stack, IP &ip) {
    ip += 1;
    Data data = *(ip.me);

    stack.push(Data(data));
}

