#ifndef MOVFORTH_IRGENERATOR_H
#define MOVFORTH_IRGENERATOR_H

#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include "llvm/IR/LegacyPassManager.h"

#include "../Symbolic/sWord.h"
#include "FBuilder.h"

using namespace llvm;
namespace mov {

    class IRGenerator {
    private:
        std::unordered_map<sWordptr, Function*> visited_words{};

        LLVMContext the_context;
        std::shared_ptr<Module> the_module;
        std::unique_ptr<legacy::FunctionPassManager> fpm;

        void declare_printf();
        Function* make_main();

        void print_module();
        static void exec_module();

        Function* generate_function(sWord *fword, bool is_root);
        Function* get_function(sWordptr word);

    public:
        IRGenerator();


        int hello_world();
        void hello_world2();

        std::shared_ptr<Module> generate(sWord *root);
    };
}



#endif //MOVFORTH_IRGENERATOR_H
