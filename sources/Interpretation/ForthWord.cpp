//
// Created by reschivon on 11/17/21.
//

#include "../../headers/Interpretation/ForthWord.h"

using namespace mov;


ForthWord::ForthWord(std::string name, bool immediate)
        : iWord(name, immediate, false) {}

void ForthWord::execute(IP ip, Interpreter &interpreter) {
    for(auto &[local, value] : locals) {
        locals[local] = iData(nullptr);
    }

    for(auto it = definition.begin(); it != definition.end(); it++) {

        // println("       [exec] ", (it->is_word()?it->as_word()->name():std::to_string(it->as_number())), " ");

        iData current_cell = *it;
        if(current_cell.is_word())
            current_cell.as_word()->execute(it, interpreter);
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
    auto front = definition.begin();
    std::advance(front, index);
    *front = value;
}


