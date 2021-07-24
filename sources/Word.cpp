#include <utility>
#include "../headers/Word.h"
#include "../headers/Stack.h"

using namespace mfc;

Word::Word(std::string name) : Word(std::move(name), false) {}
Word::Word(std::string name, bool immediate) : name(std::move(name)), immediate(immediate) {}

std::string Word::to_string() {
    return name;
}

std::string Word::base_string() {
    return name;
}

ForthWord::ForthWord(std::string name, bool immediate) : Word(std::move(name), immediate){}
void ForthWord::execute(Stack &stack, IP &ip) {
    for(IP it(definition.begin()); it < definition.end(); it+=1) {

        //std::cout << "       [exec] " << ((*(it.me)).is_xt()?(*(it.me)).as_xt()->to_string():std::to_string((*(it.me)).as_num())) << " " << std::endl;

        if(it.me->is_xt())
            it.me->as_xt()->execute(stack, it);
        else
            std::cout << "Number was not consumed by a word before it, and was erroneously executed" << std::endl;
    }
}
void ForthWord::add(Data data){
    definition.push_back(data);
}

void ForthWord::definition_to_string() {
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

void ForthWord::set(int index, Data value) {
    definition[index] = value;
}


LambdaPrimitive::LambdaPrimitive(std::string name, bool immediate, std::function<void(Stack&, IP&)> action)
    : Word(std::move(name), immediate), action(std::move(action)) {}

void LambdaPrimitive::execute(Stack &stack, IP &ip) {
    action(stack, ip);
}


Branch::Branch() : Word ("branch"){}

void Branch::execute(Stack &stack, IP &ip) {
    ip += 1;
    Data data = *(ip.me);
    try {
        ip += (data.as_num() - 2);
        // negative jumps go ++ more
        if (data.as_num() < 0) ip += 1;
    }catch (const std::exception& e){
        std::cout << "fucker: " << e.what() << std::endl;
    }
}

BranchIf::BranchIf() : Word("branchif"){}

void BranchIf::execute(Stack &stack, IP &ip) {
    ip += 1;
    Data data = *(ip.me);

    if(stack.pop_number() > 0) {
        std::cout << "BB jump" << std::endl;

        if (data.is_num()) {
            ip += data.as_num() + 1;
        } else
            std::cout << "fuck" << std::endl;
    } else {
        std::cout << "BB no jump" << std::endl;
        //ip += 1;
    }
}

Literal::Literal() : Word("literal"){}

void Literal::execute(Stack &stack, IP &ip) {
    ip += 1;
    Data data = *(ip.me);

    stack.push(Data(data));
}

