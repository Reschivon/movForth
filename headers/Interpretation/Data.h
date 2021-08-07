

#ifndef MOVFORTH_DATA_H
#define MOVFORTH_DATA_H

#include <variant>
#include <string>

namespace mfc {
    class Word;
    typedef Word *Wordptr;

    struct Data : std::variant<std::nullptr_t, int, Wordptr>{
        using var_type = std::variant<std::nullptr_t, int, Wordptr>;

        const static int empty = 0, number = 1, word = 2;

        typedef std::nullptr_t empty_t;
        typedef int number_t;
        typedef Wordptr word_t;

        explicit Data(var_type data) : var_type(data) {}
        Data() : var_type(nullptr) {}

        int type(){
            return var_type::index();
        }

        template<typename T>
        T to_type() { return std::get<T>(*this); }

        std::string to_string();
        Data clone();
    };
}
#endif //MOVFORTH_DATA_H
