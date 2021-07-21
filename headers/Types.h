

#ifndef MOVFORTH_TYPES_H
#define MOVFORTH_TYPES_H

namespace mfc {

    class Word;

    typedef Word *Wordptr;

    class Data {
        std::variant<int, Wordptr> data;
    public:
        bool is_num() {
            return data.index() == 0;
        }

        bool is_xt() {
            return data.index() == 1;
        }

        int as_num() {
            return std::get<int>(data);
        }

        Wordptr as_xt() {
            return std::get<Wordptr>(data);
        }

        Data(std::variant<int, Wordptr> data) : data(data) {}

        template<typename T>
        bool is_type(){
            if(!is_xt())
                return false;
            return nullptr != dynamic_cast<T>(as_xt());
        }

        template<typename T>
        T as_type(){
            if(!is_xt())
                return nullptr;
            return dynamic_cast<T>(as_xt());
        }
    };

}

#endif //MOVFORTH_TYPES_H
