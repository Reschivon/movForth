
#include "Word.h"

using namespace sym;

void Word::definition_to_string() {
    println("[", name, "]");
    println("input parameters: ", effects.num_popped, " output frames: ", effects.num_pushed);
    for(auto thing : definition)
        print(thing->name, " ");
    println();
}