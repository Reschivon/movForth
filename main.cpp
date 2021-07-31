
#include "headers/Interpretation/Interpreter.h"
#include "headers/Symbolic/SymbolicPass.h"

int main() {
    mfc::Interpreter interpreter;
    sym::StackGrapher stackGrapher;

    auto word_to_compile = interpreter.find("tri");

    auto converted_word = stackGrapher.compute_effects(word_to_compile);

    //stackGrapher.generate_ir(converted_word);
}

