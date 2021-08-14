#include "../../headers/Symbolic/Pass.h"

using namespace mov;

/**
 * Check if any stack elments pass through bb without changes
 * If so, registers a matching pair to bb, which can be used
 * for optimizing register generation
 * @param bb
 */
void Analysis::compute_matching_pairs(BasicBlock &bb) {

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



