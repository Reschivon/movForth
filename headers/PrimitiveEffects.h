

#ifndef MOVFORTH_PRIMITIVEEFFECTS_H
#define MOVFORTH_PRIMITIVEEFFECTS_H

#include "PrimitiveEnums.h"
#include "Symbolic/sWord.h"

namespace mov{
    static const std::vector<std::pair<int, int>> swap_oi_pairs = {{1, 0}, {0, 1}};
    static const std::vector<std::pair<int, int>> rot_oi_pairs = {{2, 0}, {1, 2}, {0, 1}};
    static const std::vector<std::pair<int, int>> dup_oi_pairs = {{1, 0}, {0, 0}};

    // primitives (static instances)
    static const std::unordered_map<primitive_words, sWordptr> primitive_lookup = {
        {primitive_words::EQUALS,        new sWord("=",         primitive_words::EQUALS,       Effects{.num_popped = 2, .num_pushed = 1})},
        {primitive_words::ADD,           new sWord("+",         primitive_words::ADD,          Effects{.num_popped = 2, .num_pushed = 1})},
        {primitive_words::SUBTRACT,      new sWord("-",         primitive_words::SUBTRACT,     {.num_popped = 2, .num_pushed = 1})},
        {primitive_words::MULTIPLY,      new sWord("*",         primitive_words::MULTIPLY,     {.num_popped = 2, .num_pushed = 1})},
        {primitive_words::DIVIDE,        new sWord("/",         primitive_words::DIVIDE,       {.num_popped = 2, .num_pushed = 1})},
        {primitive_words::SWAP,          new sWord("swap",      primitive_words::SWAP,         Effects{.num_popped = 2, .num_pushed = 2, .consume_token=false, .compiled_slots=0, .interpret_state=Effects::interpret_state::NONE, .define_new_word=false, .out_in_pairs = swap_oi_pairs})},
        {primitive_words::ROT,           new sWord("rot",       primitive_words::ROT,          Effects{.num_popped = 3, .num_pushed = 3, .consume_token=false, .compiled_slots=0, .interpret_state=Effects::interpret_state::NONE, .define_new_word=false, .out_in_pairs = rot_oi_pairs})},
        {primitive_words::DUP,           new sWord("dup",       primitive_words::DUP,          Effects{.num_popped = 1, .num_pushed = 2, .consume_token=false, .compiled_slots=0, .interpret_state=Effects::interpret_state::NONE, .define_new_word=false, .out_in_pairs = dup_oi_pairs})},
        {primitive_words::DROP,          new sWord("drop",      primitive_words::DROP,         {.num_popped = 1})},
        {primitive_words::EMIT,          new sWord(".",         primitive_words::EMIT,         {.num_popped = 1})},
        {primitive_words::SHOW,          new sWord(".S",        primitive_words::SHOW,         Effects::neutral)},
        {primitive_words::TICK,          new sWord("'",         primitive_words::TICK,         {.num_popped = 0, .num_pushed = 0, .consume_token = true})},
        {primitive_words::COMMA,         new sWord(",",         primitive_words::COMMA,        {.num_popped = 1,  .num_pushed=0, .consume_token=false, .compiled_slots = 1})},
        {primitive_words::SEE,           new sWord("see",       primitive_words::SEE,          Effects::neutral)},
        {primitive_words::TOIMMEDIATE,   new sWord("[",         primitive_words::TOIMMEDIATE,  {.num_popped = 0, .num_pushed = 0, .consume_token=false, .compiled_slots=0, .interpret_state = Effects::interpret_state::TOIMM})},
        {primitive_words::TOCOMPILE,     new sWord("]",         primitive_words::TOCOMPILE,    {.num_popped = 0, .num_pushed = 0, .consume_token=false, .compiled_slots=0, .interpret_state = Effects::interpret_state::TOCOMP})},
        {primitive_words::IMMEDIATE,     new sWord("immediate", primitive_words::IMMEDIATE,    Effects::neutral)}, // very rare this ends up in compiled code, consider warn on encounter
        {primitive_words::ALLOT,         new sWord("allot",     primitive_words::ALLOT,        {.num_popped = 1})},
        {primitive_words::FETCH,         new sWord("@",         primitive_words::FETCH,        {.num_popped = 1, .num_pushed = 1})},
        {primitive_words::STORE,         new sWord("!",         primitive_words::STORE,        {.num_popped = 2,})},
        {primitive_words::BRANCH,        new sWord("branch",    primitive_words::BRANCH,       Effects::neutral)},
        {primitive_words::BRANCHIF,      new sWord("branchif",  primitive_words::BRANCHIF,     {.num_popped = 1})},
        {primitive_words::LITERAL,       new sWord("literal",   primitive_words::LITERAL,      {.num_popped=0, .num_pushed = 1})},
        {primitive_words::HERE,          new sWord("here",      primitive_words::HERE,         {.num_popped=0, .num_pushed = 1})},
        {primitive_words::CREATE,        new sWord("create",    primitive_words::CREATE,       {.num_popped = 0, .num_pushed = 0, .consume_token = true, .compiled_slots=0, .interpret_state=Effects::interpret_state::NONE, .define_new_word = true})}
    };
}

#endif //MOVFORTH_PRIMITIVEEFFECTS_H
