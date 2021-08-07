

#include "../../headers/Interpretation/Data.h"
#include "../../headers/Interpretation/Word.h"

using namespace mfc;

Data Data::clone(){
    if(type() == word)
        return Data(to_type<word_t>()->clone());
    if(type() == number)
        return Data(to_type<number_t>());
    return Data(nullptr);
}

std::string Data::to_string(){
    if(type() == word)
        return to_type<word_t>()->to_string();
    if(type() == number)
        return std::to_string(to_type<number_t>());
    return "undef";
}
