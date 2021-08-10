
#include "headers/Interpretation/Interpreter.h"
#include "headers/Symbolic/SymbolicPass.h"
//#include "headers/Generation/IRGenerator.h"

int main() {
    //hello_word();

    // Create a plain old interpreter that executes
    // the contents of the Forth file boot.fs
    mov::Interpreter interpreter("../boot.fs");

    // Extract the compiled word "test" from the interpreter's
    // dictionary. "test" has pointers to each of its definition words,
    // which also have pointers to each of their definition words
    auto word_to_compile = interpreter.find("test");

    // Create a stackGrapher object for static analysis of
    // the compiled definition
    mov::StackGrapher stackGrapher;

    // Do static analysis on "test".
    // 1. Control Analysis (compute a definition's into basic
    //    blocks and look for stack size inconsistencies between
    //    basic block edges)
    // 2. Data Flow Analysis (graph of stack data as it gets
    //    mutated by words)
    auto converted_word = stackGrapher.static_analysis(word_to_compile);

    // Show the fruits of labor from static analysis
    // This function is still incomplete
    println();
    mov::StackGrapher::show_word_info(converted_word);

}

