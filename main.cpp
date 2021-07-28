
#include "headers/Interpretation/Interpreter.h"
#include "headers/Symbolic/SymbolicPass.h"

int main() {
    mfc::Interpreter interpreter;

    auto to_compile = interpreter.find("tri");
    auto converted_word = sym::stack_analysis(to_compile);

    // converted_word->definition_to_string();

    sym::generate_ir(converted_word);
}

