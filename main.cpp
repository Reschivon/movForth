
#include "headers/Interpretation/Interpreter.h"
#include "headers/Symbolic/Pass.h"
#include "headers/Generation/IRGenerator.h"

int main() {

    // Create a plain old interpreter that interprets
    // the contents of the Forth file boot.fs
    mov::Interpreter interpreter("../boot.fs");

    // Extract the compiled word "test" from the interpreter's
    // dictionary. "test" has pointers to each of its definition words,
    // which also have pointers to each of their definition words
    auto word_to_compile = interpreter.find("main");

    // Create an analysis object for static analysis of
    // the compiled definition
    mov::Analysis analysis;

    // Do static analysis on "test".
    // 1. Control Analysis (compute the basic blocks of
    //    a definition and look for stack size inconsistencies
    //    between basic block edges)
    // 2. Data Flow Analysis (graph of data exchange between words.
    //    Stack elements are nodes and registers are edges. This is
    //    how the compiler converts from stack to SSA format)
    auto converted_word = analysis.static_analysis(word_to_compile);

    mov::Register r{1, 1, mov::Register::registerType::PARAM};
    std::unordered_map<mov::Register, std::string, mov::Register::RegisterHash> d;
    d.insert(std::make_pair(r, "tadaaa"));
    println(d.at(r));

    // Show the fruits of labor from static analysis
    println();
    println();
    mov::Analysis::show_word_info(converted_word);

    mov::IRGenerator ir_generator;
    ir_generator.generate(converted_word, false);

}

