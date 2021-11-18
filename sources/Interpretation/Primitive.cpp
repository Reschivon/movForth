//
// Created by reschivon on 11/17/21.
//

#include "../../headers/Interpretation/Primitive.h"
#include "../../headers/Interpretation/Stack.h"
#include "../../headers/Interpretation/ForthWord.h"

using namespace mov;


Primitive::Primitive(std::string name, primitive_words id, bool immediate, std::function<void(IP, Interpreter&)> action, bool stateful)
        : iWord(std::move(name), id, immediate, stateful), action(std::move(action)) {}

void Primitive::execute(IP ip, Interpreter &interpreter) {
    action(ip, interpreter);
}

ToLocal::ToLocal(Local local, std::unordered_map<Local, iData, LocalHasher> &locals)
        : Primitive("toLocal", primitive_words::TOLOCAL, false,[&](IP ip, Interpreter &interpreter) {
    locals.at(local) = interpreter.stack.pop();
}, false),

          local(local),
          locals(locals)
{
    if(locals.find(local) == locals.end())
        locals.insert(std::make_pair(local, iData(nullptr)));
}

std::string ToLocal::name() {
    return iWord::name() + "(" + local.name + ")";
}

FromLocal::FromLocal(Local local, std::unordered_map<Local, iData, LocalHasher> &locals)

        : Primitive("fromLocal", primitive_words::FROMLOCAL, false, [&](IP ip, Interpreter &interpreter) {
    interpreter.stack.push(locals[local]);
}, false),

          local(local),
          locals(locals)
{}

std::string FromLocal::name() {
    return iWord::name() + "(" + local.name + ")";
}

TO::TO()
       : Primitive("to", primitive_words::TO, true,
            [](IP ip, Interpreter &interpreter) {
                  auto parent = interpreter.dictionary.back().as_forth_word();

                  std::string next_token = interpreter.input.next_token();
                  println("add Local ", next_token, " to ", parent->name());

                  Local this_local = Local{parent->name(), next_token};
                  parent->locals.insert(std::make_pair(this_local, iData(0)));
                  parent->add(iData(new ToLocal(this_local, parent->locals)));
            }
       , false)
{}
