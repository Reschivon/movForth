//
// Created by reschivon on 11/17/21.
//
#include "../../headers/Interpretation/Stack.h"
using namespace mov;

void Stack::push(element number) {
    stack.emplace_back(number);
}

void Stack::push(iWordptr iWord_pointer) {
    stack.emplace_back(iWord_pointer);
}

void Stack::push(iData thing) {
    stack.push_back(thing);
}

element Stack::pop_number() {
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

iData Stack::pop() {
    if(stack.empty()) {
        println("tried popping empty stack");
        return iData(nullptr);
    }
    auto ret = stack.back();
    stack.pop_back();
    return ret;
}

iData Stack::top() {
    if(stack.empty()) {
        println("tried getting top of empty stack");
        return iData(nullptr);
    }
    return stack.back();
}

int Stack::size() {
    return stack.size();
}

void Stack::for_each(std::function<void(iData)> action) {
    for(auto thing : stack)
        action(thing);
}
