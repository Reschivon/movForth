

#include "../../headers/Interpretation/iData.h"
#include "../../headers/Interpretation/iWord.h"

using namespace mov;

iData iData::clone(){
    if(type() == iword)
        return iData(to_type<iword_t>()->clone());
    if(type() == number)
        return iData(to_type<number_t>());
    return iData(nullptr);
}

std::string iData::to_string(){
    if(type() == iword)
        return to_type<iword_t>()->to_string();
    if(type() == number)
        return std::to_string(to_type<number_t>());
    return "undef";
}
