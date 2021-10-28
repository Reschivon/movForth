
#ifndef MOVFORTH_OBJECTGENERATOR_H
#define MOVFORTH_OBJECTGENERATOR_H

#include <llvm/IR/Module.h>
#include "IRGenerator.h"

using namespace llvm;
namespace mov {
    class ObjectGenerator {
    public:
        static bool generate(const std::string &filename, IRModule mod);
        static void link(const std::string &assembly_name, const std::string &executable_name);
    };
}

#endif //MOVFORTH_OBJECTGENERATOR_H
