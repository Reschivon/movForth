#ifndef MOVFORTH_IRGENERATOR_H
#define MOVFORTH_IRGENERATOR_H

#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include "../Symbolic/sWord.h"
#include "Variables.h"
#include "FBuilder.h"

using namespace llvm;
namespace mov {

    class IRGenerator {
    private:
        std::unordered_map<sWordptr, Function*> visited_words{};

        LLVMContext the_context;
        std::unique_ptr<Module> the_module;

        void declare_printf();
        Function* make_main();

        void print_module();
        static void exec_module();

    public:
        IRGenerator();

        Function* get_function(sWordptr word);

        Function* generate(sWord *fword, bool is_root);

        int hello_world();
        void hello_world2();
    };
}



#endif //MOVFORTH_IRGENERATOR_H
