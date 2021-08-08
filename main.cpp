
#include "headers/Interpretation/Interpreter.h"
#include "headers/Symbolic/SymbolicPass.h"
//#include "headers/Generation/IRGenerator.h"

int main() {
    //hello_word();

    mov::Interpreter interpreter("../boot.fs");
    mov::StackGrapher stackGrapher;

    auto word_to_compile = interpreter.find("test");

    auto converted_word = stackGrapher.compute_effects(word_to_compile);

    println();
    mov::StackGrapher::show_word_info(converted_word);

}

