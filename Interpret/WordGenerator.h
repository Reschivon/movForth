//
// Created by Administrator on 7/13/2021.
//

#ifndef MOVFORTH_WORDGENERATOR_H
#define MOVFORTH_WORDGENERATOR_H

#include <unordered_map>
#include <string>
#include <functional>

#include "../Word/Word.h"

namespace mfc {
    class WordGenerator {
    private:
        std::unordered_map<std::string, std::function<Word_ptr()>>
        generator_lookup;

    public:
        // holy fuck what I am I doing here
        template<typename T, typename ...Args>
        void register_type(std::string name, Args... args) {
            generator_lookup[std::move(name)] = [&]{
                return std::make_shared<T>(std::forward<Args>(args)...);
            };
        }

        void register_lambda_word(std::string name, std::function<int()> action);

        void register_lambda_word(std::string name, bool immediate, std::function<int()> action);

        Word_ptr get(const std::string &name);
    };

}


#endif //MOVFORTH_WORDGENERATOR_H
