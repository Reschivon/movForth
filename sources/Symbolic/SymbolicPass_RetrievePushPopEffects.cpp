#include "../../headers/Symbolic/SymbolicPass.h"

using namespace mov;

void StackGrapher::retrieve_push_pop_effects(sWordptr word) {
    /*
    // matching pairs
    for (int i = 0; i < word->my_graphs_outputs.size(); i++)
    {
        Node *output = word->my_graphs_outputs[i];

        Node *next = output->target;
        while (true)
        {
            if (next->target == nullptr)
                break;
            if (next->edge_register.register_type == Register::PARAM)
            {
                word->effects.out_in_pairs.emplace_back(i, next->edge_register.ID);
                break;
            }
            next = next->target;
        }
    }

    // push pop effects
    dln("Finished effects for ", word->name, " pops: ",
        word->my_graphs_inputs.size(), " pushes: ",
        word->my_graphs_outputs.size());
    word->effects.num_pushed = (int) word->my_graphs_outputs.size();
    word->effects.num_popped = (int) word->my_graphs_inputs.size();

     */
}



