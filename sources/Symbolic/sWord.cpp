
#include "../../headers/Symbolic/sWord.h"

#include <utility>

using namespace mov;

Effects Effects::neutral{};

sWord::sWord(std::string  name, primitive_words id)
    : name(std::move(name)), id(id) {}

sWord::sWord(std::string  name, primitive_words id, Effects effects)
    : name(std::move(name)), id(id), effects(std::move(effects)) {}

bool sWord::branchy() {
    return
            id == primitive_words::BRANCH ||
            id == primitive_words::BRANCHIF ||
            id == primitive_words::EXIT;
}
