
#include <vector>
#include <set>
#include "../../headers/Symbolic/SymbolicPass.h"
#include "../../headers/Interpretation/Word.h"
#include "../../headers/Symbolic/Structures.h"
#include "../../headers/Print.h"

using namespace sym;

// primitives (static instances)
std::unordered_map<std::string, Wordptr> primitive_lookup = {
        {"+",        new Word{.name = "+",         .stack_pop = 2, .stack_push = 1}},
        {"-",        new Word{.name = "-",         .stack_pop = 2, .stack_push = 1}},
        {"*",        new Word{.name = "*",         .stack_pop = 2, .stack_push = 1}},
        {"/",        new Word{.name = "/",         .stack_pop = 2, .stack_push = 1}},
        {"swap",     new Word{.name = "swap"}},
        {"dup",      new Word{.name = "dup"}},
        {"drop",     new Word{.name = "drop"}},
        {".",        new Word{.name = ".",         .stack_pop = 1}},
        {".S",       new Word{.name = ".S"}},
        {"'",        new Word{.name = "'",         .consume_token = true, .data_push = std::vector<Data>{Data(nullptr)}}},
        {",",        new Word{.name = ",",         .stack_pop = 1, .compiled_slots = 1}},
        {"see",      new Word{.name = "see"}},
        {"[",        new Word{.name = "[",         .interpret_state = sym::Word::TOINTERPRET}},
        {"]",        new Word{.name = "]",         .interpret_state = sym::Word::TOCOMPILE}},
        {"immediate",new Word{.name = "immediate"}}, // very rare this ends up in compile code, consider error on encounter
        {"@",        new Word{.name = "@",         .stack_pop = 1, .stack_push = 1}},
        {"!",        new Word{.name = "!",         .stack_pop = 2, }}, // TODO more memory stuff
        {"branch",   new Word{.name = "branch"}},
        {"branchif", new Word{.name = "branchif",  .stack_pop = 1}},
        {"literal",  new Word{.name = "literal",   .stack_push = 1}},
        {"here",     new Word{.name = "here",      .stack_push = 1}}, // TODO this is a special case
        {"create",   new Word{.name = "create",    .consume_token = true, .new_word = true}}
};

bool is_stateful(Wordptr wordptr){
    if(
            wordptr->name == "literal"||
            wordptr->name == "branch" ||
            wordptr->name == "branchif")
        return true;
    return false;
}

// for multiple iterations of compile, which may
// use the same sub definitions
std::unordered_map<mfc::Wordptr, Wordptr> converted_words;

Data symbolize_data(mfc::Data data){
    if(data.is_num())
        return Data(data.as_num());
    if(data.is_xt()){}
        return Data(symbolize_word(data.as_xt()));
}

Wordptr sym::symbolize_word(mfc::Wordptr wordptr){
    auto converted = converted_words.find(wordptr);
    if(converted != converted_words.end()) {
        dln(wordptr->to_string(), " already analysed, skipping");
        return converted->second;
    }

    auto name = wordptr->base_string();

    if(auto primitive = dynamic_cast<mfc::Primitive*>(wordptr)){
        // is a primitive
        Wordptr word_singleton = primitive_lookup[name];
        return word_singleton;

    }else if(auto forth_word = dynamic_cast<mfc::ForthWord*>(wordptr)){
        // is a forth word
        auto *new_word = new Word{.name = name};

        dln("\nanalyse ", new_word->name);

        converted_words[wordptr] = new_word;

        new_word->first_stack = new Stack;
        Stack* next_stack = new_word->first_stack;
        for (int i = 0; i < forth_word->get_definition().size(); i++)
        {
            mfc::Data definition_slot = forth_word->get_definition()[i];

            sym::Word *current_definee = symbolize_word(definition_slot.as_xt());
            new_word->definition.push_back(current_definee);

            if(is_stateful(current_definee))
            {
                // treat next word as data and add data to definee
                auto next_thing = symbolize_data(definition_slot);
                current_definee->data_push.push_back(next_thing);
                i++;
            }

            new_word->acquire_side_effects(current_definee);

            Stack* curr_stack = next_stack;
            next_stack = curr_stack->propagate(new_word, current_definee);
        }
        new_word->last_stack = next_stack;

        new_word->stack_push = new_word->last_stack->data.size();

        dln("finished ", new_word->name, ": +", new_word->stack_push, " -", new_word->stack_pop, "\n");


        return new_word;
    }

    println("fuck");
    return nullptr;
}