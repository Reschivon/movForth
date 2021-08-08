
#include <set>
#include <utility>
#include "Structures.h"

#ifndef MOVFORTH_WORD_H
#define MOVFORTH_WORD_H

namespace mov{

    struct Effects{
        int num_popped = 0;
        int num_pushed = (0);

        bool consume_token = false; // ONLY for single token consumers, leave out "

        unsigned int compiled_slots = 0; // TODO still unsure how to handle memory

        enum interpret_state{NONE, TOCOMPILE, TOINTERPRET} interpret_state = NONE;

        bool define_new_word = false;

        // mapping of index for identical registers
        std::vector<std::pair<int, int>> out_in_pairs;

        void acquire_side_effects(Effects& other){
            // pop and push handled elsewhere

            if(other.consume_token) consume_token = true;

            compiled_slots += other.compiled_slots;

            if(other.interpret_state != NONE)
                interpret_state = other.interpret_state;

            if(other.define_new_word) define_new_word = true;
        }

        static Effects neutral;
    };

    struct BranchInstruction;
    struct BranchIfInstruction;
    struct ReturnInstruction;
    struct Instruction{
        NodeList pop_nodes;
        NodeList push_nodes;

        sWordptr linked_word;
        sData data = sData(nullptr); // acquired from next in token list

        explicit Instruction(sWordptr linked_word) : linked_word(linked_word) {}

        static bool is_jumpy(Instruction*);
        BranchInstruction* as_branch();
        BranchIfInstruction* as_branchif();
        ReturnInstruction* as_return();

        virtual std::string to_string();
    };

    struct BBEgen{
        unsigned int get(){
            return id++;
        }
    private:
        unsigned int id = 0;
    };

    struct BasicBlock {
        explicit BasicBlock(BBEgen& gen) : index(gen.get()) {}
        std::vector<Instruction*> instructions;//TODO reference wrapper
        unsigned int index = 0; // like Register but I was lazy
    };

    struct BranchInstruction : public Instruction {
        BasicBlock *jump_to = nullptr;
        explicit BranchInstruction(sWordptr linked_word, BasicBlock *jump_to)
            : Instruction(linked_word), jump_to(jump_to) {}

        std::string to_string() override;
    };
    struct BranchIfInstruction : public Instruction {
        BasicBlock *jump_to_next = nullptr;
        BasicBlock *jump_to_far = nullptr;
        explicit BranchIfInstruction(sWordptr linked_word, BasicBlock *jump_to_close, BasicBlock *jump_to_far)
            : Instruction(linked_word), jump_to_next(jump_to_close), jump_to_far(jump_to_far) {}

        std::string to_string() override;
    };
    struct ReturnInstruction : public Instruction{
        ReturnInstruction();
        std::string to_string() override {
            return "return";
        }
    };

    class sWord {
    public:
        const std::string name;
        Effects effects;

        explicit sWord(const std::string& name, Effects effects)
                : name(name), effects(effects) {}

        // components for graph
        //std::vector<Wordptr> definition;
        NodeList my_graphs_outputs;
        NodeList my_graphs_inputs;

        void definition_to_string();

        std::vector<BasicBlock> basic_blocks;

        /*BasicBlock* block_pointing_at(std::vector<Instruction*>::iterator target){
            auto *bbe = new BasicBlock{.target = target};
            auto success = basic_blocks.insert(bbe);
            if(success.second) // newly inserted bbe
                (*success.first)->index = bbe_gen.get();
            return bbe;
        }*/

        std::vector<Instruction*> instructions;

        BBEgen bbe_gen;
    };

}


#endif //MOVFORTH_WORD_H
