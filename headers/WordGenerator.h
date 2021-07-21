#include <utility>
#include <iostream>
#include "Word.h"

namespace mfc {
    class WordGenerator {
    private:
        std::unordered_map<std::string, std::function<Wordptr()>>
                generator_lookup;

    public:
        // holy fuck what I am I doing here
        template<typename T, typename ...Args>
        void register_type(std::string name, Args... args) {
            generator_lookup[std::move(name)] = [&]{
                return new T(std::forward<Args>(args)...);
            };
        }

        void register_lambda_word(std::string name, std::function<void(Stack&, IP&)> action) {
            register_lambda_word(name, false, action);
        }

        void register_lambda_word(std::string name, bool immediate, std::function<void(Stack&, IP&)> action){
            generator_lookup[name] = [=]{
                return new LambdaPrimitive(name, immediate, action);
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