#ifndef MOVFORTH_INTER_INPUT_H
#define MOVFORTH_INTER_INPUT_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

namespace mfc {

    class Input {
        virtual std::string next_token() = 0;
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
            return
                    "";
        }
    };



    class file_input : public Input {
        std::ifstream file;

    public:
        explicit file_input(std::string path) {
            file.open(path);
            if (!file.is_open())
                std::cout << "Cannot open file " << std::endl;
        }

        std::string next_token() override {
            std::string ret;
            if(file >> ret)
                return ret;
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


#endif //MOVFORTH_INPUT_H
