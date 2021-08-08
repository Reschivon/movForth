#include <utility>
#include <iostream>
#include "iWord.h"
#include "../PrimitiveEffects.h"

#ifndef MOVFORTH_INTER_iWordGEN_H
#define MOVFORTH_INTER_iWordGEN_H

namespace mov {
    class iWordGenerator {
    private:
        std::unordered_map<std::string, std::function<iWordptr()>>
                generator_lookup;

    public:
        void register_primitive(const std::string& name, primitive_words id, std::function<void(Stack &, IP &)> action, bool stateful = false) {
            register_lambda_word(name, id, std::move(action), false, stateful);
        }

        void register_lambda_word(const std::string& name, primitive_words id, std::function<void(Stack&, IP&)> action, bool immediate, bool stateful = false){
            generator_lookup[name] = [=]{
                return new Primitive(name, id, immediate, action, stateful);
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