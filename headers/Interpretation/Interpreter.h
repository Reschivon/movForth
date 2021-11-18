#ifndef MOVFORTH_INTER_INTERPRETER_H
#define MOVFORTH_INTER_INTERPRETER_H

#include <vector>

#include "Input.h"

namespace mov {
    class Stack;
    class iData;
    class iWord;
    /**
     * Forth Interpreter that holds state
     */

    class iWordGenerator;
    class Interpreter {

        iWordGenerator &word_generator;
        void init_words();

    public:
        file_input input;
        std::vector<iData> dictionary;
        Stack &stack;
        bool immediate = true;

        /**
         * Finds the most recent dictionary entry that matches the given name
         * @param name  name of word to be found
         * @return a pointer to the word, if found. Otherwise nullptr
         */
        iWord* find(const std::string &name);

        /**
         * Creates an interpreter object and run the given Forth file
         * @param path relative or absolute path to the Forth file
         */
        explicit Interpreter(const std::string& path);

        bool interpret();
    };
}

#endif
