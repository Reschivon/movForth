//
// Created by Administrator on 7/12/2021.
//

#ifndef MOVFORTH_STACK_H
#define MOVFORTH_STACK_H

#include <stack>
#include <vector>
#include <iostream>
#include "../Word/Word.h"

namespace mfc {

    enum DataType_enum {
        NUMBER, CFA
    };

    class DataType {};
    class Number : public DataType{
    public:
        Number(int num) : num(num) {}
        int num;
    };
    class Cfa : public DataType{
    public:
        Cfa(Word_ptr cfa) : cfa(cfa) {}
        Word_ptr cfa;
    };

    class Stack {
    private:
        std::vector<std::pair<DataType_enum, std::unique_ptr<DataType>>> me;
    public:
        DataType_enum top_type() {
            return me.back().first;
        }

        int pop_num() {

            if (me.back().first == NUMBER) {
                auto num = ((Number*)(me.back().second.get()))->num;
                me.pop_back();
                return num;
            }

            std::cout << "popped Cfa with the intent to use as Number" << std::endl;
            return 0;
        }

        Word_ptr pop_cfa() {
            if (me.back().first == CFA) {
                auto cfa = ((Cfa*)(me.back().second.get()))->cfa;
                me.pop_back();
                return cfa;
            }
            std::cout << "popped Number with the intent to use as Cfa" << std::endl;
            return 0;
        }

        void push_num(int pushed) {
            me.emplace_back(NUMBER, std::make_unique<Number>(pushed));
        }

        void push_cfa(Word_ptr pushed) {
            me.emplace_back(CFA, std::make_unique<Cfa>(pushed));
        }

        int top_num() {
            if (me.back().first == NUMBER)
                return static_cast<Number*>(me.back().second.get())->num;

            std::cout << "accessed Cfa with the intent to use as int" << std::endl;
            return 0;
        }

        Word_ptr top_CFA() {
            if (me.back().first == CFA)
                return ((Cfa*)(me.back().second.get()))->cfa;

            std::cout << "accessed Number with the intent to use as Cfa" << std::endl;
            return 0;
        }

        auto begin() {
            return me.begin();
        }

        auto end() {
            return me.end();
        }
    };
}
#endif //MOVFORTH_STACK_H
