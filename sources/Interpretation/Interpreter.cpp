#include <algorithm>
#include <iomanip>
#include "../../headers/Interpretation/Interpreter.h"
#include "../../headers/Print.h"

using namespace mfc;

Interpreter::Interpreter() : input("../boot.fs"){
    init_words();

    std::string token;
    while (!(token = input.next_token()).empty()){
        Wordptr Wordptr = find(token);
        if(!Wordptr)
            Wordptr = wordGenerator.get(token);

        if(Wordptr == nullptr){
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
                        forth_word->add(Data(find("literal")));
                        forth_word->add(Data(num));
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
            if(Wordptr->immediate || immediate) {
                auto dummy_ip = IP();
                //dln("execute word ", Wordptr->base_string());
                Wordptr->execute(stack, dummy_ip);
            } else {
                auto forth_word =
                        try_cast<ForthWord>(dictionary.back());

                if(forth_word){
                    //dln("compile FW ", Wordptr->base_string());
                    forth_word->add(Data(Wordptr));
                }else
                    println("attempted to compile xts to a primitive word");
            }
        }

    }
}

Wordptr Interpreter::find(const std::string& name) {
    auto find_result = std::find_if(dictionary.begin(), dictionary.end(),
                                    [name](Wordptr other){return other->base_string() == name;});

    if(find_result == dictionary.end())
        // try primitives
        return wordGenerator.get(name);

    return *find_result;
}


void Interpreter::init_words(){

    wordGenerator.register_primitive("+", [](Stack &s, Data data, IP &i) {
        s.push(s.pop_number() + s.pop_number());
    });

    wordGenerator.register_primitive("-", [](Stack &s, Data data, IP &i) {
        auto one = s.pop_number();
        auto two = s.pop_number();
        s.push(two - one);

    });

    wordGenerator.register_primitive("*", [](Stack &s, Data data, IP &i) {
        s.push(s.pop_number() * s.pop_number());

    });

    wordGenerator.register_primitive("/", [](Stack &s, Data data, IP &i) {
        s.push(s.pop_number() / s.pop_number());
    });

    wordGenerator.register_primitive("swap", [](Stack &s, Data data, IP &i) {
        auto top = s.pop_number(), second = s.pop_number();
        s.push(top);
        s.push(second);
    });

    wordGenerator.register_primitive("rot", [](Stack &s, Data data, IP &i) {
        auto top = s.pop_number();
        auto second = s.pop_number();
        auto third = s.pop_number();

        s.push(second);// third
        s.push(top);   // second
        s.push(third); // top
    });

    wordGenerator.register_primitive("dup", [](Stack &s, Data data, IP &i) {
        s.push(s.top());
    });

    wordGenerator.register_primitive("drop", [](Stack &s, Data data, IP &i) {
        s.pop_number();
    });

    wordGenerator.register_primitive(".", [](Stack &s, Data data, IP &i) {
        println(s.pop_number());
    });

    wordGenerator.register_primitive(".S", [](Stack &s, Data data, IP &i) {
        s.for_each([](Data thing) {
            print(thing.to_string(), " ");
        });
        println("<-top");
    });

    wordGenerator.register_primitive("'", [&](Stack &s, Data data, IP &i) {
        std::string next_token = input.next_token();
        auto cfa = find(next_token);
        if (cfa != nullptr)
            s.push(cfa);
    });

    wordGenerator.register_primitive(",", [&](Stack &s, Data data, IP &i) {
        if (auto last_word = try_cast<ForthWord>(dictionary.back())) {
            last_word->add(s.pop());
        } else {
            println("attempted to compile to a primitive");
        }
    });

    wordGenerator.register_primitive("see", [&](Stack &s, Data data, IP &i) {
        println("\n\tSo you want to see?");

        for (Wordptr word_pointer : dictionary) {
            std::cout << std::setfill(' ') << std::setw(15) <<
                      word_pointer->to_string() + "  ";
            print((word_pointer->immediate) ? "IMM  " : "     ");

            if (auto forth_word = try_cast<ForthWord>(word_pointer))
                forth_word->definition_to_string();
            println();
        }
        println();
    });

    wordGenerator.register_lambda_word("[", [&](Stack &s, Data data, IP &i) {
        immediate = true;
    }, true);

    wordGenerator.register_primitive("]", [&](Stack &s, Data data, IP &i) {
        immediate = false;
    });

    wordGenerator.register_lambda_word("immediate", [&](Stack &s, Data data, IP &i) {
        dictionary.back()->immediate = true;
    }, true);

    wordGenerator.register_primitive("@", [&](Stack &s, Data data, IP &i) {
        dictionary.back()->immediate = true;
    });

    wordGenerator.register_primitive("!", [&](Stack &s, Data data, IP &i) {
        int address = stack.pop_number();
        Data val = stack.pop();
        auto last_word = try_cast<ForthWord>(dictionary.back());
        if (!last_word)
           println("used ! for an address not in most recent word");
        else {
            last_word->set(address, val);
        }
    });

    wordGenerator.register_primitive("branch", [&](Stack &s, Data data, IP &ip) {
        ip += data.as_num();
    }, true);

    wordGenerator.register_primitive("branchif", [&](Stack &s, Data data, IP &ip) {
        if (s.pop_number() == 0)
            ip += data.as_num();
    }, true);

    wordGenerator.register_primitive("literal", [&](Stack &s, Data data, IP &ip) {
        s.push(data.clone());
    }, true);

    // Use HERE for relative computations only
    // Does not represent specific address
    wordGenerator.register_primitive("here", [&](Stack &s, Data data, IP &i) {
        auto last_word = try_cast<ForthWord>(dictionary.back());
        if (!last_word)
            println("Define some words before calling HERE");
        else
            s.push(last_word->definition_size());
    });

    wordGenerator.register_primitive("create", [&](Stack &s, Data data, IP &i) {
        std::string next_token = input.next_token();
        //dln("    consume ", next_token);
        dictionary.push_back(new ForthWord(next_token, false));
    });


    auto colon_word = new ForthWord(":", false);
    colon_word->add(Data(find("create")));
    colon_word->add(Data(find("]")));
    dictionary.push_back(colon_word);

    auto exit_word = new ForthWord(";", true);
    exit_word->add(Data(find("[")));
    dictionary.push_back(exit_word);
}

std::vector<Wordptr> Interpreter::get_dictionary() {
    return std::move(dictionary);
}

