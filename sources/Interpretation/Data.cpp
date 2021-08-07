

#include "../../headers/Interpretation/Data.h"
#include "../../headers/Interpretation/iWord.h"

using namespace mfc;

Data Data::clone(){
    if(type() == iword)
        return Data(to_type<iword_t>()->clone());
    if(type() == number)
        return Data(to_type<number_t>());
    return Data(nullptr);
}

std::string Data::to_string(){
    if(type() == iword)
        return to_type<iword_t>()->to_string();
    if(type() == number)
        return std::to_string(to_type<number_t>());
    return "undef";
}
