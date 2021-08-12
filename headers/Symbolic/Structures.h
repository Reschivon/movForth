
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
        int ID = -1;
        int groupID = -1;
        enum registerType {UNDEF, NORMAL, PARAM} register_type = UNDEF;

        Register operator++(int){
            ID++;
            return *this;
        }

        [[nodiscard]] std::string to_string() const{
            switch (register_type)
            {
                case NORMAL:
                    return "(register " + std::to_string(groupID) + "-" + std::to_string(ID) + ")";
                case PARAM:
                    return "(input " + std::to_string(groupID) + "-" + std::to_string(ID) + ")";
                case UNDEF:
                    return "(undefined)";
                default:
                    return "(fucked)";
            }
        }
    };

    struct RegisterGen{
        Register get(){
            auto ret = current_id;
            current_id++;
            return ret;
        }
        Register get_input(){
            auto ret = current_param_id;
            current_param_id++;
            return ret;
        }

        explicit RegisterGen(int groupID) : groupID(groupID) {}
    private:
        const int groupID;
        Register current_id =       {0, groupID, Register::NORMAL};
        Register current_param_id = {0, groupID, Register::PARAM};
    };

    struct Node{
        Node* target = nullptr;
        Register edge_register;
        Register forward_edge_register;

        static void link(Node *back, Node *front, Register id){
            front->target = back;
            front->edge_register = id;
        }

        static void link_bidirection(Node *back, Node *front, Register id){
            front->target = back;
            front->edge_register = id;

            back->forward_edge_register = id;
        }
    };

    // thin wrapper for convenience
    struct NodeList : std::vector<Node*>{
        Node* push_back(Node *push){
            std::vector<Node*>::push_back(push);
            return back();
        }

        Node* new_front(){
            return push_front(new Node);
        }

        Node* push_front(Node *push){ // bad perf, but keeping it simple
            std::vector<Node*>::insert(begin(), push);
            return back();
        }

        Node* new_back(){
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
