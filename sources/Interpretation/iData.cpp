

#include "../../headers/Interpretation/iData.h"
#include "../../headers/Interpretation/iWord.h"

using namespace mov;

iData iData::clone(){
    if(is_word())
        return iData(as_word()->clone());
    if(is_number())
        return iData(as_number());
    return iData(nullptr);
}

std::string iData::to_string(){
    if(is_word())
        return as_word()->to_string();
    if(is_number())
        return std::to_string(as_number());
    return "undef";
}
