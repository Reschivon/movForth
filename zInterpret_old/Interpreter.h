//
// Created by Administrator on 7/11/2021.
//

#ifndef MOVFORTH_INTERPRETER_H
#define MOVFORTH_INTERPRETER_H
#include "../Word/Word.h"
#include "Stack.h"
#include "WordGenerator.h"
#include <fstream>
#include <memory>

namespace mfc {

    class Interpreter {
    public:
        std::vector<Word_ptr> dictionary;
        bool immediate = true;
        Stack stack;

        Word_ptr find(const std::string &name);

        void append(Word_ptr word_ptr) {
            dictionary.push_back(word_ptr);
        }

        Interpreter();

    private:
        std::ifstream file;
        std::string token_buffer;
        void init_words();
        WordGenerator wordGenerator;
    };

} // namespace

#endif //MOVFORTH_INTERPRETER_H
