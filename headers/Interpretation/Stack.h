#ifndef MOVFORTH_INTER_STACK_H
#define MOVFORTH_INTER_STACK_H

#include <iostream>
#include <variant>
#include <vector>
#include <functional>

#include "../Constants.h"

#include "iData.h"
#include "iWord.h"


namespace mov {

    /**
     * Stack of the interpreter
     * Holds Data objects as elements
     */
    class Stack {
        std::vector<iData> stack{};

    public:
        void push(element number);
        void push(iWordptr iWord_pointer);
        void push(iData thing);

        /**
         * @return the top number, or 0 is top is not number
         */
        element pop_number();


        /**
        * @return the top Data, or an empty Data if stack is empty
        */
        iData pop();

        iData top();

        int size();

        void for_each(std::function<void(iData)> action);
    };



}

#endif
