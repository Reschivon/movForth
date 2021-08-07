#include <utility>
#include "../../headers/Interpretation/iWord.h"
#include "../../headers/Interpretation/Stack.h"

using namespace mfc;

iWord::iWord(std::string name, bool immediate, bool stateful)
        : name(std::move(name)), immediate(immediate), stateful(stateful) {}

std::string iWord::to_string() {
    if(stateful)
        return name + "(" + data.to_string() + ")";
    else
        return name;
}

std::string iWord::base_string() {
    return name;
}


ForthWord::ForthWord(std::string name, bool immediate) : iWord(std::move(name), immediate, false){}
void ForthWord::execute(Stack &stack, IP &ip) {
    for(IP it(definition.begin()); it < definition.end(); it+=1) {

        //println("       [exec] ", ((*(it.me)).is_xt()?(*(it.me)).as_xt()->to_string():std::to_string((*(it.me)).as_num())), " ");

        (*it.me)->execute(stack, it);

    }
}
void ForthWord::add(iData data){
    if(!definition.empty() && // there exists a word
        definition.back()->stateful && // this word uses the data field
        definition.back()->data.type() == iData::empty){ // we didn't already set the data field
        definition.back()->data = data;
    }else if(data.type() == iData::iword){
        definition.push_back(data.to_type<iData::iword_t>());
    }else{
        println("Adding a number to a definition is forbidden");
    }
}

void ForthWord::definition_to_string() {
    for(iWordptr thing : definition)
        print(thing->to_string(), " ");
}

int ForthWord::definition_size() {
    return definition.size();
}

void ForthWord::set(int index, iData value) {
        definition[index]->data = value;
}

iWordptr ForthWord::clone() {
    return new ForthWord(*this);
}


Primitive::Primitive(std::string name, bool immediate, std::function<void(Stack&, iData data, IP&)> action, bool stateful)
: iWord(std::move(name), immediate, stateful), action(std::move(action)) {}

void Primitive::execute(Stack &stack, IP &ip) {
    action(stack, data, ip);
}

iWordptr Primitive::clone() {
    return new Primitive(*this);
}