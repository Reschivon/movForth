
#include "../../headers/Cyclic/DictConversion.h"

#include <vector>
#include <set>
#include "../../headers/Interpretation/Word.h"

using namespace sym;

// primitives only (static instances)
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

Data mfc_data_to_cyc_data(mfc::Data data){
    if(data.is_num())
        return Data(data.as_num());
    if(data.is_xt()){}
        return Data(mfc_word_to_cyc_word(data.as_xt()));
}

Wordptr sym::mfc_word_to_cyc_word(mfc::Wordptr wordptr){
    auto converted = converted_words.find(wordptr);
    if(converted != converted_words.end()) {
        std::cout << wordptr->to_string() << " already analysed, skipping" << std::endl;
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

        std::cout << "\nanalyse " << new_word->name << std::endl;

        auto definition = forth_word->get_definition();
        for (int index = 0; index < definition.size(); index++)
        {
            sym::Word *current_definee = mfc_word_to_cyc_word(definition[index].as_xt());

            new_word->definition.push_back(current_definee);

            if(is_stateful(current_definee))
            {
                // treat next word as data and add data to definee
                auto next_thing = mfc_data_to_cyc_data(definition[++index]);
                current_definee->data_push.push_back(next_thing);
            }
            new_word->acquire_side_effects(current_definee);
        }
        converted_words[wordptr] = new_word;

        // now the stack analysis pass
        new_word->first_stack = new Stack;
        Stack* next_stack = new_word->first_stack;
        for (auto thing : new_word->definition)
        {
            Stack* curr_stack = next_stack;
            next_stack = curr_stack->propagate(new_word, thing);
        }
        new_word->last_stack = next_stack;

        new_word->stack_push = new_word->last_stack->data.size();

        std::cout << "finished " << new_word->name << ": +" << new_word->stack_push << " -" << new_word->stack_pop << "\n\n";


        return new_word;
    }

    std::cout << "fuck" << std::endl;
    return nullptr;
}