
#include <set>
#include <stack>
#include "../../headers/Symbolic/Pass.h"
#include "../../headers/Print.h"
#include "../../headers/Interpretation/iData.h"

using namespace mov;

sData Analysis::symbolize_data(iData data) {
    if (data.is_number())
        return sData(data.as_number());
    if (data.is_word())
        return sData(static_analysis(data.as_word()));
    if(data.is_empty())
        return sData(nullptr);
    println("FUCK");
    return sData(nullptr);
}

sWordptr Analysis::static_analysis(iWordptr original_word) {

    // check to see if we have passed over this word already
    // if so, return a pointer to it
    auto cached = visited_words.find(original_word);
    if (cached != visited_words.end())
        return cached->second;

    if (dynamic_cast<Primitive*>(original_word))
    {   // is a primitive: return the singleton of the primitive
        sWordptr word_singleton = primitive_lookup.at(original_word->id);
        return word_singleton;
    }

    if (auto forth_word = dynamic_cast<ForthWord *>(original_word))
    {

        dln();
        dln("Translate ", original_word->name(), " to basic blocks");

        auto converted = translate_to_basic_blocks(forth_word);

        dln("[", original_word->name(), "] START graphing all BBs");
        indent();

        word_stack_graph(converted);

        unindent();
        dln("[", original_word->name(), "] FINSIHED graphing all BBs");
        dln("pushes: ", converted->effects.num_pushed);
        dln("pops: ", converted->effects.num_popped);

        return converted;
    }

    println("Word provided for static analysis is null");

    return nullptr;
}

sWordptr Analysis::show_word_info(sWordptr wordptr) {
    println("============[", wordptr->name, "]===========");

    println("push:" , wordptr->effects.num_pushed,
            " pop:" , wordptr->effects.num_popped);

    println();
    println("Basic Blocks:");
    for(const auto& bbe : wordptr->blocks){
        println("bbe " + std::to_string(bbe.index) + ":");
        print("    ");
        for(auto instr : bbe.instructions)
            print(" " , instr->name());
        println();
    }
    println();

    println();
    println("Stack Graph:");

    for(auto &bb : wordptr->blocks){
        println("----------------------------------");
        println("[" , bb.name() , "] stack graph");
        indent();

        for(auto &instr : bb.instructions){
            // propagate the stack state
            println();
            println("[", instr->name(), "]");

            if(!instr->pop_nodes.empty()) {
                print("pops:");
                for (auto node: instr->pop_nodes)
                    print(" ", node->backward_edge_register.to_string());
                println();
            }

            if(!instr->push_nodes.empty()) {
                print("pushes:");
                for (auto thing: instr->push_nodes)
                    print(" ", thing->forward_edge_register.to_string());
                println();
            }
        }

        unindent();

        print("next BBs: ");
        for(auto next : bb.nextBBs())
            print(next.get().index , " ");
        println();
    }

    return wordptr;
}

std::list<iData>::iterator insert_into(std::list<iData> host, std::list<iData>::const_iterator it, std::list<iData> to_insert){
    // inline it
    for(const auto& sub_word : to_insert)
        host.insert(it, sub_word);
    return host.erase(it);
}

const static uint INLINE_WORD_MAX_XTS = 50;
void dfs(iWordptr word, std::set<iWordptr>& visited){

    if(visited.find(word) != visited.end()) {
        visited.insert(word);
        return;
    }

    if(dynamic_cast<Primitive*>(word)){
        return;
    }


    if(auto *fw = dynamic_cast<ForthWord*>(word)){
        auto &definition = fw->def();
        int branch_offset = 0;
        for(auto it = definition.begin(); it != definition.end(); it++){
            if(it->is_word()) {
                dfs(it->as_word(), visited);

                if (it->as_word()->id == primitive_words::OTHER) {
                    ForthWord *sub_fw = dynamic_cast<ForthWord *>(it->as_word());
                    auto &sub_def = sub_fw->def();
                    if (definition.size() + sub_def.size() < INLINE_WORD_MAX_XTS) {
                        dln("Inlining word ", sub_fw->name(), " into ", word->name());
                        it = insert_into(definition, it, sub_def);

                        branch_offset += sub_def.size() - 1;
                    }
                }

                auto curr_word_id = it->as_word()->id;
                auto next_slot = std::next(it);
                if((curr_word_id == primitive_words::BRANCH || curr_word_id == primitive_words::BRANCHIF)
                    && next_slot->as_number() < 0){
                    *next_slot = iData(next_slot->as_number() - branch_offset);
                }
            }
        }
    }
}

void Analysis::inlining(iWordptr root) {
    std::set<iWordptr> visited;
    dfs(root, visited);
}

