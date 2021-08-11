#include <utility>
#include "../../headers/Interpretation/iWord.h"

using namespace mov;

int DictData::as_number()      { return std::get<int>(*this);}
iWord* DictData::as_word()
{
    if(is_forth_word())
        return std::get<ForthWord*>(*this);
    if(is_primitive())
        return std::get<Primitive*>(*this);
    return std::get<iWord*>(*this); // assume word if not FW
}
ForthWord* DictData::as_forth_word()  { return std::get<ForthWord*>(*this);}
Primitive* DictData::as_primitive()   { return std::get<Primitive*>(*this);}

DictData::DictData(DictData::dict_data_var_type data) : dict_data_var_type(data) {}

std::string DictData::to_string() {
    if(is_word())
        return as_word()->name();
    if(is_number())
        return std::to_string(as_number());
    return "undef";
}

DictData::DictData() : DictData::dict_data_var_type(nullptr) {}

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

        DictData current_cell = *it;
        if(current_cell.is_word())
            current_cell.as_word()->execute(it);
        else
            println("Too many numbers in definition, no LITERAL to consume them");
    }
}
void ForthWord::add(DictData data){
    definition.push_back(data);
}

void ForthWord::definition_to_string() {
    for(auto thing : definition)
        print(thing.to_string(), " ");
}

void ForthWord::set(int index, DictData value) {
    definition.at(index) = value;
}

Primitive::Primitive(std::string name, primitive_words id, bool immediate, std::function<void(IP&)> action, bool stateful)
    : iWord(std::move(name), id, immediate, stateful), action(std::move(action)) {}

void Primitive::execute(IP &ip) {
    action(ip);
}

