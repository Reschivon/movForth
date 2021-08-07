#include <utility>
#include <iostream>
#include "iWord.h"

#ifndef MOVFORTH_INTER_iWordGEN_H
#define MOVFORTH_INTER_iWordGEN_H

namespace mov {
    class iWordGenerator {
    private:
        std::unordered_map<std::string, std::function<iWordptr()>>
                generator_lookup;

    public:
        void register_primitive(std::string name, std::function<void(Stack &, iData data, IP &)> action, bool stateful = false) {
            register_lambda_word(name, action, false,stateful );
        }

        void register_lambda_word(std::string name, std::function<void(Stack&, iData data, IP&)> action, bool immediate, bool stateful = false){
            generator_lookup[name] = [=]{
                return new Primitive(name, immediate, action, stateful);
            };
        }

        iWordptr get(const std::string &name){
            auto primitive = generator_lookup.find(name);
            if(primitive == generator_lookup.end())
                return nullptr;

            return primitive->second();
        }
    };

}

#endif