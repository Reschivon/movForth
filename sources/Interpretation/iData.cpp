

#include "../../headers/Interpretation/iData.h"

using namespace mov;

int iData::as_number()      { return std::get<int>(*this);}

iWord* iData::as_word()
{
    if(is_forth_word())
        return std::get<ForthWord*>(*this);
    if(is_primitive())
        return std::get<Primitive*>(*this);
    if(is_number()) {
        println("Expected a word, got number");
        return nullptr;
    }
    return std::get<iWord*>(*this); // assume word if not FW
}
ForthWord* iData::as_forth_word() const  { return std::get<ForthWord*>(*this);}
Primitive* iData::as_primitive()   { return std::get<Primitive*>(*this);}

iData::iData(iData::dict_data_var_type data) : dict_data_var_type(data) {}

std::string iData::to_string() {
    if(is_word())
        return as_word()->name();
    if(is_number())
        return std::to_string(as_number());
    return "undef";
}

iData::iData() : iData::dict_data_var_type(nullptr) {}
