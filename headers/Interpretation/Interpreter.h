#ifndef MOVFORTH_INTER_INTERPRETER_H
#define MOVFORTH_INTER_INTERPRETER_H

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

        void init_words();

    public:
        Wordptr find(const std::string &name);
        Interpreter();
        std::vector<Wordptr> get_dictionary();
    };
}

#endif //MOVFORTH_INTERPRETER_H
