#include "../../headers/Symbolic/Pass.h"

using namespace mov;

/**
 * Check if any stack elments pass through bb without changes
 * If so, registers a matching pair to bb, which can be used
 * for optimizing register generation
 * @param bb
 */
void Analysis::compute_matching_pairs(Block &bb) {

    // matching pairs
    for (int i = 0; i < bb.outputs.size(); i++)
    {
        Node *output = bb.outputs[i];

        Node *next = output->target;
        while (true)
        {
            if (next->target == nullptr)
                break;
            if (next->backward_edge_register.register_type == Register::PARAM)
            {
                bb.effects_without_push_pop.out_in_pairs.emplace_back(i, next->backward_edge_register.index);
                break;
            }
            next = next->target;
        }
    }
}



