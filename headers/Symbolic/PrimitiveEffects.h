

#ifndef MOVFORTH_PRIMITIVEEFFECTS_H
#define MOVFORTH_PRIMITIVEEFFECTS_H

namespace mov{

    static const std::vector<std::pair<int, int>> swap_oi_pairs = {{1, 0}, {0, 1}};
    static const std::vector<std::pair<int, int>> rot_oi_pairs = {{2, 0}, {1, 2}, {0, 1}};
    static const std::vector<std::pair<int, int>> dup_oi_pairs = {{1, 0}, {0, 0}};

    // primitives (static instances)
    static const std::unordered_map<std::string, sWordptr> primitive_lookup = {
            {"+",         new sWord("+",         {.num_popped = 2, .num_pushed = 1})},
            {"-",         new sWord("-",         {.num_popped = 2, .num_pushed = 1})},
            {"*",         new sWord("*",         {.num_popped = 2, .num_pushed = 1})},
            {"/",         new sWord("/",         {.num_popped = 2, .num_pushed = 1})},
            {"swap",      new sWord("swap",      Effects{.num_popped = 2, .num_pushed = 2, .out_in_pairs = swap_oi_pairs})},
            {"rot",       new sWord("rot",       Effects{.num_popped = 3, .num_pushed = 3, .out_in_pairs = rot_oi_pairs})},
            {"dup",       new sWord("dup",       Effects{.num_popped = 1, .num_pushed = 2, .out_in_pairs = dup_oi_pairs})},
            {"drop",      new sWord("drop",      {.num_popped = 1})},
            {".",         new sWord(".",         {.num_popped = 1})},
            {".S",        new sWord(".S",        Effects::neutral)},
            {"'",         new sWord("'",         {.consume_token = true,})},
            {",",         new sWord(",",         {.num_popped = 1,  .compiled_slots = 1})},
            {"see",       new sWord("see",       Effects::neutral)},
            {"[",         new sWord("[",         {.interpret_state = Effects::TOINTERPRET})},
            {"]",         new sWord("]",         {.interpret_state = Effects::TOCOMPILE})},
            {"immediate", new sWord("immediate", Effects::neutral)}, // very rare this ends up in compiled code, consider warn on encounter
            {"@",         new sWord("@",         {.num_popped = 1, .num_pushed = 1})},
            {"!",         new sWord("!",         {.num_popped = 2,})},
            {"branch",    new sWord("branch",    Effects::neutral)},
            {"branchif",  new sWord("branchif",  {.num_popped = 1})},
            {"literal",   new sWord("literal",   {.num_pushed = 1})},
            {"here",      new sWord("here",      {.num_pushed = 1})},
            {"create",    new sWord("create",    {.consume_token = true, .define_new_word = true})}
    };
}

#endif //MOVFORTH_PRIMITIVEEFFECTS_H
