#include <utility>
#include <iostream>
#include "Word.h"

#ifndef MOVFORTH_INTER_WORDGEN_H
#define MOVFORTH_INTER_WORDGEN_H

namespace mfc {
    class WordGenerator {
    private:
        std::unordered_map<std::string, std::function<Wordptr()>>
                generator_lookup;

    public:
        void register_primitive(std::string name, std::function<void(Stack &, IP &)> action) {
            register_lambda_word(name, false, action);
        }

        void register_lambda_word(std::string name, bool immediate, std::function<void(Stack&, IP&)> action){
            generator_lookup[name] = [=]{
                return new Primitive(name, immediate, action);
            };
        }

        Wordptr get(const std::string &name){
            auto primitive = generator_lookup.find(name);
            if(primitive == generator_lookup.end())
                return nullptr;

            return primitive->second();
        }
    };

}

#endif