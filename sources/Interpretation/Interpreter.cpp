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
                //dln("execute word ", Wordptr->_name());
                iWordptr->execute(stack, dummy_ip);
            } else {
                auto forth_word = try_cast<ForthWord>(dictionary.back());

                if(forth_word){
                    //dln("compile FW ", Wordptr->_name());
                    forth_word->add(iData(iWordptr));
                }else
                    println("attempted to compile xts to a primitive word");
            }
        }

    }
}

iWordptr Interpreter::find(const std::string& name) {
    auto find_result = std::find_if(dictionary.rbegin(), dictionary.rend(),
                                    [name](iWordptr other){return other->name() == name;});

    if(find_result == dictionary.rend())
        // try primitives
        return iWordGenerator.get(name);

    return *find_result;
}


void Interpreter::init_words(){

    iWordGenerator.register_primitive("+", primitive_words::ADD, [](Stack &s, IP &i) {
        s.push(s.pop_number() + s.pop_number());
    });

    iWordGenerator.register_primitive("-", primitive_words::SUBTRACT, [](Stack &s, IP &i) {
        auto one = s.pop_number();
        auto two = s.pop_number();
        s.push(two - one);

    });

    iWordGenerator.register_primitive("*", primitive_words::MULTIPLY, [](Stack &s, IP &i) {
        s.push(s.pop_number() * s.pop_number());

    });

    iWordGenerator.register_primitive("/", primitive_words::DIVIDE, [](Stack &s, IP &i) {
        s.push(s.pop_number() / s.pop_number());
    });

    iWordGenerator.register_primitive("swap", primitive_words::SWAP, [](Stack &s, IP &i) {
        auto top = s.pop_number(), second = s.pop_number();
        s.push(top);
        s.push(second);
    });

    iWordGenerator.register_primitive("rot", primitive_words::ROT, [](Stack &s, IP &i) {
        auto top = s.pop_number();
        auto second = s.pop_number();
        auto third = s.pop_number();

        s.push(second);// third
        s.push(top);   // second
        s.push(third); // top
    });

    iWordGenerator.register_primitive("dup", primitive_words::DUP, [](Stack &s, IP &i) {
        s.push(s.top());
    });

    iWordGenerator.register_primitive("drop", primitive_words::DROP, [](Stack &s, IP &i) {
        s.pop_number();
    });

    iWordGenerator.register_primitive(".", primitive_words::EMIT, [](Stack &s, IP &i) {
        println(s.pop_number());
    });

    iWordGenerator.register_primitive(".S", primitive_words::SHOW, [](Stack &s, IP &i) {
        s.for_each([](iData thing) {
            print(thing.to_string(), " ");
        });
        println("<-top");
    });

    iWordGenerator.register_primitive("'", primitive_words::TICK, [&](Stack &s, IP &i) {
        std::string next_token = input.next_token();
        auto cfa = find(next_token);
        if (cfa != nullptr)
            s.push(cfa);
    });

    iWordGenerator.register_primitive(",", primitive_words::COMMA, [&](Stack &s, IP &i) {
        if (auto last_word = try_cast<ForthWord>(dictionary.back())) {
            last_word->add(s.pop());
        } else {
            println("attempted to compile to a primitive");
        }
    });

    iWordGenerator.register_primitive("see", primitive_words::SEE, [&](Stack &s, IP &i) {
        println("\n\tSo you want to see?");

        for (iWordptr word_pointer : dictionary) {
            std::cout << std::setfill(' ') << std::setw(15) <<
                      word_pointer->name() + "  ";
            print((word_pointer->immediate) ? "IMM  " : "     ");

            if (auto forth_word = try_cast<ForthWord>(word_pointer))
                forth_word->definition_to_string();
            println();
        }
        println();
    });

    iWordGenerator.register_lambda_word("[", primitive_words::TOIMMEDIATE, [&](Stack &s, IP &i) {
        immediate = true;
    }, true);

    iWordGenerator.register_primitive("]", primitive_words::TOCOMPILE, [&](Stack &s, IP &i) {
        immediate = false;
    });

    iWordGenerator.register_lambda_word("immediate", primitive_words::IMMEDIATE, [&](Stack &s, IP &i) {
        dictionary.back()->immediate = true;
    }, true);

    iWordGenerator.register_primitive("@", primitive_words::FETCH, [&](Stack &s, IP &i) {
        dictionary.back()->immediate = true;
    });

    iWordGenerator.register_primitive("!", primitive_words::STORE, [&](Stack &s, IP &i) {
        int address = stack.pop_number();
        iData val = stack.pop();
        auto last_word = try_cast<ForthWord>(dictionary.back());
        if (!last_word)
           println("shit");
        else {
            last_word->set(address, val);
        }
    });

    iWordGenerator.register_primitive("branch", primitive_words::BRANCH, [&](Stack &s, IP &ip) {
        ip += (ip+1)->as_number();
    }, true);

    iWordGenerator.register_primitive("branchif", primitive_words::BRANCHIF, [&](Stack &s, IP &ip) {
        auto next_data = ip+1;
        if (s.pop_number() == 0)
            ip += next_data->as_number();
        else
            ip++;
    }, true);

    iWordGenerator.register_primitive("literal", primitive_words::LITERAL, [&](Stack &s, IP &ip) {
        s.push(*(++ip));
    }, true);

    // Use HERE for relative computations only
    // Does not represent specific address
    iWordGenerator.register_primitive("here", primitive_words::HERE, [&](Stack &s, IP &i) {
        auto last_word = try_cast<ForthWord>(dictionary.back());
        if (!last_word)
            println("Define some words before calling HERE");
        else
            s.push(last_word->def().size());
    });

    iWordGenerator.register_primitive("create", primitive_words::CREATE, [&](Stack &s, IP &i) {
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

