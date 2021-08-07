
#include <vector>
#include <set>
#include <stack>
#include "../../headers/Symbolic/SymbolicPass.h"
#include "../../headers/Print.h"
#include "../../headers/Symbolic/sWord.h"

using namespace sym;

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
        {"[",         new sWord("[",         {.interpret_state = sym::Effects::TOINTERPRET})},
        {"]",         new sWord("]",         {.interpret_state = sym::Effects::TOCOMPILE})},
        {"immediate", new sWord("immediate", Effects::neutral)}, // very rare this ends up in compiled code, consider warn on encounter
        {"@",         new sWord("@",         {.num_popped = 1, .num_pushed = 1})},
        {"!",         new sWord("!",         {.num_popped = 2,})},
        {"branch",    new sWord("branch",    Effects::neutral)},
        {"branchif",  new sWord("branchif",  {.num_popped = 1})},
        {"literal",   new sWord("literal",   {.num_pushed = 1})},
        {"here",      new sWord("here",      {.num_pushed = 1})},
        {"create",    new sWord("create",    {.consume_token = true, .define_new_word = true})}
};

bool is_stateful(std::string name) {
    return (name == "literal" || name == "branch" || name == "branchif");
}


Data StackGrapher::symbolize_data(mfc::Data data) {
    if (data.type() == mfc::Data::number)
        return Data(data.to_type<mfc::Data::number_t>());
    if (data.type() == mfc::Data::iword)
        return Data(compute_effects(data.to_type<mfc::Data::iword_t>()));
    if(data.type() == mfc::Data::empty)
        return Data(nullptr);
    println("FUCK");
    return Data(nullptr);
}

sWordptr StackGrapher::compute_effects(mfc::iWordptr original_word) {
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
        sWordptr word_singleton = primitive_lookup.at(original_word->base_string());
        return word_singleton;

    } else if (auto forth_word = dynamic_cast<mfc::ForthWord *>(original_word))
    {
        dln();
        dln("compute [", original_word->base_string(), "]");
        indent();

        auto converted = conversion_pass(forth_word);
        stack_graph_pass(converted);
        retrieve_push_pop_effects(converted);
        branching_pass(converted);

        unindent();
        dln("finished compute [", original_word->base_string(), "]");

        return converted;
    }

    dln("word is null");

    return nullptr;
}

sWordptr StackGrapher::compute_effects_flattened(mfc::iWordptr input) {
    auto *big_bertha = new mfc::ForthWord(input->base_string(), false);

    std::stack<mfc::iWordptr> to_add;
    to_add.push(input);

    while (!to_add.empty())
    {
        auto current = to_add.top();
        to_add.pop();

        auto forth_word = dynamic_cast<mfc::ForthWord *>(current);
        if (forth_word != nullptr)
        {
            auto def = forth_word->get_definition();
            for (auto dat = def.rbegin(); dat != def.rend(); dat++)
                to_add.push(*dat);

        } else{
            // is primitive
            big_bertha->add(mfc::Data(current));
        }
    }

    dln(input->base_string(), " flattened. Definition:");
    big_bertha->definition_to_string();

    auto converted = compute_effects(big_bertha);

    return converted;
}


sWordptr StackGrapher::conversion_pass(mfc::ForthWord *original_word) {

    //auto *new_word = new Word{.name = original_word->base_string()};
    std::vector<Instruction*> new_instructions;

    for (int i = 0; i < original_word->get_definition().size(); i++)
    {
        mfc::iWordptr old_word = original_word->get_definition()[i];

        // assume the current xt is a word
        // (all data cells should have been integrated in the previous loop)
        auto *current_definee = compute_effects(old_word);

        if(current_definee->name == "branch")
            new_instructions.push_back(new BranchInstruction(current_definee));
        else if(current_definee->name == "branchif")
            new_instructions.push_back(new BranchIfInstruction(current_definee));
        else
            new_instructions.push_back(new Instruction(current_definee));

        new_instructions.back()->data = symbolize_data(old_word->data);
    }
    auto new_word = new sWord(original_word->base_string(), Effects::neutral);
    new_word->instructions = std::move(new_instructions);

    // cache this word for the future
    visited_words[original_word] = new_word;

    return new_word;
}

void propagate_stack(NodeList &stack, Instruction *instruction, sWordptr base,
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

void StackGrapher::stack_graph_pass(sWordptr word) {
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

void StackGrapher::retrieve_push_pop_effects(sWordptr word) {
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
 *
 * Future note: What is going on?
 */

void StackGrapher::branching_pass(sWordptr word) {
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
            instruction->as_branch()->jump_to = word->block_pointing_at(
                    word->instructions.begin() + jump_index);
        }
        if (instruction->linked_word && instruction->linked_word->name == "branchif")
        {
            int jump_rel = instruction->data.as_num();
            int jump_index = i + jump_rel + 1;
            instruction->as_branchif()->jump_to_far = word->block_pointing_at(
                    word->instructions.begin() + jump_index);

            jump_rel = 1;
            jump_index = i + jump_rel + 1;
            instruction->as_branchif()->jump_to_close = word->block_pointing_at(
                    word->instructions.begin() + jump_index);
        }
    }


    println("bb entries:", word->basic_block_entries.size());
    for(auto basic_block_entries : word->basic_block_entries)
        println("   ", (*(basic_block_entries->target))->linked_word->name);

    // make sure the end of each basic block has a jump (if only just a 1 cell jump)
    auto j = word->basic_block_entries.begin();
    for(int i = 0; i < word->instructions.size()-1; i++){
        auto instruction = word->instructions.begin() + i;
        auto next_instruction = word->instructions.begin() + (i+1);

        println("ins ", (*instruction)->linked_word->name);

        // next instruction is the start of basic block
        if(*next_instruction == *(*j)->target){
            println("   next is bbe");
            j++;
            (*word->basic_block_entries.rbegin())->end = next_instruction;

            // BUT current instruction is not a branch
            if(!Instruction::is_jumpy(*instruction)){
                println("   and no jump");

                auto new_intr = new BranchInstruction(primitive_lookup.at("branch"));
                new_intr->jump_to = word->block_pointing_at(next_instruction);
                word->instructions.insert(word->instructions.begin() + i + 1, new_intr);
                i++;
            }
        }
    }

    // of course there is an implicit basic block: at the start of the definition
    word->block_pointing_at(word->instructions.begin());
}

sWordptr StackGrapher::show_word_info(sWordptr wordptr) {
    println("============[", wordptr->name, "]===========");
    println("Basic block entry points:");

    for(auto bbe : wordptr->basic_block_entries){
        println("index ", bbe->index);
        println("index ", (*bbe->target)->linked_word->name);

        for(auto it = bbe->target; it != bbe->end; it++){
            print((*it)->linked_word->name);
        }
        println();
    }
    println();


    for (auto instruction : wordptr->instructions)
    {
        println("[", instruction->linked_word->name, "]");
        print("pops:  ");
        for (auto node : instruction->pop_nodes)
            print(" ", node->edge_register.to_string());
        println();

        print("pushes:");
        for (auto node : instruction->push_nodes)
            print(" ", node->forward_edge_register.to_string());
        println();
        println();
    }

    return wordptr;
}

