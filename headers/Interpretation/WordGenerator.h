#ifndef MOVFORTH_INTER_iWordGEN_H
#define MOVFORTH_INTER_iWordGEN_H


#include <utility>
#include <iostream>
#include <unordered_map>
#include <functional>

#include "../PrimitiveEnums.h"
#include "iWord.h"
#include "Interpreter.h"
#include "IP.h"


namespace mov {
    class iWordGenerator {

    public:
        std::unordered_map<std::string, std::function<iWord*()>> generator_lookup{};

        void register_primitive(const std::string& name, primitive_words id, std::function<void(IP, Interpreter&)> action, bool stateful = false);

        void register_lambda_word(const std::string& name, primitive_words id, std::function<void(IP, Interpreter&)> action, bool immediate, bool stateful = false);

        iWordptr get(const std::string &name);
    };

}

#endif
