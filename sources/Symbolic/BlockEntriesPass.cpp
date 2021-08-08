
#include "../../headers/Symbolic/SymbolicPass.h"
using namespace mov;

bool branchy(const std::string& name){
    return name == "branch" || name == "branchif" || name == "return";
}

sWordptr StackGrapher::MakeBlockEntries(ForthWord *old_word){
    auto new_word = new sWord(old_word->base_string(), Effects::neutral);

    println("make block entries for [" , old_word->to_string() , "]");

    // cache this word for the future
    visited_words[old_word] = new_word;

    BBEgen gen;

    unsigned int bbe_lookup_size = old_word->def().size() + 1;
    short bbe_lookup[bbe_lookup_size];
    std::fill_n(bbe_lookup, bbe_lookup_size, -1);

    // ensure there is always entry block
    bbe_lookup[0] = 0;
    new_word->basic_blocks.emplace_back(gen);

    println("original def");
    for(auto thing : old_word->def())
        print(thing.to_string() , " ");

    for(int i = 0; i < old_word->def().size(); i++){
        if(old_word->def()[i].is_word() && branchy(old_word->def()[i].as_word()->base_string())){
            int jump_amount = old_word->def()[i + 1].as_number();
            int jump_to_index = i + jump_amount + 1;

            if(bbe_lookup[jump_to_index] == -1){
                bbe_lookup[jump_to_index] = new_word->basic_blocks.size();
                println("allocating dest " , jump_to_index , " for bbe#" , new_word->basic_blocks.size());
                new_word->basic_blocks.emplace_back(BasicBlock(gen));
            }

            if(old_word->def()[i].as_word()->base_string() == "branchif"){
                jump_amount = 1;
                jump_to_index = i + jump_amount + 1;

                if(bbe_lookup[jump_to_index] == -1){
                    bbe_lookup[jump_to_index] = new_word->basic_blocks.size();
                    println("allocating dest " , jump_to_index , " for bbe#" , new_word->basic_blocks.size());
                    new_word->basic_blocks.emplace_back(BasicBlock(gen));
                }
            }
        }
    }

    println(new_word->basic_blocks.size() , " bbes; lookup:");
    for(auto thing : bbe_lookup)
        print(thing , " ");
    println();

    auto curr_bb = new_word->basic_blocks.begin();
    for(int i = 0; i < old_word->def().size(); i++){
        if(old_word->def()[i].is_word()){

            auto old_sub_def = old_word->def()[i].as_word();
            auto new_sub_def = compute_effects(old_sub_def);

            auto next_data = sData(nullptr);
            if(old_sub_def->stateful){
                next_data = symbolize_data(old_word->def()[i+1]);
            }

            if(old_sub_def->base_string() == "branch")
            {
                int jump_to_index = i + next_data.as_num() + 1;
                auto jump_to_bb = new_word->basic_blocks.begin() + bbe_lookup[jump_to_index];

                println("    " , jump_to_index, " new branch to " + std::to_string(bbe_lookup[jump_to_index]));
                curr_bb->instructions.push_back(new BranchInstruction(
                        new_sub_def, jump_to_bb.base()));

            }else if(old_sub_def->base_string() == "branchif")
            {
                int jump_to_far_index = i + next_data.as_num() + 1;
                int jump_to_next_index = i + 1 + 1;

                auto jump_to_far_bb = new_word->basic_blocks.begin() + bbe_lookup[jump_to_far_index];
                auto jump_to_next_bb = new_word->basic_blocks.begin() + bbe_lookup[jump_to_next_index];

                println("    " , jump_to_far_index, " new branchif to " + std::to_string(bbe_lookup[jump_to_next_index]) + " and " + std::to_string(bbe_lookup[jump_to_far_index]));

                curr_bb->instructions.push_back(new BranchIfInstruction(
                        new_sub_def, jump_to_next_bb.base(), jump_to_far_bb.base()));
            }else
            {
                println("    new reg instr " + new_sub_def->name);
                curr_bb->instructions.push_back(new Instruction(new_sub_def));
            }
        }

        if(bbe_lookup[i+1] != -1){
            println("next adding to bbe# " , bbe_lookup[i+1]);
            auto next_bb = new_word->basic_blocks.begin() + bbe_lookup[i+1];
            // we're about to switch BBs and the last instr is not branch
            if(old_word->def()[i].is_word() &&
            !branchy(old_word->def()[i].as_word()->to_string()))
                curr_bb->instructions.push_back(
                        new BranchInstruction(new sWord("branch", Effects::neutral), next_bb.base()));

            curr_bb = next_bb;
        }
    }

    // ensure last instr of last bb is return
    auto &last_bb = new_word->basic_blocks.back();
    auto &last_instr = last_bb.instructions.back();

    if(last_bb.instructions.empty() || last_instr->linked_word->name != "return")
        last_bb.instructions.push_back(new ReturnInstruction);

    return new_word;
}


