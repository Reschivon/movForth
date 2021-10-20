
#include <iostream>

#ifndef MOVFORTH_PRINT_H
#define MOVFORTH_PRINT_H

static std::string color_begin = "\\033[1;32m\\n";
static std::string color_end = "\\033[0m\\n";

extern int indents;

// Debug toggle - not the best, but works for now
static const bool debug = true;

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
    if(debug)
    {
        println(args...);
    }
}

template <typename... Args>
inline void d(Args&&... args){
    if(debug)
    {
        print(args...);
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