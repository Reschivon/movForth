

#ifndef MOVFORTH_IDATA_H
#define MOVFORTH_IDATA_H

#include <variant>
#include <string>
#include "../Constants.h"
#include "iWord.h"

namespace mov{

    struct iData : std::variant<element, iWord*, ForthWord*, Primitive*, std::nullptr_t>{
        using dict_data_var_type = std::variant<element, iWord*, ForthWord*, Primitive*, std::nullptr_t>;

        [[nodiscard]] bool is_number()      const{ return index() == 0;}
        [[nodiscard]] bool is_word()        const{ return index() == 1 || index() == 2 || index() == 3;}
        [[nodiscard]] bool is_forth_word()  const{ return index() == 2;}
        [[nodiscard]] bool is_primitive()   const{ return index() == 3;}
        [[nodiscard]] bool is_empty()       const{ return index() == 4;}

        element as_number();
        iWord* as_word();
        ForthWord* as_forth_word() const;
        Primitive* as_primitive();

        explicit iData(dict_data_var_type data);
        iData();

        std::string to_string();
    };
}

#endif //MOVFORTH_IDATA_H
