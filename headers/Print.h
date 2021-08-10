
#include <iostream>

#ifndef MOVFORTH_PRINT_H
#define MOVFORTH_PRINT_H

extern int indents;

template <typename... Args>
inline void print(Args&&... args)
{
    ((std::cout << std::forward<Args>(args)), ...);
}

template <typename... Args>
inline void println(Args&&... args){
    print(args...);
    std::cout << std::endl;
    for(int i = 0; i < indents; i++)
        std::cout <<"\t";
}
template <typename... Args>
inline void dln(Args&&... args){
    if(true) // Debug toggle - not the best, but works for now
    {
        println(args...);
    }
}

inline void indent(){
    indents++;
    for(int i = 0; i < indents; i++)
        std::cout <<"\t";
}
inline void unindent(){
    indents--;
    std::cout << std::endl;
    for(int i = 0; i < indents; i++)
        std::cout <<"\t";
}

#endif