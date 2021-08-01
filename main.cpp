
#include "headers/Interpretation/Interpreter.h"
#include "headers/Symbolic/SymbolicPass.h"

int main() {
    mfc::Interpreter interpreter;
    sym::StackGrapher stackGrapher;

    auto word_to_compile = interpreter.find("test");

    auto converted_word = stackGrapher.compute_effects(word_to_compile);

//    auto converted_word = stackGrapher.compute_effects_flattened(word_to_compile);

    converted_word->definition_to_string();

    stackGrapher.generate_ir(converted_word);
}

