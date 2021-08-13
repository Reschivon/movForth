
#include <set>
#include <stack>
#include "../../headers/Symbolic/SymbolicPass.h"
#include "../../headers/Print.h"
#include "../../headers/Interpretation/iData.h"

using namespace mov;

sData StackGrapher::symbolize_data(iData data) {
    if (data.is_number())
        return sData(data.as_number());
    if (data.is_word())
        return sData(static_analysis(data.as_word()));
    if(data.is_empty())
        return sData(nullptr);
    println("FUCK");
    return sData(nullptr);
}

sWordptr StackGrapher::static_analysis(iWordptr original_word) {
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
        dln("analyze [", original_word->name(), "]");
        indent();

        auto converted = translate_to_basic_blocks(forth_word);
        word_stack_graph(converted);

        unindent();
        dln("finished analyzing [", original_word->name(), "]");

        return converted;
    }

    dln("word is null");

    return nullptr;
}

sWordptr StackGrapher::show_word_info(sWordptr wordptr) {
    println("============[", wordptr->name, "]===========");


    println("push:" , wordptr->effects.num_pushed,
            " pop:" , wordptr->effects.num_popped);

    println();
    println("Basic Blocks:");
    for(const auto& bbe : wordptr->basic_blocks){
        println("bbe " + std::to_string(bbe.index) + ":");
        print("    ");
        for(auto instr : bbe.instructions)
            print(" " , instr->name());
        println();
    }
    println();

    println();
    println("Stack Graph:");

    for(auto &bb : wordptr->basic_blocks){
        println();
        println("[bb#: " , bb.index , "] BEGIN stack graph");
        indent();

        for(auto &instr : bb.instructions){
            // propagate the stack state
            dln();
            dln("[", instr->name(), "]");

            print("pops:");
            for (auto node : instr->pop_nodes)
                print(" ", node->edge_register.to_string());
            println();

            print("pushes:");
            for (auto thing : instr->push_nodes)
                print(" ", thing->forward_edge_register.to_string());
            println();
        }

        unindent();
        println("[bb#: " , bb.index , "] END stack graph");

        print("next BBs: ");
        for(auto next : bb.nextBBs())
            print(next.get().index , " ");
        println();
    }

    return wordptr;
}

