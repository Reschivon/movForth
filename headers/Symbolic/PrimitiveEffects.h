

#ifndef MOVFORTH_PRIMITIVEEFFECTS_H
#define MOVFORTH_PRIMITIVEEFFECTS_H

#include "PrimitiveEnums.h"
#include "sWord.h"

namespace mov{
    static const std::vector<std::pair<int, int>> swap_oi_pairs = {{1, 0}, {0, 1}};
    static const std::vector<std::pair<int, int>> rot_oi_pairs = {{2, 0}, {1, 2}, {0, 1}};
    static const std::vector<std::pair<int, int>> dup_oi_pairs = {{1, 0}, {0, 0}};

    // primitives (static instances)
    static const std::unordered_map<std::string, sWordptr> primitive_lookup = {
            {"+",         new sWord("+",         primitive_words::ADD,          {.num_popped = 2, .num_pushed = 1})},
            {"-",         new sWord("-",         primitive_words::SUBTRACT,     {.num_popped = 2, .num_pushed = 1})},
            {"*",         new sWord("*",         primitive_words::MULTIPLY,     {.num_popped = 2, .num_pushed = 1})},
            {"/",         new sWord("/",         primitive_words::DIVIDE,       {.num_popped = 2, .num_pushed = 1})},
            {"swap",      new sWord("swap",      primitive_words::SWAP,         Effects{.num_popped = 2, .num_pushed = 2, .out_in_pairs = swap_oi_pairs})},
            {"rot",       new sWord("rot",       primitive_words::ROT,          Effects{.num_popped = 3, .num_pushed = 3, .out_in_pairs = rot_oi_pairs})},
            {"dup",       new sWord("dup",       primitive_words::DUP,          Effects{.num_popped = 1, .num_pushed = 2, .out_in_pairs = dup_oi_pairs})},
            {"drop",      new sWord("drop",      primitive_words::DROP,         {.num_popped = 1})},
            {".",         new sWord(".",         primitive_words::EMIT,         {.num_popped = 1})},
            {".S",        new sWord(".S",        primitive_words::SHOW,         Effects::neutral)},
            {"'",         new sWord("'",         primitive_words::TICK,         {.consume_token = true,})},
            {",",         new sWord(",",         primitive_words::COMMA,        {.num_popped = 1,  .compiled_slots = 1})},
            {"see",       new sWord("see",       primitive_words::SEE,          Effects::neutral)},
            {"[",         new sWord("[",         primitive_words::TOIMMEDIATE,  {.interpret_state = Effects::TOINTERPRET})},
            {"]",         new sWord("]",         primitive_words::TOCOMPILE,    {.interpret_state = Effects::TOCOMPILE})},
            {"immediate", new sWord("immediate", primitive_words::IMMEDIATE,    Effects::neutral)}, // very rare this ends up in compiled code, consider warn on encounter
            {"@",         new sWord("@",         primitive_words::FETCH,        {.num_popped = 1, .num_pushed = 1})},
            {"!",         new sWord("!",         primitive_words::STORE,         {.num_popped = 2,})},
            {"branch",    new sWord("branch",    primitive_words::BRANCH,       Effects::neutral)},
            {"branchif",  new sWord("branchif",  primitive_words::BRANCHIF,     {.num_popped = 1})},
            {"literal",   new sWord("literal",   primitive_words::LITERAL,      {.num_pushed = 1})},
            {"here",      new sWord("here",      primitive_words::HERE,         {.num_pushed = 1})},
            {"create",    new sWord("create",    primitive_words::CREATE,       {.consume_token = true, .define_new_word = true})}
    };
}

#endif //MOVFORTH_PRIMITIVEEFFECTS_H
