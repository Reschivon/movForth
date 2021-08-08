#include <utility>
#include "../../headers/Interpretation/iWord.h"
#include "../../headers/Interpretation/Stack.h"

using namespace mov;

iWord::iWord(std::string name, bool immediate, bool stateful)
        : name(std::move(name)), immediate(immediate), stateful(stateful) {}

std::string iWord::to_string() {
    return name;
}

std::string iWord::base_string() {
    return name;
}


ForthWord::ForthWord(std::string name, bool immediate) : iWord(std::move(name), immediate, false){}
void ForthWord::execute(Stack &stack, IP &ip) {
    for(IP it = definition.begin(); it < definition.end(); it++) {

        //println("       [exec] ", (it->is_word()?it->as_word()->to_string():std::to_string(it->as_number())), " ");

        iData current_cell = *it;
        if(current_cell.is_word())
            current_cell.as_word()->execute(stack, it);
        else
            println("Too many numbers in definition, no LITERAL to consume them");
    }
}
void ForthWord::add(iData data){
    definition.push_back(data);
}

void ForthWord::definition_to_string() {
    for(iData thing : definition)
        print(thing.to_string(), " ");
}

int ForthWord::definition_size() {
    return definition.size();
}

void ForthWord::set(int index, iData value) {
    definition[index] = value;
}

iWordptr ForthWord::clone() {
    return new ForthWord(*this);
}


Primitive::Primitive(std::string name, primitive_words id, bool immediate, std::function<void(Stack&, IP&)> action, bool stateful)
: iWord(std::move(name), immediate, stateful), action(std::move(action)) {}

void Primitive::execute(Stack &stack, IP &ip) {
    action(stack, ip);
}

iWordptr Primitive::clone() {
    return new Primitive(*this);
}