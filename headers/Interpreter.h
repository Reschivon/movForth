#ifndef MOVFORTH_INTERPRETER_H
#define MOVFORTH_INTERPRETER_H

#include <vector>
#include "Input.h"
#include "Word.h"
#include "WordGenerator.h"
#include "Stack.h"

namespace mfc {

    class Interpreter {
        file_input input;
        std::vector<Wordptr> dictionary;
        Stack stack;
        bool immediate = true;
        WordGenerator wordGenerator;

        Wordptr find(const std::string &name);

        void init_words();

    public:
        Interpreter();
    };
}

#endif //MOVFORTH_INTERPRETER_H
