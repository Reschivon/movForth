#include <utility>

#include "../../headers/Interpretation/iWord.h"

using namespace mov;

iWord::iWord(std::string name, bool immediate, bool stateful)
        : _name(name), immediate(immediate), stateful(stateful) {}

iWord::iWord(std::string name, primitive_words id, bool immediate, bool stateful)
    : _name(name), id(id), immediate(immediate), stateful(stateful) {}

std::string iWord::name() {
    return _name;
}
bool iWord::branchy() {
    return id == primitive_words::BRANCH || id == primitive_words::BRANCHIF;
}

