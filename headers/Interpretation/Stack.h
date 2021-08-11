#ifndef MOVFORTH_INTER_STACK_H
#define MOVFORTH_INTER_STACK_H

#include <iostream>
#include <variant>
#include "iWord.h"
#include "../../headers/Print.h"
#include "iData.h"

namespace mov {

    /**
     * Stack of the interpreter
     * Holds Data objects as elements
     */
    class Stack {
        std::vector<DictData> stack{};

    public:
        void push(int number) {
            stack.emplace_back(number);
        }
        void push(iWordptr iWord_pointer) {
            stack.emplace_back(iWord_pointer);
        }
        void push(DictData thing){
            stack.push_back(thing);
        }

        /**
         * @return the top number, or 0 is top is not number
         */
        int pop_number(){
            if(stack.empty())
                println("tried popping empty stack");

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

        /**
        * @return the top word, or nullptr is top is not number
        */
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

        /**
        * @return the top Data, or an empty Data if stack is empty
        */
        DictData pop(){
            if(stack.empty()) {
                println("tried popping empty stack");
                return DictData(nullptr);
            }
            auto ret = stack.back();
            stack.pop_back();
            return ret;
        }

        DictData top(){
            return stack.back();
        }

        int size(){
            return stack.size();
        }

        void for_each(std::function<void(DictData)> action){
            for(auto thing : stack)
                action(thing);
        }
    };



}

#endif
