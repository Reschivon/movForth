#include <algorithm>
#include <iomanip>
#include <utility>
#include "../../headers/Interpretation/Interpreter.h"
#include "../../headers/Print.h"

using namespace mov;

Interpreter::Interpreter(const std::string& path) : input(path) {
    init_words();
}

bool Interpreter::interpret() {
    dln("========[Interpretation]======");

    std::string token;
    while (true){
        token = input.next_token();
        if(!input.open()) break;

        iWordptr iWordptr = find(token);
        if(!iWordptr)
            iWordptr = word_generator.get(token);

        if(iWordptr == nullptr){
            // might be a number
            try{
                int num = stoi(token);

                if(immediate)
                    stack.push(num);
                else {
                    if(dictionary.back().is_forth_word()){
                        //dln("compile number ", num);
                        dictionary.back().as_forth_word()->add(iData(find("literal")));
                        dictionary.back().as_forth_word()->add(iData(num));
                    }else{
                        println("attempted to compile LITERAL to a primitive word");
                        return true;
                    }
                }
            } catch (...) {
                // not a number or word
                println("word ", token, " not found and not number");
                return true;
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
                    dictionary.back().as_forth_word()->add(iData(iWordptr));
                }else {
                    println("attempted to compile xts to a primitive word");
                    return true;
                }
            }
        }

    }

    dln("======[End Interpretation]======");
    println();
    return false;
}

iWordptr Interpreter::find(const std::string& name) {
    auto find_result = std::find_if(
            dictionary.rbegin(),
            dictionary.rend(),
            [name](iData other){return other.is_forth_word() && other.as_forth_word()->name() == name;});

    if(find_result == dictionary.rend())
        // try primitives
        return word_generator.get(name);

    return find_result->as_forth_word();
}


void Interpreter::init_words(){

    word_generator.register_primitive("=", primitive_words::EQUALS, [&](IP &ip) {
        stack.push(stack.pop_number() == stack.pop_number());
    });

    word_generator.register_primitive("+", primitive_words::ADD, [&](IP &ip) {
        stack.push(stack.pop_number() + stack.pop_number());
    });

    word_generator.register_primitive("-", primitive_words::SUBTRACT, [&](IP &ip) {
        auto one = stack.pop_number();
        auto two = stack.pop_number();
        stack.push(two - one);

    });

    word_generator.register_primitive("*", primitive_words::MULTIPLY, [&](IP &ip) {
        stack.push(stack.pop_number() * stack.pop_number());

    });

    word_generator.register_primitive("/", primitive_words::DIVIDE, [&](IP &ip) {
        stack.push(stack.pop_number() / stack.pop_number());
    });

    word_generator.register_primitive("swap", primitive_words::SWAP, [&](IP &ip) {
        auto top = stack.pop_number(), second = stack.pop_number();
        stack.push(top);
        stack.push(second);
    });

    word_generator.register_primitive("rot", primitive_words::ROT, [&](IP &ip) {
        auto top = stack.pop_number();
        auto second = stack.pop_number();
        auto third = stack.pop_number();

        stack.push(second);// third
        stack.push(top);   // second
        stack.push(third); // top
    });

    word_generator.register_primitive("dup", primitive_words::DUP, [&](IP &ip) {
        stack.push(stack.top());
    });

    word_generator.register_primitive("drop", primitive_words::DROP, [&](IP &ip) {
        stack.pop_number();
    });

    word_generator.register_primitive(".", primitive_words::EMIT, [&](IP &ip) {
        println(stack.pop_number());
        println();
    });

    word_generator.register_primitive(".S", primitive_words::SHOW, [&](IP &ip) {
        stack.for_each([](iData thing) {
            print(thing.to_string(), " ");
        });
        println("<-top");
    });

    word_generator.register_primitive("'", primitive_words::TICK, [&](IP &ip) {
        std::string next_token = input.next_token();
        auto cfa = find(next_token);
        if (cfa != nullptr)
            stack.push(cfa);
    });

    word_generator.register_primitive(",", primitive_words::COMMA, [&](IP &ip) {
        if (immediate) {
            if (stack.top().is_number())
                dictionary.emplace_back(stack.top().as_number());
            else {
                println("COMMA pops a number from stack, but only XT was available");
            }
        } else if (dictionary.back().is_forth_word())
            dictionary.back().as_forth_word()->add(stack.pop());
        else {
            println("attempted to compile to a primitive");
        }
    });

    word_generator.register_primitive("see", primitive_words::SEE, [&](IP &ip) {
        std::cout << "\n\tDefinitions:\n";

        for (iData dict_data : dictionary) {
            if(!dict_data.is_forth_word()) continue;
            iWordptr word_pointer = dict_data.as_word();

            std::cout << std::setfill(' ') << std::setw(15) <<
                      word_pointer->name() + "  ";
            std::cout <<((word_pointer->immediate) ? "IMM  " : "     ");

            if (dict_data.is_forth_word())
                dict_data.as_forth_word()->definition_to_string();
            std::cout << std::endl;
        }
        std::cout << std::endl;
    });

    word_generator.register_lambda_word("[", primitive_words::TOIMMEDIATE, [&](IP &ip) {
        immediate = true;
    }, true);

    word_generator.register_primitive("]", primitive_words::TOCOMPILE, [&](IP &ip) {
        immediate = false;
    });

    word_generator.register_lambda_word("immediate", primitive_words::IMMEDIATE, [&](IP &ip) {
        if(!dictionary.back().is_forth_word()){
            println("tried to set immediate on number data");
            return;
        }
        dictionary.back().as_forth_word()->immediate = true;
    }, true);


    word_generator.register_primitive("allot", primitive_words::ALLOT, [&](IP &ip) {
        int num_to_allot = stack.pop_number();
        if(num_to_allot < 0)
            while(num_to_allot --> 0 || dictionary.back().is_number())
                dictionary.pop_back();
        else
            while(num_to_allot --> 0)
                dictionary.emplace_back(0);
    });

    word_generator.register_primitive("@", primitive_words::FETCH, [&](IP &ip) {
        stack.push(dictionary.at(stack.pop_number()));
    });

    word_generator.register_primitive("!", primitive_words::STORE, [&](IP &ip) {
        if(immediate){
            int address = stack.pop_number();
            iData val = stack.pop();
            dictionary.at(address) = val;
        }else{
            int address = stack.pop_number();
            iData val = stack.pop();

            if (!dictionary.back().is_forth_word())
                println("shit");
            else {
                dictionary.back().as_forth_word()->set(address, val);
            }
        }
    });

    word_generator.register_primitive("branch", primitive_words::BRANCH, [&](IP &ip) {
        ip += (ip+1)->as_number();
    }, true);

    word_generator.register_primitive("branchif", primitive_words::BRANCHIF, [&](IP &ip) {
        auto next_data = ip+1;
        if (stack.pop_number() == 0)
            ip += next_data->as_number();
        else
            ip++;
    }, true);

    word_generator.register_primitive("literal", primitive_words::LITERAL, [&](IP &ip) {
        stack.push(*(++ip));
    }, true);

    // Use HERE for relative computations only
    // Does not represent specific address
    word_generator.register_primitive("here", primitive_words::HERE, [&](IP &ip) {
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

    word_generator.register_primitive("create", primitive_words::CREATE, [&](IP &ip) {
        std::string next_token = input.next_token();
        //dln("    consume ", next_token);
        dictionary.emplace_back(new ForthWord(next_token, false));
    });


    auto colon_word = new ForthWord(":", false);
    colon_word->add(iData(find("create")));
    colon_word->add(iData(find("]")));
    dictionary.emplace_back(colon_word);

    auto exit_word = new ForthWord(";", true);
    exit_word->add(iData(find("[")));
    dictionary.emplace_back(exit_word);
}

