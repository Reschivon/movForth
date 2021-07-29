
#include "headers/Interpretation/Interpreter.h"
#include "headers/Symbolic/SymbolicPass.h"

int main() {
    mfc::Interpreter interpreter;

    auto word_to_compile = interpreter.find("tri");

    auto converted_word = sym::stack_analysis(word_to_compile);

    sym::generate_ir(converted_word);
}

