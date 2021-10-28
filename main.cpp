
#include "headers/Interpretation/Interpreter.h"
#include "headers/Symbolic/Pass.h"
#include "headers/Generation/IRGenerator.h"
#include "headers/Generation/ObjectGenerator.h"

int main(int argc, char* argv[]) {

    std::string forth_path;

    if(argc == 1) { // no args, set to default
        forth_path = "../CompilationSpace/boot.fs";
    }else{ // argument(s) passed! Is it a .fs file?
        if(argc == 2) {
            forth_path = std::string(argv[1]);
        }else{
            std::cout << "Pass only one argument - the path to a Forth file" << std::endl;
            return 1;
        }
    }

    std::string program_name = forth_path.substr(0, forth_path.find_last_of('.'));


    // Create a plain old interpreter that interprets
    // the contents of the Forth file boot.fs
    mov::Interpreter interpreter(forth_path);
    bool interpret_error = interpreter.interpret();
    if(interpret_error) {
        println("Interpretation phase ended with errors");
        return 1;
    }


    // Extract the compiled word "main" from the interpreter's
    // dictionary. "main" has pointers to each of its definition words,
    // which also have pointers to each of their definition words
    auto word_to_compile = interpreter.find("main");
    if(!word_to_compile) {
        println("Could not find a user-written or generated word with name \"main\"");
        return 1;
    }


    // Do static analysis on "main," and return symbolic information
    // computed from "main" in the form of a mov::sWord
    mov::Analysis analysis;
    auto converted_word = analysis.static_analysis(word_to_compile);


    // Show the fruits of labor from static analysis
    mov::Analysis::show_word_info(converted_word);


    // Generate IR from the symbolic object
    mov::IRGenerator ir_generator;
    auto module_result = ir_generator.generate(converted_word);
    if(module_result.second) {
        println("Error during IR generation");
        return 1;
    }


    ir_generator.print_module(program_name, true);

    // Run the new module_result
    // Note: invokes `lli` command in a new shell instance
    // May not work on every system

    ir_generator.exec_module(program_name);

    bool generate_error = mov::ObjectGenerator::generate(
            program_name + ".S", module_result.first);
    if(generate_error) {
        println("Error during executable generation");
        return 1;
    }

    // Link the assembly file provided
    // Note: invokes `clang++` command in a new shell instance
    // May not work on every system
    mov::ObjectGenerator::link(program_name + ".S", program_name);

}

