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

    class DataType {
    public:
        virtual std::string name() = 0;
    };
    class Number : public DataType{
    public:
        Number(int num) : num(num) {}
        int num;
    private:
        std::string name() override {
            return std::to_string(num);
        }
    };
    class Cfa : public DataType{
    public:
        Cfa(Word_ptr cfa) : cfa(cfa) {}
        Word_ptr cfa;
        std::string name() override {

            return "[CFA]" + cfa->name();
        }
    };

    class StackData {
    public:
        DataType_enum type_enum;
        std::shared_ptr<DataType> data_ptr;

        static StackData make_data(int num){
            return StackData(NUMBER, std::make_shared<Number>(num));
        }
        static StackData make_data(const Cfa& cfa){
            return StackData(CFA, std::make_shared<Cfa>(cfa));
        }
    private:
        StackData(DataType_enum e, std::shared_ptr<DataType> data)
                : type_enum(e), data_ptr(std::move(data)) {}
    };

    class Stack {
    private:
        std::vector<StackData> me;
    public:
        DataType_enum top_type() {
            return me.back().type_enum;
        }

        int pop_num() {
            if(me.size() == 0){
                std::cout << "tried to pop empty stack" << std::endl;
                return 0;
            }
            if (me.back().type_enum == NUMBER) {
                auto num = ((Number*)(me.back().data_ptr.get()))->num;
                me.pop_back();
                return num;
            }

            std::cout << "popped Cfa with the intent to use as Number" << std::endl;
            return 0;
        }

        Word_ptr pop_cfa() {
            if (me.back().type_enum == CFA) {
                auto cfa = ((Cfa*)(me.back().data_ptr.get()))->cfa;
                me.pop_back();
                return cfa;
            }
            std::cout << "popped Number with the intent to use as Cfa" << std::endl;
            return 0;
        }

        void push_num(int pushed) {
            me.emplace_back(StackData::make_data(pushed));
        }

        void push_cfa(Word_ptr pushed) {
            std::cout << "stack pushing a " << pushed->name() << std::endl;
            me.emplace_back(StackData::make_data(pushed));
        }

        int top_num() {
            if (me.back().type_enum == NUMBER)
                return static_cast<Number*>(me.back().data_ptr.get())->num;

            std::cout << "accessed Cfa with the intent to use as int" << std::endl;
            return 0;
        }

        Word_ptr top_CFA() {
            if (me.back().type_enum == CFA)
                return ((Cfa*)(me.back().data_ptr.get()))->cfa;

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
