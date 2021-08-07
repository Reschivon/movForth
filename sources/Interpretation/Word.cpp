#include <utility>
#include "../../headers/Interpretation/Word.h"
#include "../../headers/Interpretation/Stack.h"

using namespace mfc;

Word::Word(std::string name, bool immediate, bool stateful)
        : name(std::move(name)), immediate(immediate), stateful(stateful) {}

std::string Word::to_string() {
    if(stateful)
        return name + "(" + data.to_string() + ")";
    else
        return name;
}

std::string Word::base_string() {
    return name;
}


ForthWord::ForthWord(std::string name, bool immediate) : Word(std::move(name), immediate, false){}
void ForthWord::execute(Stack &stack, IP &ip) {
    for(IP it(definition.begin()); it < definition.end(); it+=1) {

        //println("       [exec] ", ((*(it.me)).is_xt()?(*(it.me)).as_xt()->to_string():std::to_string((*(it.me)).as_num())), " ");

        (*it.me)->execute(stack, it);

    }
}
void ForthWord::add(Data data){
    if(!definition.empty() && // there exists a word
        definition.back()->stateful && // this word uses the data field
        definition.back()->data.type() == Data::empty){ // we didn't already set the data field
        definition.back()->data = data;
    }else if(data.type() == Data::word){
        definition.push_back(data.to_type<Data::word_t>());
    }else{
        println("Adding a number to a definition is forbidden");
    }
}

void ForthWord::definition_to_string() {
    for(Wordptr thing : definition)
        print(thing->to_string(), " ");
}

int ForthWord::definition_size() {
    return definition.size();
}

void ForthWord::set(int index, Data value) {
        definition[index]->data = value;
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