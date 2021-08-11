#include <algorithm>
#include <iomanip>
#include <utility>
#include "../../headers/Interpretation/Interpreter.h"
#include "../../headers/Print.h"

using namespace mov;

Interpreter::Interpreter(const std::string& path) : input(path){
    init_words();

    std::string token;
    while (!(token = input.next_token()).empty()){
        iWordptr iWordptr = find(token);
        if(!iWordptr)
            iWordptr = iWordGenerator.get(token);

        if(iWordptr == nullptr){
            // might be a number
            try{
                int num = stoi(token);

                if(immediate)
                    stack.push(num);
                else {
                    if(dictionary.back().is_forth_word()){
                        //dln("compile number ", num);
                        dictionary.back().as_forth_word()->add(DictData(find("literal")));
                        dictionary.back().as_forth_word()->add(DictData(num));
                    }else{
                        println("attempted to compile LITERAL to a primitive word");
                    }

                }

            } catch (...) {
                // not a number or word
                std::cerr << "word " << token << " not found and not number"
                << std::endl;
            }
        }else{
            // it's a word
            if(iWordptr->immediate || immediate) {
                auto dummy_ip = IP();
                //dln("execute word ", Wordptr->_name());
                iWordptr->execute(dummy_ip);
            } else {
                if(dictionary.back().is_forth_word()){
                    //dln("compile FW ", Wordptr->_name());
                    dictionary.back().as_forth_word()->add(DictData(iWordptr));
                }else
                    println("attempted to compile xts to a primitive word");
            }
        }

    }
}

iWordptr Interpreter::find(const std::string& name) {
    auto find_result = std::find_if(
            dictionary.rbegin(),
            dictionary.rend(),
            [name](DictData other){return other.is_forth_word() && other.as_forth_word()->name() == name;});

    if(find_result == dictionary.rend())
        // try primitives
        return iWordGenerator.get(name);

    return find_result->as_forth_word();
}


void Interpreter::init_words(){

    iWordGenerator.register_primitive("+", primitive_words::ADD, [&](IP &i) {
        stack.push(stack.pop_number() + stack.pop_number());
    });

    iWordGenerator.register_primitive("-", primitive_words::SUBTRACT, [&](IP &i) {
        auto one = stack.pop_number();
        auto two = stack.pop_number();
        stack.push(two - one);

    });

    iWordGenerator.register_primitive("*", primitive_words::MULTIPLY, [&](IP &i) {
        stack.push(stack.pop_number() * stack.pop_number());

    });

    iWordGenerator.register_primitive("/", primitive_words::DIVIDE, [&](IP &i) {
        stack.push(stack.pop_number() / stack.pop_number());
    });

    iWordGenerator.register_primitive("swap", primitive_words::SWAP, [&](IP &i) {
        auto top = stack.pop_number(), second = stack.pop_number();
        stack.push(top);
        stack.push(second);
    });

    iWordGenerator.register_primitive("rot", primitive_words::ROT, [&](IP &i) {
        auto top = stack.pop_number();
        auto second = stack.pop_number();
        auto third = stack.pop_number();

        stack.push(second);// third
        stack.push(top);   // second
        stack.push(third); // top
    });

    iWordGenerator.register_primitive("dup", primitive_words::DUP, [&](IP &i) {
        stack.push(stack.top());
    });

    iWordGenerator.register_primitive("drop", primitive_words::DROP, [&](IP &i) {
        stack.pop_number();
    });

    iWordGenerator.register_primitive(".", primitive_words::EMIT, [&](IP &i) {
        println(stack.pop_number());
    });

    iWordGenerator.register_primitive(".S", primitive_words::SHOW, [&](IP &i) {
        stack.for_each([](DictData thing) {
            print(thing.to_string(), " ");
        });
        println("<-top");
    });

    iWordGenerator.register_primitive("'", primitive_words::TICK, [&](IP &i) {
        std::string next_token = input.next_token();
        auto cfa = find(next_token);
        if (cfa != nullptr)
            stack.push(cfa);
    });

    iWordGenerator.register_primitive(",", primitive_words::COMMA, [&](IP &i) {
        if(immediate)
            if(stack.top().is_number())
                 dictionary.emplace_back(stack.top().as_number());
            else println("COMMA pops a number from stack, but only XT was available");
        else
            if (dictionary.back().is_forth_word())
                 dictionary.back().as_forth_word()->add(stack.pop());
            else println("attempted to compile to a primitive");
    });

    iWordGenerator.register_primitive("see", primitive_words::SEE, [&](IP &i) {
        println("\n\tSo you want to see?");

        for (DictData dict_data : dictionary) {
            if(!dict_data.is_forth_word()) continue;
            iWordptr word_pointer = dict_data.as_word();

            std::cout << std::setfill(' ') << std::setw(15) <<
                      word_pointer->name() + "  ";
            print((word_pointer->immediate) ? "IMM  " : "     ");

            if (dict_data.is_forth_word())
                dict_data.as_forth_word()->definition_to_string();
            println();
        }
        println();
    });

    iWordGenerator.register_lambda_word("[", primitive_words::TOIMMEDIATE, [&](IP &i) {
        immediate = true;
    }, true);

    iWordGenerator.register_primitive("]", primitive_words::TOCOMPILE, [&](IP &i) {
        immediate = false;
    });

    iWordGenerator.register_lambda_word("immediate", primitive_words::IMMEDIATE, [&](IP &i) {
        if(!dictionary.back().is_forth_word()){
            println("tried to set immediate on number data");
            return;
        }
        dictionary.back().as_forth_word()->immediate = true;
    }, true);

    iWordGenerator.register_primitive("@", primitive_words::FETCH, [&](IP &i) {
        //TODO  nothing ... yet
    });

    iWordGenerator.register_primitive("!", primitive_words::STORE, [&](IP &i) {
        if(immediate){
            int address = stack.pop_number();
            DictData val = stack.pop();
            dictionary.at(address) = val;
        }else{
            int address = stack.pop_number();
            DictData val = stack.pop();

            if (!dictionary.back().is_forth_word())
                println("shit");
            else {
                dictionary.back().as_forth_word()->set(address, val);
            }
        }
    });

    iWordGenerator.register_primitive("branch", primitive_words::BRANCH, [&](IP &ip) {
        ip += (ip+1)->as_number();
    }, true);

    iWordGenerator.register_primitive("branchif", primitive_words::BRANCHIF, [&](IP &ip) {
        auto next_data = ip+1;
        if (stack.pop_number() == 0)
            ip += next_data->as_number();
        else
            ip++;
    }, true);

    iWordGenerator.register_primitive("literal", primitive_words::LITERAL, [&](IP &ip) {
        stack.push(*(++ip));
    }, true);

    // Use HERE for relative computations only
    // Does not represent specific address
    iWordGenerator.register_primitive("here", primitive_words::HERE, [&](IP &i) {
        if(immediate){
            // words being execud, no point in compiling branches since nothing
            // is in state of half-compilation. Prime time for ALLOTing
            dictionary.emplace_back(0);
        }else{
            // words being compiled, very bad time for ALLOT right now
            // therefore we can assume user wants to mark positions for BRANCH
            if (!dictionary.back().is_forth_word())
                println("Define some words before calling HERE");
            else
                stack.push((int) dictionary.back().as_forth_word()->def().size());
        }
    });

    iWordGenerator.register_primitive("create", primitive_words::CREATE, [&](IP &i) {
        std::string next_token = input.next_token();
        //dln("    consume ", next_token);
        dictionary.emplace_back(new ForthWord(next_token, false));
    });


    auto colon_word = new ForthWord(":", false);
    colon_word->add(DictData(find("create")));
    colon_word->add(DictData(find("]")));
    dictionary.emplace_back(colon_word);

    auto exit_word = new ForthWord(";", true);
    exit_word->add(DictData(find("[")));
    dictionary.emplace_back(exit_word);
}

