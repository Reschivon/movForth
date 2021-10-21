
#ifndef MOVFORTH_mov_TYPES_H
#define MOVFORTH_mov_TYPES_H

#include <string>
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

    class sData {
        std::variant<std::nullptr_t, int, sWordptr> data{};
    public:
        bool is_num() {return data.index() == 1;}
        bool is_xt() {return data.index() == 2;}
        bool is_empty() {return data.index() == 0;};

        std::string type(){
        switch(data.index()) {case 1: return "number"; case 2: return "xt"; case 0: return "unknown"; default: return "";}
        }
        int as_num() {return std::get<int>(data);}
        sWordptr as_xt() {return std::get<sWordptr>(data);}

        explicit sData(std::variant<std::nullptr_t, int, sWordptr> data) : data(data) {}

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
        enum registerType {UNDEF, NORMAL, INPUT} register_type = UNDEF;

        Register operator++(int){
            index++;
            return *this;
        }

        [[nodiscard]] std::string to_string() const{
            switch (register_type)
            {
                case NORMAL:
                    return "(register " + std::to_string(bb_index) + "-" + std::to_string(index) + ")";
                case INPUT:
                    return "(input    " + std::to_string(bb_index) + "-" + std::to_string(index) + ")";
               case UNDEF:
                    return "(undefined)";
                default:
                    return "(fucked)";
            }
        }
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
        Register current_param = {1, groupID, Register::INPUT};
    };

    struct Node{
        Node* target = nullptr;
        Register edge_register; // guaranteed to exist
        Register forward_edge_register; // may not always be nonnull

        static void link(Node *back, Node *front, Register id){
            front->target = back;
            front->edge_register = id;
        }

        static void link_bidirection(Node *back, Node *front, Register id){
            front->target = back;
            front->edge_register = id;

            back->forward_edge_register = id;
        }

        static void redefine_preceding_edge(Node *node, Register id){
            node->edge_register = id;
            node->target->forward_edge_register = id;
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
            to.insert(to.end(), std::make_move_iterator(from.end() - num),
                      std::make_move_iterator(from.end()));
            from.erase(from.end() - num, from.end());
        }
    };
}

#endif
