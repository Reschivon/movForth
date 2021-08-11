

#ifndef MOVFORTH_IDATA_H
#define MOVFORTH_IDATA_H

#include <variant>
#include <string>
#include "../Print.h"
/*

namespace mov {
    class iWord;
    typedef iWord *iWordptr;

    */
/**
     * @brief Holds all types of data found within a word's definition (words and numbers)
     *//*

    struct iData : std::variant<std::nullptr_t, int, iWordptr>{
        using var_type = std::variant<std::nullptr_t, int, iWordptr>;

        */
/**
         * Makes a data object from a parameter of specific type
         * @param data can be an iWordptr, an int, or a nullptr signifying emptiness
         *//*

        explicit iData(var_type data) : var_type(data) {}
        iData() : var_type(nullptr) {}

        */
/**
         * @return Whether the Data object is holding a number
         *//*

        bool is_number(){
            return index() == 1;
        }

        */
/**
         * @return Whether the Data object is holding a word pointer
         *//*

        bool is_word(){
            return index() == 2;
        }

        */
/**
         * @return Whether the Data object is empty
         *//*

        bool is_empty(){
            return index() == 0;
        }

        */
/**
         * @return underlying int, or 0 if this Data does not hold number
         *//*

        int as_number(){
            try {
                return std::get<int>(*this);
            }catch (...){
                println("tried extract variant of type " , index() , " as num");
                return 0;
            }
        }
        */
/**
         * @return underlying iWordptr, or nullptr if this Data does not hold word
         *//*

        iWordptr as_word(){
            try {
                return std::get<iWordptr>(*this);
            }catch (...){
                println("tried extract variant of type " , index() , " as word");
                return nullptr;
            }
        }

        */
/**
         * @return string form of this Data, regardless of underlying type
         *//*

        std::string to_string();
    };
}
 */

#endif //MOVFORTH_IDATA_H

