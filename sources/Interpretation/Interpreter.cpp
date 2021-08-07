#include <algorithm>
#include <iomanip>
#include <utility>
#include "../../headers/Interpretation/Interpreter.h"
#include "../../headers/Print.h"

using namespace mov;

Interpreter::Interpreter(std::string path) : input(std::move(path)){
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
                    auto forth_word =
                            try_cast<ForthWord>(dictionary.back());

                    if(forth_word){
                        //dln("compile number ", num);
                        forth_word->add(iData(find("literal")));
                        forth_word->add(iData(num));
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
                //dln("execute word ", Wordptr->base_string());
                iWordptr->execute(stack, dummy_ip);
            } else {
                auto forth_word = try_cast<ForthWord>(dictionary.back());

                if(forth_word){
                    //dln("compile FW ", Wordptr->base_string());
                    forth_word->add(iData(iWordptr));
                }else
                    println("attempted to compile xts to a primitive word");
            }
        }

    }
}

iWordptr Interpreter::find(const std::string& name) {
    auto find_result = std::find_if(dictionary.begin(), dictionary.end(),
                                    [name](iWordptr other){return other->base_string() == name;});

    if(find_result == dictionary.end())
        // try primitives
        return iWordGenerator.get(name);

    return *find_result;
}


void Interpreter::init_words(){

    iWordGenerator.register_primitive("+", [](Stack &s, iData data, IP &i) {
        s.push(s.pop_number() + s.pop_number());
    });

    iWordGenerator.register_primitive("-", [](Stack &s, iData data, IP &i) {
        auto one = s.pop_number();
        auto two = s.pop_number();
        s.push(two - one);

    });

    iWordGenerator.register_primitive("*", [](Stack &s, iData data, IP &i) {
        s.push(s.pop_number() * s.pop_number());

    });

    iWordGenerator.register_primitive("/", [](Stack &s, iData data, IP &i) {
        s.push(s.pop_number() / s.pop_number());
    });

    iWordGenerator.register_primitive("swap", [](Stack &s, iData data, IP &i) {
        auto top = s.pop_number(), second = s.pop_number();
        s.push(top);
        s.push(second);
    });

    iWordGenerator.register_primitive("rot", [](Stack &s, iData data, IP &i) {
        auto top = s.pop_number();
        auto second = s.pop_number();
        auto third = s.pop_number();

        s.push(second);// third
        s.push(top);   // second
        s.push(third); // top
    });

    iWordGenerator.register_primitive("dup", [](Stack &s, iData data, IP &i) {
        s.push(s.top());
    });

    iWordGenerator.register_primitive("drop", [](Stack &s, iData data, IP &i) {
        s.pop_number();
    });

    iWordGenerator.register_primitive(".", [](Stack &s, iData data, IP &i) {
        println(s.pop_number());
    });

    iWordGenerator.register_primitive(".S", [](Stack &s, iData data, IP &i) {
        s.for_each([](iData thing) {
            print(thing.to_string(), " ");
        });
        println("<-top");
    });

    iWordGenerator.register_primitive("'", [&](Stack &s, iData data, IP &i) {
        std::string next_token = input.next_token();
        auto cfa = find(next_token);
        if (cfa != nullptr)
            s.push(cfa);
    });

    iWordGenerator.register_primitive(",", [&](Stack &s, iData data, IP &i) {
        if (auto last_word = try_cast<ForthWord>(dictionary.back())) {
            last_word->add(s.pop());
        } else {
            println("attempted to compile to a primitive");
        }
    });

    iWordGenerator.register_primitive("see", [&](Stack &s, iData data, IP &i) {
        println("\n\tSo you want to see?");

        for (iWordptr word_pointer : dictionary) {
            std::cout << std::setfill(' ') << std::setw(15) <<
                      word_pointer->to_string() + "  ";
            print((word_pointer->immediate) ? "IMM  " : "     ");

            if (auto forth_word = try_cast<ForthWord>(word_pointer))
                forth_word->definition_to_string();
            println();
        }
        println();
    });

    iWordGenerator.register_lambda_word("[", [&](Stack &s, iData data, IP &i) {
        immediate = true;
    }, true);

    iWordGenerator.register_primitive("]", [&](Stack &s, iData data, IP &i) {
        immediate = false;
    });

    iWordGenerator.register_lambda_word("immediate", [&](Stack &s, iData data, IP &i) {
        dictionary.back()->immediate = true;
    }, true);

    iWordGenerator.register_primitive("@", [&](Stack &s, iData data, IP &i) {
        dictionary.back()->immediate = true;
    });

    iWordGenerator.register_primitive("!", [&](Stack &s, iData data, IP &i) {
        int address = stack.pop_number();
        iData val = stack.pop();
        auto last_word = try_cast<ForthWord>(dictionary.back());
        if (!last_word)
           println("shit");
        else {
            last_word->set(address, val);
        }
    });

    iWordGenerator.register_primitive("branch", [&](Stack &s, iData data, IP &ip) {
        ip += (ip+1)->as_number();
    }, true);

    iWordGenerator.register_primitive("branchif", [&](Stack &s, iData data, IP &ip) {
        auto next_data = ip+1;
        if (s.pop_number() == 0)
            ip += next_data->as_number();
        else
            ip++;
    }, true);

    iWordGenerator.register_primitive("literal", [&](Stack &s, iData data, IP &ip) {
        s.push(*(++ip));
    }, true);

    // Use HERE for relative computations only
    // Does not represent specific address
    iWordGenerator.register_primitive("here", [&](Stack &s, iData data, IP &i) {
        auto last_word = try_cast<ForthWord>(dictionary.back());
        if (!last_word)
            println("Define some words before calling HERE");
        else
            s.push(last_word->definition_size());
    });

    iWordGenerator.register_primitive("create", [&](Stack &s, iData data, IP &i) {
        std::string next_token = input.next_token();
        //dln("    consume ", next_token);
        dictionary.push_back(new ForthWord(next_token, false));
    });


    auto colon_word = new ForthWord(":", false);
    colon_word->add(iData(find("create")));
    colon_word->add(iData(find("]")));
    dictionary.push_back(colon_word);

    auto exit_word = new ForthWord(";", true);
    exit_word->add(iData(find("[")));
    dictionary.push_back(exit_word);
}

