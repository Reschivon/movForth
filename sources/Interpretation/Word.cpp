#include <utility>
#include "../../headers/Interpretation/Word.h"
#include "../../headers/Interpretation/Stack.h"

using namespace mfc;

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

        //println("       [exec] ", ((*(it.me)).is_xt()?(*(it.me)).as_xt()->to_string():std::to_string((*(it.me)).as_num())), " ");

        if(it.me->is_xt())
            it.me->as_xt()->execute(stack, it);
        else
           println("Number ", it.me->as_num(), " was not consumed by a word before it, and was erroneously executed");
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
        print(h, " ");
    }
}

int ForthWord::definition_size() {
    return definition.size();
}

void ForthWord::set(int index, Data value) {
    definition[index] = value;
}


Primitive::Primitive(std::string name, bool immediate, std::function<void(Stack&, IP&)> action)
    : Word(std::move(name), immediate), action(std::move(action)) {}

void Primitive::execute(Stack &stack, IP &ip) {
    action(stack, ip);
}
