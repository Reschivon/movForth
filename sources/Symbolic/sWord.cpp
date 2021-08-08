
#include "../../headers/Symbolic/sWord.h"

#include <utility>

using namespace mov;

Effects Effects::neutral{};

void sWord::definition_to_string() {
    println("[", name, "]");
    println("input parameters: ", effects.num_popped, " output frames: ", effects.num_pushed);
    for(auto &thing : instructions)
        print(thing->name(), " ");
    println();
}

sWord::sWord(std::string  name, primitive_words id)
    : name(std::move(name)), id(id) {}

sWord::sWord(std::string  name, primitive_words id, Effects effects)
    : name(std::move(name)), id(id), effects(std::move(effects)) {}
