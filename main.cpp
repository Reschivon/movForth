
#include "headers/Interpretation/Interpreter.h"
#include "headers/symbolic/symbolicPass.h"
//#include "headers/Generation/IRGenerator.h"

int main() {
    //hello_word();

    mov::Interpreter interpreter;
    mov::StackGrapher stackGrapher;

    auto word_to_compile = interpreter.find("test");

    auto converted_word = stackGrapher.compute_effects(word_to_compile);

//    auto converted_word = stackGrapher.compute_effects_flattened(word_to_compile);

    converted_word->definition_to_string();

    stackGrapher.show_word_info(converted_word);


}

