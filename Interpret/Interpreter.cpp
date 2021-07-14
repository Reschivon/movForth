
#include <algorithm>
#include <iostream>
#include "Interpreter.h"
#include <utility>
#define say std::cout <<
#define over << std::endl;

using namespace mfc;

Word_ptr Interpreter::find(const std::string& name) {
    auto find_result = std::find_if(dictionary.begin(), dictionary.end(),
                 [name](Word_ptr& other){return other->name == name;});

    if(find_result == dictionary.end())
        // try primitives
        return wordGenerator.get(name);

    return *find_result;
}

Interpreter::Interpreter(){
    stack = new Stack;
    file.open("../boot.fs");

    if(!file.is_open()) return;

    init_words();

    std::string token;
    while (file >> token) {
        Word_ptr word_ptr = find(token);
        if(!word_ptr)
            word_ptr = wordGenerator.get(token);

        //say "[interpreter] " << token << " [state] " << (immediate?"imm":"comp") over

        if(word_ptr == nullptr){
            // might be a number
            try{
                int num = stoi(token);

                if(immediate)
                    stack->push(num);
                else {
                    if(auto *forth_word = dynamic_cast<ForthWord*>(dictionary.back().get())){
                        forth_word->append_xt(std::make_shared<Literal>(stack));
                        forth_word->append_data(num);
                    }else{
                        std::cout << "attempted to compile LITERAL to a primitive word"
                                  << std::endl;
                    }

                }

            } catch (...) {
                // not a number or word
                std::cout << "word >" << token << "< not found and not number" << std::endl;
            }

        }else{
            // it's a word
            if(word_ptr->immediate || immediate)
                word_ptr->execute();
            else {
                if(auto forth_word = dynamic_cast<ForthWord*>(dictionary.back().get())){
                    forth_word->append_xt(word_ptr);
                }else
                    std::cout << "attempted to compile xts to a primitive word"
                              << std::endl;
            }
        }
    }
}

void Interpreter::init_words(){
    // holy fuck what I am I doing here
    wordGenerator.register_lambda_word("+",[&] {
            stack->push(stack->pop() + stack->pop());
            return 1;
    });

    wordGenerator.register_lambda_word("-", [&] {
            auto one = stack->pop();
            auto two = stack->pop();
            stack->push(two - one);
            return 1;
    });


    wordGenerator.register_lambda_word("*", [&] {
            stack->push(stack->pop() * stack->pop());
            return 1;
    });


    wordGenerator.register_lambda_word("/", [&] {
            stack->push(stack->pop() / stack->pop());
            return 1;
    });

    wordGenerator.register_lambda_word("swap", [&] {
        auto top = stack->pop(), second = stack->pop();
        stack->push(top);
        stack->push(second);
        return 1;
    });

    wordGenerator.register_lambda_word("dup", [&] {
        stack->push(stack->top());
        return 1;
    });

    wordGenerator.register_lambda_word("drop", [&] {
        stack->pop();
        return 1;
    });

    wordGenerator.register_lambda_word(".", [&] {
        std::cout << stack->pop() << std::endl;
        return 1;
    });

    wordGenerator.register_lambda_word(".S", [&] {
        for(auto s : *stack)
            say s << " ";
        return 1;
    });

    wordGenerator.register_lambda_word("see", [&] {
        for(const Word_ptr& word_ptr : dictionary){
            say "[" + word_ptr->name + "] ";
            say ((word_ptr->immediate)?"IMM ":" ");
            auto forth_word = dynamic_cast<ForthWord*>(word_ptr.get());
            if(forth_word != nullptr)
                forth_word->print_def();
            say std::endl;
        }
        return 1;
    });

    wordGenerator.register_lambda_word("word", [&] {
        file >> token_buffer;
        return 1;
    });

    wordGenerator.register_lambda_word("[", [&] {
        immediate = true;
        return 1;
    });

    wordGenerator.register_lambda_word("]", [&] {
        immediate = false;
        return 1;
    });

    wordGenerator.register_lambda_word("immediate", true, [&] {
        dictionary.back()->immediate = true;
        return 1;
    });

    wordGenerator.register_type<Branch>("branch");

    wordGenerator.register_type<Literal>("literal", stack);


    // relative arithmetic ONLY. Spec does not guarantee it
    // points to a specific address
    wordGenerator.register_lambda_word("here", [&] {
        auto last_word = dynamic_cast<ForthWord*>(dictionary.back().get());
        if(last_word == nullptr)
            std::cout << "attempted to use HERE for purpose other than compilation" <<
            "This is not supported since it destroys the illusion that you" <<
            "are using an interpreted Forth" << std::endl;
        else
            stack->push((last_word->definition_size()));
        return 1;
    });

    wordGenerator.register_lambda_word("create", [&] {
        std::string next_token;
        file >> next_token;
        append(std::make_shared<ForthWord>(ForthWord(next_token, false)));
        return 1;
    });

    auto colon_word = std::make_shared<ForthWord>(ForthWord(":", false));
    colon_word->append_xt(find("create"));
    colon_word->append_xt(find("]"));
    append(colon_word);

    auto exit_word = std::make_shared<ForthWord>(ForthWord(";", true));
    exit_word->append_xt(find("["));
    append(exit_word);
}
