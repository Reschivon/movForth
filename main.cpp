
#include "headers/Interpretation/Interpreter.h"
#include "headers/Symbolic/Pass.h"
#include "headers/Generation/IRGenerator.h"
#include "headers/Generation/ObjectGenerator.h"

int main(int argc, char* argv[]) {

    std::string forth_path = "../boot.fs";

    if(argc != 1) { // something passed! A .fs filename?
        if(argc != 2) {
            std::cout << "Pass only one argument, the path to a Forth file" << std::endl;
            return 1;
        }
        forth_path = std::string(argv[1]);
    }

    println("Compiling Forth file ", forth_path);
    std::string program_name = forth_path.substr(0, forth_path.find_last_of("."));

    // Create a plain old interpreter that interprets
    // the contents of the Forth file boot.fs
    mov::Interpreter interpreter(forth_path);

    // Extract the compiled word "main" from the interpreter's
    // dictionary. "main" has pointers to each of its definition words,
    // which also have pointers to each of their definition words
    auto word_to_compile = interpreter.find("main");

    // Create an analysis object for static analysis of
    // the compiled definition
    mov::Analysis analysis;

    // Do static analysis on "main," and return symbolic information
    // computed from "main." in the form of a mov::sWord
    auto converted_word = analysis.static_analysis(word_to_compile);

    // Show the fruits of labor from static analysis
    println();
    println();
    mov::Analysis::show_word_info(converted_word);

    // Generate IR from the symbolic object
    mov::IRGenerator ir_generator;
    auto module = ir_generator.generate(converted_word);

    // Run the new module
    // Note: invokes `lli` command in a new shell instance
    // May not work on every system

    // ir_generator.exec_module();

    mov::ObjectGenerator::generate(program_name + ".S", *module);

    // Link the assembly file provided
    // Note: invokes `clang++` command in a new shell instance
    // May not work on every system
    mov::ObjectGenerator::link(program_name + ".S", program_name);

}

