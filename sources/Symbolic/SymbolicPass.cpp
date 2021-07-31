
#include <vector>
#include <set>
#include "../../headers/Symbolic/SymbolicPass.h"
#include "../../headers/Interpretation/Word.h"
#include "../../headers/Print.h"
#include "../../headers/Symbolic/Word.h"

using namespace sym;

static const std::vector<std::pair<int,int>> swap_oi_pairs = {{1, 0}, {0,1}};
static const std::vector<std::pair<int,int>> rot_oi_pairs = {{2,0}, {1,2}, {0,1}};
static const std::vector<std::pair<int,int>> dup_oi_pairs = {{1,0}, {0,0}};

// primitives (static instances)
const std::unordered_map<std::string, Wordptr> primitive_lookup = {
        {"+",        new Word{.name = "+",         .effects = {.num_popped = 2, .num_pushed = 1}}},
        {"-",        new Word{.name = "-",         .effects = {.num_popped = 2, .num_pushed = 1}}},
        {"*",        new Word{.name = "*",         .effects = {.num_popped = 2, .num_pushed = 1}}},
        {"/",        new Word{.name = "/",         .effects = {.num_popped = 2, .num_pushed = 1}}},
        {"swap",     new Word{.name = "swap",      .effects = {.num_popped = 2, .num_pushed = 2,
                                                               .output_input_pairs = swap_oi_pairs}}},
       {"rot",     new Word{.name = "rot",         .effects = {.num_popped = 3, .num_pushed = 3,
                                                               .output_input_pairs = rot_oi_pairs}}},
        {"dup",      new Word{.name = "dup",       .effects = {.num_popped = 1, .num_pushed = 2,
                                                               .output_input_pairs = dup_oi_pairs}}},
        {"drop",     new Word{.name = "drop",      .effects = {.num_popped = 1}}},
        {".",        new Word{.name = ".",         .effects = {.num_popped = 1}}},
        {".S",       new Word{.name = ".S"}},
        {"'",        new Word{.name = "'",         .effects = {.consume_token = true, .data_push = std::vector<Data>{Data(nullptr)}}}},
        {",",        new Word{.name = ",",         .effects = {.num_popped = 1, .compiled_slots = 1}}},
        {"see",      new Word{.name = "see"}},
        {"[",        new Word{.name = "[",         .effects = {.interpret_state = sym::Effects::TOINTERPRET}}},
        {"]",        new Word{.name = "]",         .effects = {.interpret_state = sym::Effects::TOCOMPILE}}},
        {"immediate",new Word{.name = "immediate"}}, // very rare this ends up in compiled code, consider error on encounter
        {"@",        new Word{.name = "@",         .effects = {.num_popped = 1, .num_pushed = 1}}},
        {"!",        new Word{.name = "!",         .effects = {.num_popped = 2, }}}, // TODO more memory stuff
        {"branch",   new Word{.name = "branch"}},
        {"branchif", new Word{.name = "branchif",  .effects = {.num_popped = 1}}},
        {"literal",  new Word{.name = "literal",   .effects = {.num_pushed = 1}}},
        {"here",     new Word{.name = "here",      .effects = {.num_pushed = 1}}}, // TODO this is a special case
        {"create",   new Word{.name = "create",    .effects = {.consume_token = true, .define_new_word = true}}}
};

bool is_stateful(Wordptr wordptr){
    if(
            wordptr->name == "literal"||
            wordptr->name == "branch" ||
            wordptr->name == "branchif")
        return true;
    return false;
}


Data StackGrapher::symbolize_data(mfc::Data data){
    if(data.is_num())
        return Data(data.as_num());
    if(data.is_xt()){}
        return Data(compute_effects(data.as_xt()));
}

Wordptr StackGrapher::compute_effects(mfc::Wordptr original_word){
    // check to see if we have passed over this word already
    // if so, return a pointer to it
    auto cached = visited_words.find(original_word);
    if(cached != visited_words.end()) {
        println(original_word->to_string(), " already analysed, skipping");
        return shallow_copy(cached->second);
    }

    if(dynamic_cast<mfc::Primitive*>(original_word)){
        // is a primitive: return the singleton of the primitive
        Wordptr word_singleton = primitive_lookup.at(original_word->base_string());
        return word_singleton;

    }else if(auto forth_word = dynamic_cast<mfc::ForthWord*>(original_word)){
        dln();
        dln("compute effects for [", original_word->base_string(), "]");
        indent();


        auto converted = conversion_pass(forth_word);

        graph_pass(converted);

        retrieve_push_pop_effects(converted);


        unindent();

        return converted;
    }

    dln("word is null");

    return nullptr;
}

Wordptr StackGrapher::conversion_pass(mfc::ForthWord *original_word){
    auto *new_word = new Word{.name = original_word->base_string()};

    for (int i = 0; i < original_word->get_definition().size(); i++)
    {
        mfc::Data current_data = original_word->get_definition()[i];

        // assume the current xt is a word
        // (all data cells should have been integrated in the previous loop)
        auto *current_definee = compute_effects(current_data.as_xt());
        new_word->definition.push_back(current_definee);

        // collapse nodes literals into the word that owns them
        if(is_stateful(current_definee))
        {
            // treat next word as nodes and add nodes to definee
            auto next_thing = symbolize_data(current_data);
            current_definee->effects.data_push.push_back(next_thing);
            i++;
        }

        // cache this word for the future
        visited_words[original_word] = new_word;
    }

    return new_word;
}

void StackGrapher::graph_pass(Wordptr word){
    // the stack is a constantly updated list of
    // loose pop nodes that the next word might need
    auto &running_stack = *(new NodeList);
    RegisterGenerator register_generator;

    for (auto definee : word->definition)
    {
        // update the word's total Effects
        word->effects.acquire_side_effects(definee->effects);

        // iterate the stack state
        word->propagate(definee, running_stack, register_generator);

        dln();
        dln("intermediate stack: ");
        for(auto thing : running_stack){
            dln("\t", thing->edge_register.to_string());
        }
    }

    // the pushed side effect is the same as remaining stack frames
    word->effects.num_pushed = running_stack.size();
    word->my_graphs_outputs = running_stack;
}

void StackGrapher::retrieve_push_pop_effects(Wordptr word) {
    // matching pairs
    for(int i = 0 ; i < word->my_graphs_outputs.size(); i++){
        Node *output = word->my_graphs_outputs[i];

        Node *next = output->target;
        while(true){
            if(next->target == nullptr)
                break;
            if(next->edge_register.register_type == Register::PARAM){
                word->effects.output_input_pairs.emplace_back(i, next->edge_register.ID);
                break;
            }
            next = next->target;
        }
    }

    // push pop effects
    dln("pops: ", word->my_graphs_inputs.size(), " pushes: ", word->my_graphs_outputs.size());
    word->effects.num_pushed = word->my_graphs_outputs.size();
    word->effects.num_popped = word->my_graphs_inputs.size();
}

Wordptr StackGrapher::generate_ir(Wordptr wordptr){
    println("============[", wordptr->name, "]===========");
    for(auto sub_def : wordptr->definition){
        /*println();
        println("[stack]");
        println("pop from ids:");
        for(auto node : stack->nodes)
            println("   ", node->edge_register.to_string());

        println("push to ids:");
        for(auto node : stack->nodes)
            println("   ", node->forward_id.to_string());*/

        println();
        println("[", sub_def->name, "]");
        println("pop from ids:");
        for(auto node : sub_def->pop_nodes)
            println("   ", node->edge_register.to_string());

        println("push to ids:");
        for(auto node : sub_def->push_nodes)
            println("   ", node->forward_edge_register.to_string());
    }

    return wordptr;
}

