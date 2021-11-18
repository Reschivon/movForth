


#include "../../headers/Interpretation/iData.h"
#include "../../headers/Interpretation/ForthWord.h"
#include "../../headers/Interpretation/Primitive.h"

using namespace mov;


iData::iData(iData::dict_data_var_type data) : dict_data_var_type(data) {}

std::string iData::to_string() {
    if(is_word())
        return as_word()->name();
    if(is_number())
        return std::to_string(as_number());
    return "undef";
}

iData::iData() : iData::dict_data_var_type(nullptr) { }

bool iData::is_number() const { return index() == 0;}

bool iData::is_word() const { return index() == 1 || index() == 2 || index() == 3;}

bool iData::is_forth_word() const { return index() == 2;}

bool iData::is_primitive() const { return index() == 3;}

bool iData::is_empty() const { return index() == 4;}



element iData::as_number()      { return std::get<element>(*this);}

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

