
#include <iostream>
#include <regex>

#ifndef MOVFORTH_PRINT_H
#define MOVFORTH_PRINT_H

extern std::string indents;

template <typename Arg>
inline void print_impl(Arg arg){
    std::cout << arg;
}
template <typename Arg>
inline void print_impl(std::string arg){
    std::cout << std::regex_replace(arg, std::regex("\n"), indents);
}


template <class ...Args>
void print(Args... args)
{
    ((print_impl(args)), ...);
}

template <typename... Args>
inline void println(Args&&... args){
    print(args..., "\n");
}

template <typename... Args>
inline void d(Args&&... args){
    if(true) // Debug toggle - not the best, but works for now
    {
        print(args...);
    }
}

template <typename... Args>
inline void dln(Args&&... args){
    if(true) // Debug toggle - not the best, but works for now
    {
        println(args...);
    }
}

inline void indent(){
    indents += "\t";
}
inline void unindent(){
    indents.pop_back();
}

#endif