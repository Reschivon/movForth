
#include "headers/Interpretation/Interpreter.h"
#include "headers/Symbolic/SymbolicPass.h"

int main() {
    mfc::Interpreter interpreter;

    auto to_compile = interpreter.find("tri");
    auto converted_word = sym::symbolize_word(to_compile);

    converted_word->definition_to_string();
}

