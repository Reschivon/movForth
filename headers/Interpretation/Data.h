

#ifndef MOVFORTH_DATA_H
#define MOVFORTH_DATA_H

#include <variant>
#include <string>

namespace mfc {
    class iWord;
    typedef iWord *iWordptr;

    struct Data : std::variant<std::nullptr_t, int, iWordptr>{
        using var_type = std::variant<std::nullptr_t, int, iWordptr>;

        const static int empty = 0, number = 1, iword = 2;

        typedef std::nullptr_t empty_t;
        typedef int number_t;
        typedef iWordptr iword_t;

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
