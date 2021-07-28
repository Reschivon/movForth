
#ifndef MOVFORTH_UTIL_H
#define MOVFORTH_UTIL_H

template <typename T>
void append_to_vector(std::vector<T> base, std::vector<T> to_add){
    for(auto thing : to_add){
        base.push_back(thing);
    }
}

template <typename T>
T* copy_of(T *in){
  return new T(*in);
}

#endif //MOVFORTH_UTIL_H
