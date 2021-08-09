
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
    // which have pointers to each of their definition words
    auto word_to_compile = interpreter.find("test");

    // Create a stackGrapher object that we will use
    // for static analysis of compiled definition
    mov::StackGrapher stackGrapher;

    // Do static analysis on "test".
    // 1. Control Analysis (sort a definition into basic
    //    blocks and look for stack size inconsistencies between
    //    control flow edges)
    // 2. Data Flow Analysis (graph of stack data,
    //    how it is passed around, and the words that create/consume
    //    them)
    auto converted_word = stackGrapher.compute_effects(word_to_compile);

    // Show the fruits of labor from static analysis
    // This function is still incomplete
    println();
    mov::StackGrapher::show_word_info(converted_word);

}

