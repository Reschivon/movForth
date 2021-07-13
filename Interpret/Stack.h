//
// Created by Administrator on 7/12/2021.
//

#ifndef MOVFORTH_STACK_H
#define MOVFORTH_STACK_H

#include <stack>
#include <vector>

class Stack{
private:
    std::vector<int> me;
public:
    inline int pop(){
        auto ret = me.back();
        me.pop_back();
        return ret;
    }

    inline void push(int pushed){
        me.push_back(pushed);
    }

    inline int top(){
        return me.back();
    }

    auto begin(){
        return me.begin();
    }

    auto end(){
        return me.end();
    }
};

#endif //MOVFORTH_STACK_H
