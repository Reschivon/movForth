#ifndef MOVFORTH_INTER_INPUT_H
#define MOVFORTH_INTER_INPUT_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "../Print.h"

namespace mov {

    /**
     * Interface for all types of Forth input
     */
    class Input {
        /**
         * Next space or whitespace delimited token from inout source
         * @return std::string of token
         */
        virtual std::string next_token() = 0;

    protected:
        bool has_next = true;

    public:
        bool open(){
            return has_next;
        }
    };


    class string_input : public Input{
        std::istringstream input;

    public:
        explicit string_input(const std::string &data) : input(data) {}

        std::string next_token() override {
            std::string ret;
            bool open = (input >> ret).operator bool();
            if(open)
                return ret;
            return "";
        }
    };

    /**
     * Helper class that returns tokens one by one from file
     */
    class file_input : public Input {
        std::ifstream file;

    public:
        /**
         * Makes a file_input that immediately opens an ifstream to the provided file
         * @param path path to file
         */
        explicit file_input(const std::string& path) {
            file.open(path);
            if (!file.is_open())
                println("Cannot open file");
        }

        std::string next_token() override {
            std::string tok;

            if(file >> tok)
            {
                std::string::size_type slash_index = tok.find('\\');
                if(slash_index != std::string::npos){
                    tok = tok.substr(0, slash_index);
                    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }else
            {
                has_next = false;
            }

            if(tok.empty()){
                if(has_next)
                    return next_token();
            }else{
                return tok;
            }

            return "";
        }
    };

    class cin_input : public Input {

    public:
        std::string next_token() override {
            std::string ret;
            if(std::cin >> ret)
                return ret;
            return "";
        }
    };


}


#endif //MOVFORTH_INTER_INPUT_H
