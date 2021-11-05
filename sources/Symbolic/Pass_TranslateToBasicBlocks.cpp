
#include "../../headers/Symbolic/Pass.h"
#include "../../headers/Interpretation/iData.h"

using namespace mov;

// This might be more complicated than it needs
// to be since I tried to keep everything O(n)

struct BasicBlockBuilder{
private:
    short* bbe_lookup;
    short* jump_lookup;
    const short bbe_lookup_size;

    sWordptr new_word;
    BBgen gen;

public:
    using it_type = std::vector<Block>::iterator;

    BasicBlockBuilder(sWordptr new_word, short instructions)
        : new_word(new_word), bbe_lookup_size(instructions + 1)
    {
        bbe_lookup = new short[bbe_lookup_size];
        std::fill_n(bbe_lookup, bbe_lookup_size, -1);

        // ensure there is always entry block
        bbe_lookup[0] = 0;

        const short jump_lookup_size = instructions;
        jump_lookup = new short[jump_lookup_size];
        std::fill_n(jump_lookup, jump_lookup_size, -1);
    }

    void make_bb_for_jump(int from, int delta){
        short dest_bb_entry = from + delta + 1;
        jump_lookup[from] = dest_bb_entry;

        if(bbe_lookup[dest_bb_entry] == -1){
            bbe_lookup[dest_bb_entry] = 1;
            //println("at index ", dest_bb_entry, " is a bb");
        }
    }

    it_type get_bb_at_index(int index){
        return new_word->blocks.begin() + bbe_lookup[index] - 1;
    }

    it_type get_bb_for_branch_at(int index){
        return get_bb_at_index(jump_lookup[index]);
    }

    bool is_index_bb(int index){
        return bbe_lookup[index] != -1;
    }

    int index_of_bb_at(int index) {
        return bbe_lookup[index];
    }
    int get_bb_index_for_branch_at(int index) {
        return index_of_bb_at(jump_lookup[index]);
    }

    void createBBs(){
        for(int index = 0; index < bbe_lookup_size; index++){
            if(bbe_lookup[index] != -1) {
                new_word->blocks.emplace_back(gen);
                bbe_lookup[index] = new_word->blocks.size();
            }
        }
    }
};

// why is this happening
uint sizeOf(const std::list<iData>& li){
    uint size = 0;
    for(auto thing : li)
        size++;
    return size;
}

sWordptr Analysis::translate_to_basic_blocks(ForthWord *template_word){
    auto new_word = new sWord(template_word->name(), primitive_words::OTHER);

    //dln("compute basic blocks for [" , template_word->name() , "]");

    // cache this word for the future
    visited_words[template_word] = new_word;


    //dln("Populating Basic Blocks\n");
    BasicBlockBuilder bb_builder(new_word, (short) sizeOf(template_word->def()));

    // precompute BB entry points
    int i = 0;
    std::list<iData> template_def = template_word->def();
    for(auto it = template_def.begin(); it != template_def.end(); it++, i++){
        auto template_sub_def = it;

        if(!template_sub_def->is_word())
            continue;

        if(template_sub_def->as_word()->id == primitive_words::BRANCH){
            auto next_number = std::next(it)->as_number();
            bb_builder.make_bb_for_jump(i, next_number);
        }

        if(template_sub_def->as_word()->id == primitive_words::BRANCHIF){
            bb_builder.make_bb_for_jump(i, 1);
            // must come after //TODO
            auto next_number = std::next(it)->as_number();
            bb_builder.make_bb_for_jump(i, next_number);
        }
    }

    bb_builder.createBBs();
    println();

    // fill BBs with instructions derived from template word
    auto curr_bb = bb_builder.get_bb_at_index(0);
    //println("switch to bb#", bb_builder.index_of_bb_at(0));

    i = 0;
    template_def = template_word->def();
    for(auto it = template_def.begin(); it != template_def.end(); it++, i++){

        auto template_sub_def = it->as_word();

        auto new_sub_def = static_analysis(template_sub_def);

        sData next_data = sData(nullptr);
        if(template_sub_def->stateful){
            next_data = symbolize_data(*std::next(it));
        }

        if(template_sub_def->id == primitive_words::BRANCH) {
            curr_bb->instructions.push_back(new BranchInstruction(
                    new_sub_def, next_data,
                    bb_builder.get_bb_for_branch_at(i).base()));

        } else if(template_sub_def->id == primitive_words::BRANCHIF) {
            curr_bb->instructions.push_back(new BranchIfInstruction(
                    new_sub_def, next_data,
                    bb_builder.get_bb_at_index(i + 2).base(),
                    bb_builder.get_bb_for_branch_at(i).base()));
        } else {
            curr_bb->instructions.push_back(new Instruction(new_sub_def, next_data));
        }

        // the next word will be consumed, so skip
        if(template_sub_def->stateful) {
            it++;
            i++;
        }

        // reached the end of a BB, go to next
        if(bb_builder.is_index_bb(i + 1)){
            //println("Next index is bb entry: ", i+1);
            auto next_bb = bb_builder.get_bb_at_index(i + 1);

            if(!curr_bb->instructions.back()->branchy())
                curr_bb->instructions.push_back(new BranchInstruction(new sWord("branch", primitive_words::BRANCH), sData(nullptr), next_bb.base()));

            curr_bb = next_bb;
        }
    }
    unindent();

    // ensure last instr of last BB is `return`
    auto &last_bb = new_word->blocks.back();
    auto &last_instr = last_bb.instructions.back();

    if(last_bb.instructions.empty() || last_instr->name() != "is_exit")
        last_bb.instructions.push_back(new ReturnInstruction);

    return new_word;
}



