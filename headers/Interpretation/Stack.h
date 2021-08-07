#ifndef MOVFORTH_INTER_STACK_H
#define MOVFORTH_INTER_STACK_H

#include <iostream>
#include <variant>
#include "iWord.h"
#include "../../headers/Print.h"

namespace mfc {

    class Stack {
        std::vector<Data> stack;

    public:
        void push(int number) {
            stack.emplace_back(number);
        }
        void push(iWordptr iWord_pointer) {
            stack.emplace_back(iWord_pointer);
        }
        void push(Data thing){
            stack.push_back(thing);
        }

        int pop_number(){
            if(stack.empty()) println("tried popping empty stack");

            if(stack.back().type() == Data::number) { // int
                auto ret = stack.back().to_type<Data::number_t>();
                stack.pop_back();
                return ret;
            }
            if(stack.back().type() == Data::iword) { // iWord ptr
                println("tried popping cfa expecting int");
                return 0;
            }
            return 0;
        }

        iWordptr pop_iWord_pointer(){
            if(stack.size() == 0) println("tried popping empty stack");

            if(stack.back().type() == Data::number) { // int
                std::cout << "tried popping int expecting cfa" << std::endl;
                return nullptr;
            }
            if(stack.back().type() == Data::iword) { // iWord ptr
                auto ret = stack.back().to_type<Data::iword_t>();
                stack.pop_back();
                return ret;
            }
            return nullptr;
        }

        Data pop(){
            if(stack.empty()) println("tried popping empty stack");
            auto ret = stack.back();
            stack.pop_back();
            return ret;
        }

        Data top(){
            return stack.back();
        }

        int size(){
            return stack.size();
        }

        void for_each(std::function<void(Data)> action){
            for(auto thing : stack)
                action(thing);
        }
    };



}

#endif
