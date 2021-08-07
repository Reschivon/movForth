#ifndef MOVFORTH_INTER_STACK_H
#define MOVFORTH_INTER_STACK_H

#include <iostream>
#include <variant>
#include "iWord.h"
#include "../../headers/Print.h"

namespace mov {

    class Stack {
        std::vector<iData> stack;

    public:
        void push(int number) {
            stack.emplace_back(number);
        }
        void push(iWordptr iWord_pointer) {
            stack.emplace_back(iWord_pointer);
        }
        void push(iData thing){
            stack.push_back(thing);
        }

        int pop_number(){
            if(stack.empty()) println("tried popping empty stack");

            if(stack.back().is_number()) { // int
                auto ret = stack.back().as_number();
                stack.pop_back();
                return ret;
            }
            if(stack.back().is_word()) { // iWord ptr
                println("tried popping cfa expecting int");
                return 0;
            }
            return 0;
        }

        iWordptr pop_iWord_pointer(){
            if(stack.size() == 0) println("tried popping empty stack");

            if(stack.back().is_number()) { // int
                std::cout << "tried popping int expecting cfa" << std::endl;
                return nullptr;
            }
            if(stack.back().is_word()) { // iWord ptr
                auto ret = stack.back().as_word();
                stack.pop_back();
                return ret;
            }
            return nullptr;
        }

        iData pop(){
            if(stack.empty()) println("tried popping empty stack");
            auto ret = stack.back();
            stack.pop_back();
            return ret;
        }

        iData top(){
            return stack.back();
        }

        int size(){
            return stack.size();
        }

        void for_each(std::function<void(iData)> action){
            for(auto thing : stack)
                action(thing);
        }
    };



}

#endif
