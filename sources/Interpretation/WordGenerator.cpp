//
// Created by reschivon on 11/17/21.
//

#include "../../headers/Interpretation/WordGenerator.h"
#include "../../headers/Interpretation/Primitive.h"

using namespace mov;

void
mov::iWordGenerator::register_lambda_word(const std::string &name, primitive_words id, std::function<void(IP, Interpreter&)> action,
                                          bool immediate, bool stateful) {
    generator_lookup[name] = [=]{
        return new Primitive(name, id, immediate, action, stateful);
    };
}

void
mov::iWordGenerator::register_primitive(const std::string &name, primitive_words id, std::function<void(IP, Interpreter&)> action,
                                        bool stateful) {
    register_lambda_word(name, id, action, false, stateful);
}

iWordptr mov::iWordGenerator::get(const std::string &name) {
    auto primitive = generator_lookup.find(name);
    if(primitive == generator_lookup.end())
        return nullptr;

    return primitive->second();
}
