#ifndef MOVFORTH_INTER_INTERPRETER_H
#define MOVFORTH_INTER_INTERPRETER_H

#include <vector>
#include "Input.h"
#include "iWord.h"
#include "WordGenerator.h"
#include "Stack.h"

namespace mfc {

    class Interpreter {
        file_input input;
        std::vector<iWordptr> dictionary;
        Stack stack;
        bool immediate = true;
        iWordGenerator iWordGenerator;

        void init_words();

    public:
        iWordptr find(const std::string &name);
        Interpreter();
        std::vector<iWordptr> get_dictionary();
    };
}

#endif //MOVFORTH_INTERPRETER_H
