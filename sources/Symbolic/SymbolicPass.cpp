
#include <vector>
#include <set>
#include <stack>
#include "../../headers/Symbolic/SymbolicPass.h"
#include "../../headers/Print.h"
#include "../../headers/Symbolic/Word.h"

using namespace sym;

static const std::vector<std::pair<int, int>> swap_oi_pairs = {{1, 0},
                                                               {0, 1}};
static const std::vector<std::pair<int, int>> rot_oi_pairs = {{2, 0},
                                                              {1, 2},
                                                              {0, 1}};
static const std::vector<std::pair<int, int>> dup_oi_pairs = {{1, 0},
                                                              {0, 0}};

// primitives (static instances)
const std::unordered_map<std::string, Wordptr> primitive_lookup = {
        {"+",         new Word{.name = "+",         .effects = {.num_popped = 2, .num_pushed = 1}}},
        {"-",         new Word{.name = "-",         .effects = {.num_popped = 2, .num_pushed = 1}}},
        {"*",         new Word{.name = "*",         .effects = {.num_popped = 2, .num_pushed = 1}}},
        {"/",         new Word{.name = "/",         .effects = {.num_popped = 2, .num_pushed = 1}}},
        {"swap",      new Word{.name = "swap",      .effects = {.num_popped = 2, .num_pushed = 2, .out_in_pairs = swap_oi_pairs}}},
        {"rot",       new Word{.name = "rot",       .effects = {.num_popped = 3, .num_pushed = 3, .out_in_pairs = rot_oi_pairs}}},
        {"dup",       new Word{.name = "dup",       .effects = {.num_popped = 1, .num_pushed = 2, .out_in_pairs = dup_oi_pairs}}},
        {"drop",      new Word{.name = "drop",      .effects = {.num_popped = 1}}},
        {".",         new Word{.name = ".",         .effects = {.num_popped = 1}}},
        {".S",        new Word{.name = ".S"}},
        {"'",         new Word{.name = "'",         .effects = {.consume_token = true,}}},
        {",",         new Word{.name = ",",         .effects = {.num_popped = 1,       .compiled_slots = 1}}},
        {"see",       new Word{.name = "see"}},
        {"[",         new Word{.name = "[",         .effects = {.interpret_state = sym::Effects::TOINTERPRET}}},
        {"]",         new Word{.name = "]",         .effects = {.interpret_state = sym::Effects::TOCOMPILE}}},
        {"immediate", new Word{.name = "immediate"}}, // very rare this ends up in compiled code, consider warn on encounter
        {"@",         new Word{.name = "@",         .effects = {.num_popped = 1, .num_pushed = 1}}},
        {"!",         new Word{.name = "!",         .effects = {.num_popped = 2,}}},
        {"branch",    new Word{.name = "branch"}},
        {"branchif",  new Word{.name = "branchif",  .effects = {.num_popped = 1}}},
        {"literal",   new Word{.name = "literal",   .effects = {.num_pushed = 1}}},
        {"here",      new Word{.name = "here",      .effects = {.num_pushed = 1}}},
        {"create",    new Word{.name = "create",    .effects = {.consume_token = true, .define_new_word = true}}}
};

bool is_stateful(std::string name) {
    return (name == "literal" || name == "branch" || name == "branchif");
}


Data StackGrapher::symbolize_data(mfc::Data data) {
    if (data.is_num())
        return Data(data.as_num());
    if (data.is_xt())
        return Data(compute_effects(data.as_xt()));

    dln("FUCK");
    return Data(nullptr);
}

Wordptr StackGrapher::compute_effects(mfc::Wordptr original_word) {
    // check to see if we have passed over this word already
    // if so, return a pointer to it
    auto cached = visited_words.find(original_word);
    if (cached != visited_words.end())
    {
        println(original_word->to_string(), " already analysed, skipping");
        return cached->second;
    }

    if (dynamic_cast<mfc::Primitive *>(original_word))
    {
        // is a primitive: return the singleton of the primitive
        Wordptr word_singleton = primitive_lookup.at(
                original_word->base_string());
        return word_singleton;

    } else if (auto forth_word = dynamic_cast<mfc::ForthWord *>(original_word))
    {
        dln();
        dln("compute [", original_word->base_string(), "]");
        indent();

        auto converted = conversion_pass(forth_word);
        graph_pass(converted);
        retrieve_push_pop_effects(converted);
        branching_pass(converted);

        unindent();
        dln("finished compute [", original_word->base_string(), "]");

        return converted;
    }

    dln("word is null");

    return nullptr;
}

Wordptr StackGrapher::compute_effects_flattened(mfc::Wordptr input) {
    auto *big_bertha = new mfc::ForthWord(input->base_string(), false);

    std::stack<mfc::Wordptr> to_add;
    to_add.push(input);

    while (!to_add.empty())
    {
        auto current = to_add.top();
        to_add.pop();

        auto fw_maybe = dynamic_cast<mfc::ForthWord *>(current);
        if (fw_maybe != nullptr)
        {
            auto def = fw_maybe->get_definition();
            for (auto dat = def.rbegin(); dat != def.rend(); dat++)
            {
                if (dat->is_xt())
                    to_add.push(dat->as_xt());
            }
        } else
        {
            // is primitive
            big_bertha->add(mfc::Data(current));
        }
    }

    dln(input->base_string(), " flattened. Definition:");
    big_bertha->definition_to_string();

    auto converted = compute_effects(big_bertha);

    return converted;
}


Wordptr StackGrapher::conversion_pass(mfc::ForthWord *original_word) {

    auto *new_word = new Word{.name = original_word->base_string()};

    for (int i = 0; i < original_word->get_definition().size(); i++)
    {
        mfc::Data current_data = original_word->get_definition()[i];

        // assume the current xt is a word
        // (all data cells should have been integrated in the previous loop)
        auto *current_definee = compute_effects(current_data.as_xt());
        if(current_definee->name == "branch")
            new_word->instructions.push_back(new BranchInstruction(current_definee));
        else if(current_definee->name == "branchif")
            new_word->instructions.push_back(new BranchIfInstruction(current_definee));
        else
            new_word->instructions.push_back(new Instruction(current_definee));

        // collapse nodes literals into the word that owns them
        if (is_stateful(current_definee->name))
        {
            i++;
            // add next cell to current Instruction
            auto data = symbolize_data(original_word->get_definition()[i]);
            new_word->instructions.back()->data = data;

            // unfortunate, but we must keep the offset of branch the same
            new_word->instructions.push_back(new Instruction(Word::nop));
        }

        // cache this word for the future
        visited_words[original_word] = new_word;
    }

    return new_word;
}

void propagate_stack(NodeList &stack, Instruction *instruction, Wordptr base,
                     RegisterGenerator &register_generator) {
    auto effects = instruction->linked_word->effects;

    dln("pops: ", effects.num_popped, " pushes: ", effects.num_pushed);

    // add necessary input nodes
    unsigned int nodes_from_input = 0;
    unsigned int nodes_from_stack = effects.num_popped;
    if (effects.num_popped > stack.size())
    {
        nodes_from_input = effects.num_popped - stack.size();
        nodes_from_stack = stack.size();
    }
    while (nodes_from_input-- > 0)
    {
        Register input_register = register_generator.get_param();
        auto input_node = new Node;
        instruction->pop_nodes.push_back(new Node{
                .target = input_node,
                .edge_register = input_register});
        base->my_graphs_inputs.push_front(input_node);
        dln("needs extra input ", input_register.to_string());
    }

    // pop input nodes from stack
    NodeList::move_top_elements(stack, instruction->pop_nodes, nodes_from_stack);

    // make empty output nodes
    for (int i = 0; i < effects.num_pushed; i++)
        instruction->push_nodes.push_back(new Node);

    // cross internally
    for (auto out_in_pair : effects.out_in_pairs)
    {
        // dln("cross internal o", out_in_pair.second, " with ",out_in_pair.first);

        auto pop_node = instruction->pop_nodes[out_in_pair.second];
        auto push_node = instruction->push_nodes[out_in_pair.first];

        Node::link(pop_node, push_node, pop_node->edge_register);
    }

    // push output nodes to stack
    for (auto push_node : instruction->push_nodes)
    {
        Register aRegister;
        if (push_node->target != nullptr) // it was linked in the [cross internally] step
            aRegister = push_node->edge_register;
        else
            aRegister = register_generator.get();

        stack.push_back(new Node);
        Node::link_bidirection(push_node, stack.back(), aRegister);
    }

    println("pop from ids:");
    for (auto node : instruction->pop_nodes)
        println("   ", node->edge_register.to_string());
    dln("push to ids");
    for (auto thing : instruction->push_nodes)
        dln("   ", thing->forward_edge_register.to_string());
}

void StackGrapher::graph_pass(Wordptr word) {
    // the stack is a constantly updated list of
    // loose pop nodes that the next word might need
    auto &running_stack = *(new NodeList);
    RegisterGenerator register_generator;

    for (auto instruction : word->instructions)
    {
        auto definee = instruction->linked_word;
        if(definee->name == "nop")
            continue;
        // update the word's total Effects
        word->effects.acquire_side_effects(definee->effects);

        // propagate the stack state
        dln();
        dln("[", definee->name, "]");
        propagate_stack(running_stack, instruction, word, register_generator);

        dln();
        dln("intermediate stack: ");
        for (auto thing : running_stack)
        {
            dln("\t", thing->edge_register.to_string());
        }
    }

    // the pushed side effect is the same as remaining stack frames
    word->effects.num_pushed = running_stack.size();
    word->my_graphs_outputs = running_stack;
}

void StackGrapher::retrieve_push_pop_effects(Wordptr word) {
    // matching pairs
    for (int i = 0; i < word->my_graphs_outputs.size(); i++)
    {
        Node *output = word->my_graphs_outputs[i];

        Node *next = output->target;
        while (true)
        {
            if (next->target == nullptr)
                break;
            if (next->edge_register.register_type == Register::PARAM)
            {
                word->effects.out_in_pairs.emplace_back(i, next->edge_register.ID);
                break;
            }
            next = next->target;
        }
    }

    // push pop effects
    dln();
    dln("Finished effects for ", word->name, " pops: ",
        word->my_graphs_inputs.size(), " pushes: ",
        word->my_graphs_outputs.size());
    word->effects.num_pushed = word->my_graphs_outputs.size();
    word->effects.num_popped = word->my_graphs_inputs.size();
}

/*
 * Wow the readability of this pass is fucked!
 * What is going on???
 */

void StackGrapher::branching_pass(Wordptr word) {
    dln();
    dln("Branching pass for [", word->name, "]");

    // add ret instruction at end
    // since branches may point there (and it helps with IR generation)
    word->instructions.push_back(new ReturnInstruction());

    for (int i = 0; i < word->instructions.size(); i++)
    {
        auto instruction = word->instructions[i];
        if (instruction->linked_word && instruction->linked_word->name == "branch")
        {
            int jump_rel = instruction->data.as_num();
            int jump_index = i + jump_rel + 1;
            instruction->as_branch()->jump_to = word->EntryPointingAt(word->instructions[jump_index]);
        }
        if (instruction->linked_word && instruction->linked_word->name == "branchif")
        {
            int jump_rel = instruction->data.as_num();
            int jump_index = i + jump_rel + 1;
            instruction->as_branchif()->jump_to_far = word->EntryPointingAt( word->instructions[jump_index]);

            jump_rel = 1;
            jump_index = i + jump_rel + 1;
            instruction->as_branchif()->jump_to_close = word->EntryPointingAt(word->instructions[jump_index]);
        }
    }

    // clear nop
    std::vector<Instruction*> nopless;
    for(auto thing : word->instructions){
        if(thing->linked_word->name == "nop")
            continue;
        nopless.push_back(thing);
    }
    word->instructions = std::move(nopless);

    println("bb entries:", word->BasicBlockEntries.size());
    for(auto thing = word->BasicBlockEntries.begin(); thing != word->BasicBlockEntries.end(); thing++){
        println("   ", (*thing)->target->linked_word->name);
    }

    // make sure the end of each basic block has a jump (if only just a 1 cell jump)
    auto j = word->BasicBlockEntries.begin();
    for(int i = 0; i < word->instructions.size()-1; i++){
        auto instruction = word->instructions[i];
        auto next_instruction = word->instructions[i+1];

        println("ins ", instruction->linked_word->name);

        // next instruction is the start of basic block
        // BUT current instruction is not a branch
        if(next_instruction == (*j)->target){
            println("   next is bbe");
            j++;

            if(!Instruction::is_jumpy(instruction)){
                println("   and no jump");

                auto new_intr = new BranchInstruction(new Word{.name = "branch"});
                new_intr->jump_to = word->EntryPointingAt(next_instruction);
                word->instructions.insert(word->instructions.begin() + i + 1, new_intr);
                i++;
            }
        }
    }

    // of course there is an implicit basic block: at the start of the definition
    word->EntryPointingAt(word->instructions[0]);
}

Wordptr StackGrapher::generate_ir(Wordptr wordptr) {
    println("============[", wordptr->name, "]===========");
    println("Basic block entry points:");
    int i=0; for(auto bbe : wordptr->BasicBlockEntries)
        println("index ", i++, ": ", bbe->target->linked_word->name);
    println();
    for (auto instruction : wordptr->instructions)
    {
        println("[", instruction->linked_word->name, "]");
        println("pops registers:");
        for (auto node : instruction->pop_nodes)
            println("   ", node->edge_register.to_string());

        println("pushes registers:");
        for (auto node : instruction->push_nodes)
            println("   ", node->forward_edge_register.to_string());
        println();
    }

    return wordptr;
}

