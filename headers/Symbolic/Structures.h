
#ifndef MOVFORTH_mov_TYPES_H
#define MOVFORTH_mov_TYPES_H

#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <iostream>
#include <functional>
#include "../Print.h"

/*
 * So many graphs! Graphs, graphs, everywhere!
 *
 * It was a lot easier to use raw pointers than
 * to deal with references and shared_ptr
 */

namespace mov {

    class sWord;

    typedef sWord *sWordptr;

    struct arrayIndex()

    class sData {
        using variant_t = std::variant<std::nullptr_t, int, sWordptr>;
        variant_t data{};
    public:
        bool is_num() {return data.index() == 1;}
        bool is_xt() {return data.index() == 2;}
        bool is_empty() {return data.index() == 0;};

        std::string type(){
            switch(data.index()) {
                case 0: return "unknown";
                case 1: return "number";
                case 2: return "xt";
                default: return "";
            }
        }
        int as_num() {return std::get<int>(data);}
        sWordptr as_xt() {return std::get<sWordptr>(data);}

        explicit sData(variant_t data) : data(std::move(data)) {}

        std::string to_string(){
            if(is_xt())
                return "xt";
            if(is_num())
                return std::to_string(as_num());
            return "undef";
        }
    };

    struct Register{
        int index = -1;
        int bb_index = -1;
        enum registerType {UNDEF=0, NORMAL=1, PARAM=2, RETURN=3} register_type = UNDEF;

        Register operator++(int){
            index++;
            return *this;
        }

        bool operator==(const Register &other) const {
            return (index == other.index &&
                    bb_index == other.bb_index &&
                    register_type == other.register_type);
        }

        [[nodiscard]] std::string to_string() const{
            switch (register_type)
            {
                case NORMAL:
                    return "(register " + std::to_string(bb_index) + "-" + std::to_string(index) + ")";
                case PARAM:
                    return "(param    " + std::to_string(bb_index) + "-" + std::to_string(index) + ")";
                case RETURN:
                    return "(return    " + std::to_string(bb_index) + "-" + std::to_string(index) + ")";
                case UNDEF:
                    return "(undefined)";
                default:
                    return "(fucked)";
            }
        }

        [[nodiscard]] std::string to_string_allowed_chars() const {
            switch (register_type)
            {
                case NORMAL:
                    return "reg" + std::to_string(bb_index) + "." + std::to_string(index);
                case PARAM:
                    return "par" + std::to_string(bb_index) + "." + std::to_string(index);
                case RETURN:
                    return "ret" + std::to_string(bb_index) + "." + std::to_string(index);
                case UNDEF:
                    return "undef";
                default:
                    return "fuck";
            }
        }

    public:
        struct RegisterHash{
            std::size_t operator()(const Register& r) const
            {
                using std::size_t;

                std::size_t concatenated = (uint) r.index;
                concatenated <<= 32;
                concatenated |= (uint) r.bb_index;
                std::size_t enh = (uint) r.register_type;
                enh <<= 61;
                concatenated |= enh;

                return concatenated;
            }
        };

    };

    struct RegisterGen{
        Register get(){
            auto ret = current;
            current++;
            return ret;
        }
        Register get_param(){
            auto ret = current_param;
            current_param++;
            return ret;
        }

        explicit RegisterGen(int groupID) : groupID(groupID) {}
    private:
        const int groupID;
        Register current =       {1, groupID, Register::NORMAL};
        Register current_param = {1, groupID, Register::PARAM};
    };

    struct Node{
        Node* prev_node = nullptr;
        Register backward_edge_register; // guaranteed to exist
        Register forward_edge_register; // may not always be nonnull

        static void link(Node *back, Node *front, Register id){
            front->prev_node = back;
            front->backward_edge_register = id;
        }

        static void link_bidirection(Node *back, Node *front, Register id){
            front->prev_node = back;
            front->backward_edge_register = id;

            back->forward_edge_register = id;
        }

        static void redefine_preceding_edge(Node *node, Register id){
            node->backward_edge_register = id;
            node->prev_node->forward_edge_register = id;
        }

        static void redefine_preceding_type(Node *node, Register::registerType ty){
            Register old_reg = node->backward_edge_register;
            old_reg.register_type = ty;
            redefine_preceding_edge(node, old_reg);
        }
    };

    // thin wrapper for convenience
    struct NodeList : std::vector<Node*>{
        Node* push_back(Node *push){
            std::vector<Node*>::push_back(push);
            return back();
        }

        Node* new_bottom(){
            return push_bottom(new Node);
        }

        Node* push_bottom(Node *push){ // bad perf, but keeping it simple
            std::vector<Node*>::insert(begin(), push);
            return back();
        }

        Node* new_top(){
            return push_back(new Node);
        }

        unsigned int size(){
            return (unsigned int) std::vector<Node*>::size();
        }

        static void move_top_elements(NodeList &from, NodeList &to, int num){
            for(uint i = 1; i <= num; i++){
                Node *stack_top = from[from.size() - i];
                to.push_back(stack_top);
            }
            from.erase(from.end() - num, from.end());
        }
    };
}

#endif
