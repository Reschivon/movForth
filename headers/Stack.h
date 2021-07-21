#ifndef MOVFORTH_STACK_H
#define MOVFORTH_STACK_H

#include <iostream>
#include <variant>
#include "Word.h"

namespace mfc {

    class Stack {
        std::vector<Data> stack;

    public:
        void push(int number) {
            stack.emplace_back(number);
        }
        void push(Wordptr word_pointer) {
            stack.emplace_back(word_pointer);
        }
        void push(Data thing){
            stack.push_back(thing);
        }

        int pop_number(){
            if(stack.back().is_num()) { // int
                auto ret = stack.back().as_num();
                stack.pop_back();
                return ret;
            }
            if(stack.back().is_xt()) { // word ptr
                std::cout << "tried popping cfa expecting int" << std::endl;
                return 0;
            }
            return 0;
        }

        Wordptr pop_word_pointer(){
            if(stack.back().is_num()) { // int
                std::cout << "tried popping int expecting cfa" << std::endl;
                return nullptr;
            }
            if(stack.back().is_xt()) { // word ptr
                auto ret = stack.back().as_xt();
                stack.pop_back();
                return ret;
            }
            return nullptr;
        }

        Data pop(){
            auto ret = stack.back();
            stack.pop_back();
            return ret;
        }

        Data top(){
            return stack.back();
        }

        void for_each(std::function<void(Data)> action){
            for(auto thing : stack)
                action(thing);
        }
    };



}

#endif
