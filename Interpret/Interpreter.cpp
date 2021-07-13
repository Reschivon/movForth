
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
        return nullptr;
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

        //say "[interpreter] " << token << " [state] " << (immediate?"imm":"comp") over

        if(word_ptr == nullptr){
            // might be a number
            try{
                int num = stoi(token);

                if(immediate)
                    stack->push(num);
                else {
                    auto forth_word = dynamic_cast<ForthWord*>(dictionary.back());
                    if(forth_word == nullptr) {
                        std::cout << "attempted to compile LITERAL to a primitive word"
                                  << std::endl;
                        continue;
                    }
                    forth_word->append_xt(new Literal(stack));
                    forth_word->append_data(num);
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
                auto forth_word = dynamic_cast<ForthWord*>(dictionary.back());
                if(forth_word == nullptr) {
                    std::cout << "attempted to compile xts to a primitive word"
                              << std::endl;
                    continue;
                }
                forth_word->append_xt(word_ptr);
            }
        }
    }
}

void Interpreter::init_words(){
    append(new LambdaPrimitive("+", [&] {
        stack->push(stack->pop() + stack->pop());
        return 1;
    }));

    append(new LambdaPrimitive("-", [&] {
        auto one = stack->pop();
        auto two = stack->pop();
        stack->push(two - one);
        return 1;
    }));

    append(new LambdaPrimitive("*", [&] {
        stack->push(stack->pop() * stack->pop());
        return 1;
    }));

    append(new LambdaPrimitive("/", [&] {
        stack->push(stack->pop() / stack->pop());
        return 1;
    }));

    append(new LambdaPrimitive("swap", [&] {
        auto top = stack->pop(), second = stack->pop();
        stack->push(top);
        stack->push(second);
        return 1;
    }));

    append(new LambdaPrimitive("dup", [&] {
        stack->push(stack->top());
        return 1;
    }));

    append(new LambdaPrimitive("drop", [&] {
        stack->pop();
        return 1;
    }));

    append(new LambdaPrimitive(".", [&] {
        std::cout << stack->pop() << std::endl;
        return 1;
    }));

    append(new LambdaPrimitive(".S", [&] {
        for(auto s : *stack)
            say s << " ";
        return 1;
    }));

    append(new LambdaPrimitive("see", [&] {
        for(auto word_ptr : dictionary){
            say "[" + word_ptr->name + "] ";
            say ((word_ptr->immediate)?"IMM ":" ");
            auto forth_word = dynamic_cast<ForthWord*>(word_ptr);
            if(forth_word != nullptr)
                forth_word->print_def();
            say std::endl;
        }
        return 1;
    }));

    append(new LambdaPrimitive("word", [&] {
        file >> token_buffer;
        return 1;
    }));

    append(new LambdaPrimitive("[", [&] {
        immediate = true;
        return 1;
    }));

    append(new LambdaPrimitive("]", [&] {
        immediate = false;
        return 1;
    }));

    append(new LambdaPrimitive("immediate", true, [&] {
        dictionary.back()->immediate = true;
        return 1;
    }));

    // relative arithmetic ONLY. Spec does not guarantee it
    // points to a specific address
    append(new LambdaPrimitive("here", [&] {
        auto last_word = dynamic_cast<ForthWord*>(dictionary.back());
        if(last_word == nullptr)
            std::cout << "attempted to use HERE for purpose other than compilation" <<
            "This is not supported since it destroys the illusion that you" <<
            "are using an interpreted Forth" << std::endl;
        else
            stack->push((last_word->definition_size()));
        return 1;
    }));

    append(new LambdaPrimitive("_create", [&] {
        append(new ForthWord(token_buffer, false));
        token_buffer.clear();
        return 1;
    }));

    auto create_word = new ForthWord("create", false);
    create_word->append_xt(find("word"));
    create_word->append_xt(find("_create"));
    append(create_word);

    auto colon_word = new ForthWord(":", false);
    colon_word->append_xt(find("create"));
    colon_word->append_xt(find("]"));
    append(colon_word);

    auto exit_word = new ForthWord(";", true);
    exit_word->append_xt(find("["));
    append(exit_word);
}
