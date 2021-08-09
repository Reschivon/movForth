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


void dfs(BasicBlock *bb, std::vector<Conflict> &conflicts){
    if(bb->visited)
        return;
    for(auto *next : bb->nextBBs()){
        // The `exit` BB is a special case
        // Since it is more of an indication of definition end
        // than an actual instruction, it can take as many items
        // off the stack as needed
        if(next->is_exit())
            next->effects.num_popped = bb->effects.num_pushed;
        else if(bb->effects.num_pushed != next->effects.num_popped)
            conflicts.emplace_back(bb, next);
        dfs(next, conflicts);
    }
    bb->visited = true;
}

void StackGrapher::bb_cyclic_pass(sWordptr wordptr) {
    println();
    println("BB cyclic pass");

    std::vector<Conflict> conflicts;
    dfs(&wordptr->basic_blocks.front(), conflicts);

    if(conflicts.empty()){
        println("   found no conflicts");
    }else{
        println("   found conflicts:");
        for(auto conflict : conflicts)
            println("    " , conflict.to_string());
    }
}


