#include <algorithm>
#include <iomanip>
#include <utility>

#include "../../headers/Interpretation/Interpreter.h"

#include "../../headers/Interpretation/ForthWord.h"
#include "../../headers/Interpretation/Primitive.h"
#include "../../headers/Interpretation/WordGenerator.h"
#include "../../headers/Interpretation/Stack.h"

using namespace mov;

Interpreter::Interpreter(const std::string& path)
        : input(path), word_generator(*new iWordGenerator()), stack(*new Stack()){
    init_words();
}

bool Interpreter::interpret() {
    dln("========[Interpretation]======");
    std::cout << std::endl;

    std::string token;
    while (true){
        // get next token
        token = input.next_token();
        if(!input.open()) break;

        // try to get primitive word for token
        iWordptr wordptr = find(token);

        // try to get Forth word for token
        if(!wordptr)
            wordptr = word_generator.get(token);

        // save for convenience
        ForthWord *latest_forth_word = nullptr;
        if(dictionary.back().is_forth_word())
            latest_forth_word = dictionary.back().as_forth_word();

        if(wordptr == nullptr){
            // might be a number or Local

            if(latest_forth_word != nullptr){
                auto &locals = latest_forth_word->locals;
                Local local = Local(latest_forth_word->name(), token);

                if(locals.find(local) != locals.end()) {
                    // compile a FromLocal
                    latest_forth_word->add(iData(new FromLocal(local, latest_forth_word->locals)));
                    continue;
                }
            }

            try{
                int num = stoi(token);

                if(immediate)
                    stack.push(num);
                else {
                    if(latest_forth_word != nullptr){
                        //dln("compile number ", num);
                        latest_forth_word->add(iData(find("literal")));
                        latest_forth_word->add(iData(num));
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
            // is a word
            if(wordptr->immediate || immediate) {
               // dln("execute word ", wordptr->name());
                auto dummy = (new std::list<iData>())->begin();
                wordptr->execute(dummy, *this);
            } else {
                if(latest_forth_word != nullptr){
                    // dln("compile FW ", wordptr->name());
                    latest_forth_word->add(iData(wordptr));
                }else {
                    println("attempted to compile xts to a primitive word");
                    return true;
                }
            }
        }

    }

    dln("======[End Interpretation]======");
    std::cout << std::endl;
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

    word_generator.register_primitive("=", primitive_words::EQUALS, [&](IP ip, Interpreter &interpreter) {
        stack.push(stack.pop_number() == stack.pop_number());
    });

    word_generator.register_primitive("+", primitive_words::ADD, [&](IP ip, Interpreter &interpreter) {
        stack.push(stack.pop_number() + stack.pop_number());
    });

    word_generator.register_primitive("-", primitive_words::SUBTRACT, [&](IP ip, Interpreter &interpreter) {
        auto one = stack.pop_number();
        auto two = stack.pop_number();
        stack.push(two - one);

    });

    word_generator.register_primitive("*", primitive_words::MULTIPLY, [&](IP ip, Interpreter &interpreter) {
        stack.push(stack.pop_number() * stack.pop_number());

    });

    word_generator.register_primitive("/", primitive_words::DIVIDE, [&](IP ip, Interpreter &interpreter) {
        stack.push(stack.pop_number() / stack.pop_number());
    });

    word_generator.register_primitive("swap", primitive_words::SWAP, [&](IP ip, Interpreter &interpreter) {
        auto top = stack.pop_number(), second = stack.pop_number();
        stack.push(top);
        stack.push(second);
    });

    word_generator.register_primitive("rot", primitive_words::ROT, [&](IP ip, Interpreter &interpreter) {
        auto top = stack.pop_number();
        auto second = stack.pop_number();
        auto third = stack.pop_number();

        stack.push(second);// third
        stack.push(top);   // second
        stack.push(third); // top
    });

    word_generator.register_primitive("dup", primitive_words::DUP, [&](IP ip, Interpreter &interpreter) {
        stack.push(stack.top());
    });

    word_generator.register_primitive("drop", primitive_words::DROP, [&](IP ip, Interpreter &interpreter) {
        stack.pop_number();
    });

    word_generator.register_primitive(".", primitive_words::EMIT, [&](IP ip, Interpreter &interpreter) {
        println(stack.pop_number());
    });

    word_generator.register_primitive(".S", primitive_words::SHOW, [&](IP ip, Interpreter &interpreter) {
        stack.for_each([](iData thing) {
            print(thing.to_string(), " ");
        });
        println("<-top");
    });

    word_generator.register_primitive("'", primitive_words::TICK, [&](IP ip, Interpreter &interpreter) {
        std::string next_token = input.next_token();
        auto cfa = find(next_token);
        if (cfa != nullptr)
            stack.push(cfa);
    });

    word_generator.register_primitive(",", primitive_words::COMMA, [&](IP ip, Interpreter &interpreter) {
        if (immediate) {
            // Can't remember what this for
            // it's execution time stuff
            println("Compiling ", stack.top().to_string(), " to word ", dictionary.back().to_string());
            dictionary.back().as_forth_word()->add(stack.pop());

        } else if (dictionary.back().is_forth_word()) {
            // compiling time stuff
            //println("Compiling ", stack.top().to_string(), " to word ", dictionary.back().as_forth_word()->name());

            dictionary.back().as_forth_word()->add(stack.pop());
        } else {
            println("attempted to compile to a primitive");
        }
    });

    word_generator.register_primitive("see", primitive_words::SEE, [&](IP ip, Interpreter &interpreter) {
        std::cout << "\n\tDefinitions:\n";

        for (iData dict_data : dictionary) {
            // get the word pointer
            if(!dict_data.is_forth_word()) continue;
            iWordptr word_pointer = dict_data.as_word();

            bool is_forth_word = dict_data.is_forth_word();
            ForthWord *as_forth_word = nullptr;
            if(is_forth_word)
                as_forth_word = dict_data.as_forth_word();

            // print name with tabular indentation
            std::cout << std::setfill(' ') << std::setw(15) <<
                      word_pointer->name() + "  ";
            // print flags
            std::cout <<((word_pointer->immediate) ? "IMM  " : "     ");

            // print definition
            if (is_forth_word)
                as_forth_word->definition_to_string();
            std::cout << std::endl;

            // print locals
            std::cout << std::string(15, ' ');
            if (is_forth_word)
                for(const auto& [local, value] : as_forth_word->locals)
                    std::cout << local.name << " ";

            std::cout << std::endl;
        }
        std::cout << std::endl;
    });

    word_generator.register_lambda_word("[", primitive_words::TOIMMEDIATE, [&](IP ip, Interpreter &interpreter) {
        immediate = true;
    }, true);

    word_generator.register_primitive("]", primitive_words::TOCOMPILE, [&](IP ip, Interpreter &interpreter) {
        immediate = false;
    });

    word_generator.register_lambda_word("immediate", primitive_words::IMMEDIATE, [&](IP ip, Interpreter &interpreter) {
        if(!dictionary.back().is_forth_word()){
            println("tried to set immediate on number data");
            return;
        }
        dictionary.back().as_forth_word()->immediate = true;
    }, true);


    word_generator.register_primitive("allot", primitive_words::ALLOT, [&](IP ip, Interpreter &interpreter) {
        element num_to_allot = stack.pop_number();
        if(num_to_allot < 0)
            while(num_to_allot --> 0 || dictionary.back().is_number())
                dictionary.pop_back();
        else
            while(num_to_allot --> 0)
                dictionary.emplace_back(0);
    });

    word_generator.register_primitive("@", primitive_words::FETCH, [&](IP ip, Interpreter &interpreter) {
        stack.push(*(element*)stack.pop_number());
    });

    word_generator.register_primitive("!", primitive_words::STORE, [&](IP ip, Interpreter &interpreter) {
        if(immediate){
            // words being executed, no point in setting xts since nothing
            // is in state of half-compilation. User must be setting memory
            element ptr = stack.pop_number();
            element value = stack.pop_number();

            auto *addr = (element*) ptr;
            *addr = value;
        }else{
            // words being compiled, very bad time for setting dictionary memory right now
            // therefore we can assume user wants to set xts
            if (!dictionary.back().is_forth_word())
                println("Define some words before calling HERE");
            else {
                element location = stack.pop_number();
                iData value = stack.pop();

                auto it = dictionary.back().as_forth_word()->def().begin();
                std::advance(it, location);
                *it = value;

                println("Set index ", location, " of word ", dictionary.back().to_string(), " to ", value.to_string());
            }
        }

    });


    word_generator.register_primitive("branch", primitive_words::BRANCH, [&](IP ip, Interpreter &interpreter) {
        std::advance(ip, std::next(ip)->as_number());
    }, true);

    word_generator.register_primitive("branchif", primitive_words::BRANCHIF, [&](IP ip, Interpreter &interpreter) {
        auto next_data = std::next(ip);
        if (stack.pop_number() == 0)
            std::advance(ip, next_data->as_number());
        else
            ip++;
    }, true);

    word_generator.register_primitive("literal", primitive_words::LITERAL, [&](IP ip, Interpreter &interpreter) {
        stack.push(*(++ip));
    }, true);

    // Use HERE for relative computations only
    // Does not represent specific address
    word_generator.register_primitive("here", primitive_words::HERE, [&](IP ip, Interpreter &interpreter) {
        if(immediate){
            // words being executed, no point in compiling branches since nothing
            // is in state of half-compilation. Prime time for ALLOTing
            dictionary.emplace_back(0);
        }else{
            // words being compiled, very bad time for ALLOT right now
            // therefore we can assume user wants to mark positions for BRANCH
            if (!dictionary.back().is_forth_word())
                println("Define some words before calling HERE");
            else
                stack.push((element) dictionary.back().as_forth_word()->def().size());
        }
    });

    word_generator.register_primitive("create", primitive_words::CREATE, [&](IP ip, Interpreter &interpreter) {
        std::string next_token = input.next_token();
        dictionary.emplace_back(new ForthWord(next_token, false));
    });

    word_generator.register_primitive("malloc", primitive_words::MALLOC, [&](IP ip, Interpreter &interpreter) {
        element size_to_alloc = stack.pop_number();
        auto* ptr = (element*) malloc(size_to_alloc);
        stack.push((element) ptr);
    });

    word_generator.register_primitive("free", primitive_words::FREE, [&](IP ip, Interpreter &interpreter) {
        element mem_pointer = stack.pop_number();
        free((void*) mem_pointer);
    });

    word_generator.register_primitive("&", primitive_words::AND, [&](IP ip, Interpreter &interpreter) {
        stack.push(stack.pop_number() | stack.pop_number());
    });

    word_generator.register_primitive("<", primitive_words::LESS, [&](IP ip, Interpreter &interpreter) {
        stack.push(stack.pop_number() > stack.pop_number());
    });

    word_generator.generator_lookup["to"] = [](){
        return new class TO();
    };


    // tolocal and fromlocal are never generated by the user. So they will not be registered here

    auto colon_word = new ForthWord(":", false);
    colon_word->add(iData(find("create")));
    colon_word->add(iData(find("]")));
    dictionary.emplace_back(colon_word);

    auto exit_word = new ForthWord(";", true);
    exit_word->add(iData(find("[")));
    dictionary.emplace_back(exit_word);
}

