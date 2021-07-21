#include <algorithm>
#include <iomanip>
#include "../headers/Interpreter.h"
#include "../headers/Word.h"

using namespace mfc;

Interpreter::Interpreter() : input("../boot.fs"){
    init_words();

    std::string token;
    while (!(token = input.next_token()).empty()){
        Wordptr Wordptr = find(token);
        if(!Wordptr)
            Wordptr = wordGenerator.get(token);

        std::cout << "[interpreter] " << token << std::endl;

        if(Wordptr == nullptr){
            // might be a number
            try{
                int num = stoi(token);

                if(immediate)
                    stack.push(num);
                else {
                    if(auto forth_word = instance_of<ForthWord>(dictionary.back())){
                        forth_word->append(Data(find("literal")));
                        forth_word->append(Data(num));
                    }else{
                        std::cout << "attempted to compile LITERAL to a primitive word" << std::endl;
                    }

                }

            } catch (...) {
                // not a number or word
                std::cerr << "word " << token << " not found and not number" << std::endl;
            }

        }else{
            // it's a word
            if(Wordptr->immediate || immediate) {
                auto ippp = IP();
                Wordptr->execute(stack, ippp);
            } else {
                if(auto forth_word = instance_of<ForthWord>(dictionary.back())){
                    forth_word->append(Data(Wordptr));
                }else
                    std::cout << "attempted to compile xts to a primitive word"
                              << std::endl;
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
    // holy fuck what I am I doing here
    wordGenerator.register_lambda_word("+", [](Stack &stack, IP &i) {
        stack.push(stack.pop_number() + stack.pop_number());
    });

    wordGenerator.register_lambda_word("-", [](Stack &stack, IP &i) {
        auto one = stack.pop_number();
        auto two = stack.pop_number();
        stack.push(two - one);
        
    });


    wordGenerator.register_lambda_word("*", [](Stack &stack, IP &i) {
        stack.push(stack.pop_number() * stack.pop_number());
        
    });


    wordGenerator.register_lambda_word("/", [](Stack &stack, IP &i) {
        stack.push(stack.pop_number() / stack.pop_number());
        
    });

    wordGenerator.register_lambda_word("swap", [](Stack &stack, IP &i) {
        auto top = stack.pop_number(), second = stack.pop_number();
        stack.push(top);
        stack.push(second);
        
    });

    wordGenerator.register_lambda_word("dup", [](Stack &stack, IP &i) {
        stack.push(stack.top());
        
    });

    wordGenerator.register_lambda_word("drop", [](Stack &stack, IP &i) {
        stack.pop_number();
        
    });

    wordGenerator.register_lambda_word(".", [](Stack &stack, IP &i) {
        std::cout << stack.pop_number() << std::endl;
        
    });

    wordGenerator.register_lambda_word(".S", [](Stack &stack, IP &i) {
       stack.for_each([](Data thing) {
           std::cout << data_to_string(thing) << std::endl;
       });
        std::cout << "<-top";
        std::cout << std::endl;
        
    });

    wordGenerator.register_lambda_word("'", [&](Stack &stack, IP &i) {
        std::string next_token = input.next_token();
        auto cfa = find(next_token);
        if(cfa != nullptr)
            stack.push(cfa);
        
    });

    wordGenerator.register_lambda_word(",", [&](Stack &stack, IP &i) {
        if(auto last_word = instance_of<ForthWord>(dictionary.back())) {
            if(stack.top().is_num()) // is int
                last_word->append(Data(stack.pop_number()));
            if(stack.top().is_xt()) {
                //say "appending word " << stack.top_CFA()->name_str over // << " to " << last_word->name_str over
                last_word->append(Data(stack.pop_word_pointer()));
            }
        }else{
            std::cout << "attempted to compile to a primitive" << std::endl;
        }
        
    });

    wordGenerator.register_lambda_word("see", [&](Stack &stack, IP &i) {
        std::cout << std::endl << "\tSo you want to see?" << std::endl;

        for(const Wordptr word_pointer : dictionary){
            std::cout << std::setfill(' ') << std::setw(15) << word_pointer->to_string() + "  ";
            std::cout << ((word_pointer->immediate) ? "IMM " : "    ");

            if(auto forth_word = instance_of<ForthWord>(word_pointer))
                forth_word->print_definition();
            std::cout << std::endl;
        }

        std::cout << std::endl;
        
    });

    wordGenerator.register_lambda_word("word", [&](Stack &stack, IP &i) {
        token_buffer = input.next_token();
        
    });

    wordGenerator.register_lambda_word("[", [&](Stack &stack, IP &i) {
        immediate = true;
        
    });

    wordGenerator.register_lambda_word("]", [&](Stack &stack, IP &i) {
        immediate = false;
        
    });

    wordGenerator.register_lambda_word("immediate", true, [&](Stack &stack, IP &i) {
        dictionary.back()->immediate = true;
        
    });

    wordGenerator.register_type<Branch>("branch");
    wordGenerator.register_type<BranchIf>("branchif");

    wordGenerator.register_type<Literal>("literal");


    // relative arithmetic ONLY. Spec does not guarantee it
    // points to a specific address
    // TODO HERE for variables
    wordGenerator.register_lambda_word("here", [&](Stack &stack, IP &i) {
        if(auto last_word = instance_of<ForthWord>(dictionary.back()))
            std::cout << "attempted to use HERE for purpose other than compilation" <<
                      "This is not supported since it destroys the illusion that you" <<
                      "are using an interpreted Forth" << std::endl;
        else
            stack.push(last_word->definition_size());
        
    });

    wordGenerator.register_lambda_word("create", [&](Stack &stack, IP &i) {
        std::string next_token = input.next_token();
        dictionary.push_back(new ForthWord(next_token, false));
    });

    auto colon_word = new ForthWord(":", false);
    colon_word->append(Data(find("create")));
    colon_word->append(Data(find("]")));
    dictionary.push_back(colon_word);

    auto exit_word = new ForthWord(";", true);
    exit_word->append(Data(find("[")));
    dictionary.push_back(exit_word);
}

