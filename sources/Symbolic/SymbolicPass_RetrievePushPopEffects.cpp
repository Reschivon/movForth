#include "../../headers/Symbolic/SymbolicPass.h"

using namespace mov;

void retrieve_push_pop_effects_bb(BasicBlock &bb) {
    // matching pairs
    for (int i = 0; i < bb.my_graphs_outputs.size(); i++)
    {
        Node *output = bb.my_graphs_outputs[i];

        Node *next = output->target;
        while (true)
        {
            if (next->target == nullptr)
                break;
            if (next->edge_register.register_type == Register::PARAM)
            {
                bb.effects.out_in_pairs.emplace_back(i, next->edge_register.ID);
                break;
            }
            next = next->target;
        }
    }

    // print push pop effects
//    dln("Finished effects for bb#", bb.index ,
//        " pops: "  , bb.my_graphs_inputs.size(),
//        " pushes: ", bb.my_graphs_outputs.size());
    bb.effects.num_pushed = (int) bb.my_graphs_outputs.size();
    bb.effects.num_popped = (int) bb.my_graphs_inputs.size();
}

void StackGrapher::retrieve_push_pop_effects(sWordptr wordptr) {
    for(auto &bb : wordptr->basic_blocks)
        retrieve_push_pop_effects_bb(bb);
}



