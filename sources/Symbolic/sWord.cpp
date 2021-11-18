
#include "../../headers/Symbolic/sWord.h"

#include <utility>

using namespace mov;

Effects Effects::neutral{};

sWord::sWord(std::string  name, primitive_words id)
    : sWord(std::move(name), id, Effects()) {}

sWord::sWord(std::string  name, primitive_words id, Effects effects)
    : name(std::move(name)), id(id), effects(std::move(effects)) {
    locals.reserve(10);
}

bool sWord::branchy() {
    return
            id == primitive_words::BRANCH ||
            id == primitive_words::BRANCHIF ||
            id == primitive_words::EXIT;
}
