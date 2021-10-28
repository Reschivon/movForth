
#include "headers/Interpretation/Interpreter.h"
#include "headers/Symbolic/Pass.h"
#include "headers/Generation/IRGenerator.h"
#include "headers/Generation/ObjectGenerator.h"

int main() {

    // Create a plain old interpreter that interprets
    // the contents of the Forth file boot.fs
    mov::Interpreter interpreter("../boot.fs");

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

    ir_generator.exec_module();

    mov::ObjectGenerator::generate("../Out.S", *module);

    // Link the assembly file provided
    // Note: invokes `clang++` command in a new shell instance
    // May not work on every system
    mov::ObjectGenerator::link("../Out.S", "LinkedOut");

}

