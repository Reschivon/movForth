#include "../../headers/Symbolic/SymbolicPass.h"

using namespace mov;

void StackGrapher::compute_matching_pairs(BasicBlock &bb) {

    bb.effects.num_popped = (int) bb.my_graphs_inputs.size();
    bb.effects.num_pushed = (int) bb.my_graphs_outputs.size();

    // matching pairs
    for (int i = 0; i < bb.my_graphs_outputs.size(); i++)
    {
        Node *output = bb.my_graphs_outputs[i];

        Node *next = output->target;
        while (true)
        {
            if (next->target == nullptr)
                break;
            if (next->edge_register.register_type == Register::INPUT)
            {
                bb.effects.out_in_pairs.emplace_back(i, next->edge_register.index);
                break;
            }
            next = next->target;
        }
    }
}



