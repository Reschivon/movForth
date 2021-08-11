

#include "../../headers/Interpretation/iData.h"

using namespace mov;

int DictData::as_number()      { return std::get<int>(*this);}

iWord* DictData::as_word()
{
    if(is_forth_word())
        return std::get<ForthWord*>(*this);
    if(is_primitive())
        return std::get<Primitive*>(*this);
    return std::get<iWord*>(*this); // assume word if not FW
}
ForthWord* DictData::as_forth_word()  { return std::get<ForthWord*>(*this);}
Primitive* DictData::as_primitive()   { return std::get<Primitive*>(*this);}

DictData::DictData(DictData::dict_data_var_type data) : dict_data_var_type(data) {}

std::string DictData::to_string() {
    if(is_word())
        return as_word()->name();
    if(is_number())
        return std::to_string(as_number());
    return "undef";
}

DictData::DictData() : DictData::dict_data_var_type(nullptr) {}
