

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
    };



    class IP{
        bool isActive = false;
    public:
        std::vector<Data>::iterator me;

        IP(std::vector<Data>::iterator in) : me(in) {isActive = true;}
        IP() : me(nullptr) {}
        IP operator+=(int i) { //outside the class
            if(isActive) {
                return me += i;
            }
            return me;
        }
        bool operator<(const std::vector<Data>::iterator& a){
            return me < a;
        }
    };

}

#endif //MOVFORTH_TYPES_H
