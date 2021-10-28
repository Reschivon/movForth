
#ifndef MOVFORTH_OBJECTGENERATOR_H
#define MOVFORTH_OBJECTGENERATOR_H

#include <llvm/IR/Module.h>

using namespace llvm;
namespace mov {
    class ObjectGenerator {
    public:
        static void generate(const std::string &filename, Module &module);
        static void link(const std::string &assembly_name, const std::string &executable_name);
    };
}

#endif //MOVFORTH_OBJECTGENERATOR_H
