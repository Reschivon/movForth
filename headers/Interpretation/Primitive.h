//
// Created by reschivon on 11/17/21.
//

#ifndef MOVFORTH_PRIMITIVE_H
#define MOVFORTH_PRIMITIVE_H

#include "iWord.h"
#include "iData.h"
#include "Interpreter.h"

namespace mov {

    /**
     * Primitive word
     * Get singletons from primitive_lookup
     */
    class Primitive : public iWord {
        std::function<void(IP, Interpreter &)> action;

        friend class iWordGenerator;

    public:
        Primitive(std::string name, primitive_words id, bool immediate,
                  std::function<void(IP , Interpreter & )> action, bool stateful);

        void execute(IP ip, Interpreter &interpreter) override;

    };

    class TO : public Primitive {
    public:
        explicit TO();
    };

    class ToLocal : public Primitive {
    public:
        Local local;
        std::unordered_map<Local, iData, LocalHasher> &locals;

        explicit ToLocal(Local local, std::unordered_map<Local, iData, LocalHasher> &locals);

        std::string name() override;
    };

    class FromLocal : public Primitive {
    public:
        Local local;
        std::unordered_map<Local, iData, LocalHasher> &locals;

        explicit FromLocal(Local local, std::unordered_map<Local, iData, LocalHasher> &locals);

        std::string name() override;
    };
}
#endif //MOVFORTH_PRIMITIVE_H
