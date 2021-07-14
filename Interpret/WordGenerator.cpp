#include "WordGenerator.h"

#include <utility>
#include <iostream>

using namespace mfc;

void WordGenerator::register_lambda_word(const std::string& name, const std::function<int()>& action) {
    register_lambda_word(name, false, action);
}

// TODO figure out WTF is happening that fails with [&]
void WordGenerator::register_lambda_word(const std::string& name, bool immediate, const std::function<int()>& action) {
    generator_lookup[name] = [=]{
        return std::make_shared<LambdaPrimitive>(name, immediate, action);
    };
}

Word_ptr WordGenerator::get(const std::string& name){
    /*std::cout << "get called ";
    for(auto thing : generator_lookup)
        std::cout << thing.first << " ";
    std::cout << std::endl;*/

    auto primitive = generator_lookup.find(name);
    if(primitive == generator_lookup.end())
        return nullptr;

    return primitive->second();

}