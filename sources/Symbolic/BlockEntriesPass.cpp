
#include "../../headers/Symbolic/SymbolicPass.h"
using namespace mov;

struct BasicBlockBuilder{
private:
    short* bbe_lookup;
    sWordptr new_word;
    BBgen gen;
public:
    using it_type = std::vector<BasicBlock>::iterator;
    BasicBlockBuilder(sWordptr new_word, short instructions)
        : new_word(new_word) {
        const short bbe_lookup_size = instructions + 1;
        bbe_lookup = new short[bbe_lookup_size];
        std::fill_n(bbe_lookup, bbe_lookup_size, -1);

        // ensure there is always entry block
        bbe_lookup[0] = 0;
        new_word->basic_blocks.emplace_back(gen);
    }

    void make_bb_at_index(short index){
        if(bbe_lookup[index] == -1){
            bbe_lookup[index] = new_word->basic_blocks.size();
            new_word->basic_blocks.emplace_back(gen);
        }
    }

    it_type get_bb_at_index(int index){
        return new_word->basic_blocks.begin() + bbe_lookup[index];
    }

    bool is_index_bb(int index){
        return bbe_lookup[index] != -1;
    }
};

bool branchy(iData word){
    if(!word.is_word())
        return false;
    return word.to_string() == "branch" ||
           word.to_string() == "branchif" ||
           word.to_string() == "return";
}

bool branchy(Instruction *instr){
    if(!instr->linked_word)
        return false;
    return instr->linked_word->name == "branch" ||
           instr->linked_word->name == "branchif" ||
           instr->linked_word->name == "return";
}

sWordptr StackGrapher::MakeBlockEntries(ForthWord *template_word){
    auto new_word = new sWord(template_word->base_string());

    println("make block entries for [" , template_word->to_string() , "]");

    // cache this word for the future
    visited_words[template_word] = new_word;

    BasicBlockBuilder basic_block_builder(new_word, (short) template_word->def().size());

    for(int i = 0; i < template_word->def().size(); i++){
        auto &template_sub_def = template_word->def().at(i);
        if(!template_sub_def.is_word())
            continue;

        if(template_sub_def.as_word()->base_string() == "branch"){
            int word_data = template_word->def()[i+1].as_number();
            int jump_dest_index = i + word_data + 1;
            basic_block_builder.make_bb_at_index(jump_dest_index);
        }

        if(template_sub_def.as_word()->base_string() == "branchif"){
            int word_data = template_word->def()[i+1].as_number();
            int jump_dest_index = i + word_data + 1;
            basic_block_builder.make_bb_at_index(jump_dest_index);

            jump_dest_index = i + 1 + 1;
            basic_block_builder.make_bb_at_index(jump_dest_index);
        }
    }

    auto curr_bb = basic_block_builder.get_bb_at_index(0);
    for(int i = 0; i < template_word->def().size(); i++){
        if(template_word->def()[i].is_word()){

            auto template_sub_def = template_word->def()[i].as_word();
            auto new_sub_def = compute_effects(template_sub_def);

            sData next_data = sData(nullptr);
            if(template_sub_def->stateful){
                next_data = symbolize_data(template_word->def()[i + 1]);
            }

            if(template_sub_def->base_string() == "branch")
            {
                int jump_to_index = i + next_data.as_num() + 1;
                auto jump_to_bb = basic_block_builder.get_bb_at_index(jump_to_index);

                println("new branch");
                curr_bb->instructions.push_back(
                        new BranchInstruction(new_sub_def, next_data, jump_to_bb.base()));

            }else if(template_sub_def->base_string() == "branchif")
            {
                int jump_to_far_index = i + next_data.as_num() + 1;
                int jump_to_next_index = i + 1 + 1;

                auto jump_to_far_bb = basic_block_builder.get_bb_at_index(jump_to_far_index);
                auto jump_to_next_bb = basic_block_builder.get_bb_at_index(jump_to_next_index);

                println("new branchif");
                curr_bb->instructions.push_back(
                        new BranchIfInstruction(new_sub_def, next_data, jump_to_next_bb.base(), jump_to_far_bb.base()));
            }else
            {
                println("new ins ", new_sub_def->name);
                curr_bb->instructions.push_back(new Instruction(new_sub_def, next_data));
            }
        }

        if(basic_block_builder.is_index_bb(i+1)){
            auto next_bb = basic_block_builder.get_bb_at_index(i+1);

            auto last_instr = curr_bb->instructions.back();

            if(!branchy(last_instr))
                curr_bb->instructions.push_back(new BranchInstruction(new sWord("branch"), sData(nullptr), next_bb.base()));

            curr_bb = next_bb;
        }
    }

    println("new word has " , new_word->basic_blocks.size() , " bbs");

    // ensure last instr of last bb is return
    auto &last_bb = new_word->basic_blocks.back();
    auto &last_instr = last_bb.instructions.back();

    if(last_bb.instructions.empty() || last_instr->linked_word->name != "return")
        last_bb.instructions.push_back(new ReturnInstruction);

    return new_word;
}


