
#ifndef MOVFORTH_INSTRUCTION_H
#define MOVFORTH_INSTRUCTION_H

#include "Structures.h"
#include "../PrimitiveEnums.h"

namespace mov{

    struct BranchInstruction;
    struct BranchIfInstruction;
    struct ReturnInstruction;

    struct Instruction{
        sWordptr linked_word;
        sData data = sData(nullptr); // acquired from next in token list

        NodeList pop_nodes{};
        NodeList push_nodes{};

        explicit Instruction(sWordptr linked_word, sData data);

        BranchInstruction* as_branch();
        BranchIfInstruction* as_branchif();
        ReturnInstruction* as_return();

        virtual std::string name();
        [[nodiscard]] primitive_words id() const;

        [[nodiscard]] bool branchy() const;
    };

    struct Block;

    struct BranchInstruction : public Instruction {
        Block *jump_to;
        explicit BranchInstruction(sWordptr linked_word, sData data, Block *jump_to);

        std::string name() override;
    };

    struct BranchIfInstruction : public Instruction {
        Block *jump_to_next;
        Block *jump_to_far;
        explicit BranchIfInstruction(sWordptr linked_word, sData data, Block *jump_to_close, Block *jump_to_far);

        std::string name() override;
    };

    struct ReturnInstruction : public Instruction{
        ReturnInstruction();
        std::string name() override;
    };

}

#endif //MOVFORTH_INSTRUCTION_H
