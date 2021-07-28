
#include "headers/Interpretation/Interpreter.h"
#include "headers/Cyclic/DictConversion.h"
#include "headers/Cyclic/Translation.h"

int main() {
    mfc::Interpreter interpreter;

    auto to_compile = interpreter.find("tri");
    auto converted_word = sym::mfc_word_to_cyc_word(to_compile);

    converted_word->definition_to_string();
}

