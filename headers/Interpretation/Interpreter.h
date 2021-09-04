#ifndef MOVFORTH_INTER_INTERPRETER_H
#define MOVFORTH_INTER_INTERPRETER_H

#include <vector>
#include "Input.h"
#include "iWord.h"
#include "WordGenerator.h"
#include "Stack.h"

namespace mov {
    /**
     * Forth Interpreter that holds state
     */
    class Interpreter {
        file_input input;
        std::vector<iData> dictionary;
        Stack stack;
        bool immediate = true;
        iWordGenerator word_generator;

        void init_words();

    public:
        /**
         * Finds the most recent dictionary entry that matches the given name
         * @param name  name of word to be found
         * @return a pointer to the word, if found. Otherwise nullptr
         */
        iWordptr find(const std::string &name);

        /**
         * Creates an interpreter object and run the given Forth file
         * @param path relative or absolute path to the Forth file
         */
        explicit Interpreter(const std::string& path);
    };
}

#endif
