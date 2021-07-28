
#include <iostream>

#ifndef MOVFORTH_PRINT_H
#define MOVFORTH_PRINT_H

extern int _indent;

template <typename... Args>
inline void print(Args&&... args)
{
    for(int i = 0; i < _indent; i++)
        std::cout <<"\t";

    ((std::cout << std::forward<Args>(args)), ...);
}

template <typename... Args>
inline void println(Args&&... args){
    print(args...);
    std::cout << std::endl;
}
template <typename... Args>
inline void dln(Args&&... args){
    if(true)
    {
        print(args...);
        std::cout << std::endl;
    }
}

inline void indent(){
    _indent++;
}
inline void unindent(){
    _indent--;
}

#endif