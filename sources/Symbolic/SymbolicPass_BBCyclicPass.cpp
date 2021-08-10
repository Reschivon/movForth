#include "../../headers/Symbolic/SymbolicPass.h"

using namespace mov;

struct Conflict{
    BasicBlock *from, *to;
    Conflict(BasicBlock *from, BasicBlock *to)
        : from(from), to(to) {}
    [[nodiscard]] std::string to_string() const{
        return "conflict from bb#" +std::to_string(from->index) +
                " to bb#" + std::to_string(to->index);
    }

};

/*void explore_graph_dfs(BasicBlock *bb, std::vector<Conflict> &conflicts, sWordptr wordptr){
    if(bb->visited)
        return;
    for(auto *next : bb->nextBBs()){
        if(next->is_exit())
            wordptr->effects.num_pushed =
            conflicts.emplace_back(bb, next);
        explore_graph_dfs(next, conflicts, wordptr);
    }
    bb->visited = true;
}*/

void StackGrapher::bb_cyclic_pass(sWordptr wordptr) {
    println();
    println("BB cyclic pass");

    std::vector<Conflict> conflicts;
    //explore_graph_dfs(&wordptr->basic_blocks.front(), conflicts);

    if(conflicts.empty()){
        println("   found no conflicts");
    }else{
        println("   found conflicts:");
        for(auto conflict : conflicts)
            println("    " , conflict.to_string());
    }


}


