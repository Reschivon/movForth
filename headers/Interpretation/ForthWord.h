//
// Created by reschivon on 11/17/21.
//

#ifndef MOVFORTH_FORTHWORD_H
#define MOVFORTH_FORTHWORD_H

#include "iWord.h"
#include "iData.h"
#include "Interpreter.h"

namespace mov {
    /**
      * Defined word, holds a definition
      */
    class ForthWord : public iWord {
        std::list<iData> definition;

    public:

        ForthWord(std::string name, bool immediate);

        void execute(IP ip, Interpreter &interpreter) override;

        std::unordered_map<Local, iData, LocalHasher> locals;

        /**
         * Append definition
         */
        void add(iData data);

        /**
         * Set index of definition to value
         * @throws std::__out_of_range if invalid index
         */
        void set(int index, iData value);

        /**
         * For debuggin only. Does what you think
         */
        void definition_to_string();

        /**
         * @return reference to definition
         */
        std::list<iData> &def() {
            return definition;
        }

        /**
         *
         * @return the size of definition
         */
        uint def_size(){
            int size = 0;
            for(auto thing : definition)
                size++;
            return size;
        }
    };



}

#endif //MOVFORTH_FORTHWORD_H
