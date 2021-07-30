
#include <vector>
#include <set>
#include "../../headers/Symbolic/SymbolicPass.h"
#include "../../headers/Interpretation/Word.h"
#include "../../headers/Print.h"

using namespace sym;

// primitives (static instances)
const std::unordered_map<std::string, Wordptr> primitive_lookup = {
        {"+",        new Word{.name = "+",         .num_popped = 2, .num_pushed = 1}},
        {"-",        new Word{.name = "-",         .num_popped = 2, .num_pushed = 1}},
        {"*",        new Word{.name = "*",         .num_popped = 2, .num_pushed = 1}},
        {"/",        new Word{.name = "/",         .num_popped = 2, .num_pushed = 1}},
        {"swap",     new Word{.name = "swap",      .num_popped = 2, .num_pushed = 2, .register_passthrough = Word::swap_node_passthrough}},
        {"dup",      new Word{.name = "dup",       .num_popped = 1, .num_pushed = 2, .register_passthrough = Word::dup_node_passthrough}},
        {"drop",     new Word{.name = "drop",      .num_popped = 1}},
        {".",        new Word{.name = ".",         .num_popped = 1}},
        {".S",       new Word{.name = ".S"}},
        {"'",        new Word{.name = "'",         .consume_token = true, .data_push = std::vector<Data>{Data(nullptr)}}},
        {",",        new Word{.name = ",",         .num_popped = 1, .compiled_slots = 1}},
        {"see",      new Word{.name = "see"}},
        {"[",        new Word{.name = "[",         .interpret_state = sym::Word::TOINTERPRET}},
        {"]",        new Word{.name = "]",         .interpret_state = sym::Word::TOCOMPILE}},
        {"immediate",new Word{.name = "immediate"}}, // very rare this ends up in compile code, consider error on encounter
        {"@",        new Word{.name = "@",         .num_popped = 1, .num_pushed = 1}},
        {"!",        new Word{.name = "!",         .num_popped = 2, }}, // TODO more memory stuff
        {"branch",   new Word{.name = "branch"}},
        {"branchif", new Word{.name = "branchif",  .num_popped = 1}},
        {"literal",  new Word{.name = "literal",   .num_pushed = 1}},
        {"here",     new Word{.name = "here",      .num_pushed = 1}}, // TODO this is a special case
        {"create",   new Word{.name = "create",    .consume_token = true, .define_new_word = true}}
};

bool is_stateful(Wordptr wordptr){
    if(
            wordptr->name == "literal"||
            wordptr->name == "branch" ||
            wordptr->name == "branchif")
        return true;
    return false;
}

// cached results of stack_analysis
std::unordered_map<mfc::Wordptr, Wordptr> converted_words;


Data symbolize_data(mfc::Data data){
    if(data.is_num())
        return Data(data.as_num());
    if(data.is_xt()){}
        return Data(stack_analysis(data.as_xt()));
}

Wordptr sym::stack_analysis(mfc::Wordptr wordptr){
    // check to see if we have passed over this word already
    // if so, return a pointer to it
    auto converted = converted_words.find(wordptr);
    if(converted != converted_words.end()) {
        println(wordptr->to_string(), " already analysed, skipping");
        return converted->second;
    }

    auto name = wordptr->base_string();

    if(dynamic_cast<mfc::Primitive*>(wordptr)){
        // is a primitive: simply return the singleton of the primitive
        // with hard-coded effects
        Wordptr word_singleton = primitive_lookup.at(name);
        return word_singleton;

    }else if(auto forth_word = dynamic_cast<mfc::ForthWord*>(wordptr)){
        // is a forth word: pass over its definition to compute side effects
        auto *new_word = new Word{.name = name};

        // initialize the symbolic stack's starting state
        // we will update its state as we pass over xts
        auto* next_stack = new Stack;
        RegisterGenerator register_generator;
        for (int i = 0; i < forth_word->get_definition().size(); i++)
        {
            mfc::Data current_data = forth_word->get_definition()[i];

            // assume the current xt is a word
            auto *current_definee = shallow_copy(stack_analysis(current_data.as_xt()));
            new_word->definition.push_back(current_definee);

            // collapse nodes literals into the word that owns them
            if(is_stateful(current_definee))
            {
                // treat next word as nodes and add nodes to definee
                auto next_thing = symbolize_data(current_data);
                current_definee->data_push.push_back(next_thing);
                i++;
            }

            // update the word's total effects
            new_word->acquire_side_effects(current_definee);

            // iterate the stack state
            Stack* curr_stack = next_stack;
            new_word->stacks.push_back(curr_stack);
            next_stack = curr_stack->propagate(new_word, current_definee, register_generator);
        }
        // the pushed side effect is the same as remaining stack frames
        auto *last_stack = next_stack;
        new_word->stacks.push_back(last_stack);
        new_word->num_pushed = last_stack->nodes.size();

        // cache this word for the future
        converted_words[wordptr] = new_word;

        return new_word;
    }

    println("fuck");
    return nullptr;
}


Wordptr sym::generate_ir(Wordptr wordptr){
    for(int i = 0; i < wordptr->definition.size(); i++){
        Stack *stack = wordptr->stacks[i];
        Wordptr sub_def = wordptr->definition[i];

        /*println();
        println("[stack]");
        println("pop from ids:");
        for(auto node : stack->nodes)
            println("   ", node->backward_id.to_string());

        println("push to ids:");
        for(auto node : stack->nodes)
            println("   ", node->forward_id.to_string());*/

        println();
        println("[", sub_def->name, "]");
        println("pop from ids:");
        for(auto node : sub_def->pop_nodes)
            println("   ", node->backward_id.to_string());

        println("push to ids:");
        for(auto node : sub_def->push_nodes)
            println("   ", node->forward_id.to_string());
    }

    return wordptr;
}
