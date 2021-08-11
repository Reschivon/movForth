#include "../../headers/Interpretation/iWord.h"
#include "../../headers/Interpretation/iData.h"

using namespace mov;

iWord::iWord(std::string name, bool immediate, bool stateful)
        : _name(std::move(name)), immediate(immediate), stateful(stateful) {}

iWord::iWord(std::string name, primitive_words id, bool immediate, bool stateful)
    : _name(std::move(name)), id(id), immediate(immediate), stateful(stateful) {}

std::string iWord::name() {
    return _name;
}


ForthWord::ForthWord(std::string name, bool immediate)
    : iWord(std::move(name), immediate, false) {}

void ForthWord::execute(IP &ip) {
    for(IP it = definition.begin(); it < definition.end(); it++) {

        //println("       [exec] ", (it->is_word()?it->as_word()->to_string():std::_name(it->as_number())), " ");

        iData current_cell = *it;
        if(current_cell.is_word())
            current_cell.as_word()->execute(it);
        else
            println("Too many numbers in definition, no LITERAL to consume them");
    }
}
void ForthWord::add(iData data){
    definition.push_back(data);
}

void ForthWord::definition_to_string() {
    for(auto thing : definition)
        print(thing.to_string(), " ");
}

void ForthWord::set(int index, iData value) {
    definition.at(index) = value;
}

Primitive::Primitive(std::string name, primitive_words id, bool immediate, std::function<void(IP&)> action, bool stateful)
    : iWord(std::move(name), id, immediate, stateful), action(std::move(action)) {}

void Primitive::execute(IP &ip) {
    action(ip);
}

