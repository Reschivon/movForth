#include <utility>
#include "../../headers/Interpretation/Word.h"
#include "../../headers/Interpretation/Stack.h"

using namespace mfc;

Word::Word(std::string name, bool immediate, bool stateful) : name(std::move(name)), immediate(immediate), stateful(stateful) {}

std::string Word::to_string() {
    if(stateful)
        return name + "(" + data.to_string() + ")";
    else
        return name;
}

std::string Word::base_string() {
    return name;
}

std::string Data::to_string(){
    if(is_xt())
        return as_xt()->to_string();
    if(is_num())
        return std::to_string(as_num());
    return "undef";
}

Data Data::clone(){
    if(is_xt())
        return Data(as_xt()->clone());
    if(is_num())
        return Data(as_num());
    return Data(nullptr);
}

ForthWord::ForthWord(std::string name, bool immediate) : Word(std::move(name), immediate, false){}
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
    if(!definition.empty() && // there exists something
        definition.back().is_xt() && // this something is a word (with Data field)
        definition.back().as_xt()->stateful && // this word uses the data field
        definition.back().as_xt()->data.is_undef()){ // we didn't already set the data field
        definition.back().as_xt()->data = data;
    }else{
        definition.push_back(data);
    }
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
    if(definition[index].is_xt())
        definition[index].as_xt()->data = value;
}

Wordptr ForthWord::clone() {
    return new ForthWord(*this);
}


Primitive::Primitive(std::string name, bool immediate, std::function<void(Stack&, Data data, IP&)> action, bool stateful)
: Word(std::move(name), immediate, stateful), action(std::move(action)) {}

void Primitive::execute(Stack &stack, IP &ip) {
    action(stack, data, ip);
}

Wordptr Primitive::clone() {
    return new Primitive(*this);
}