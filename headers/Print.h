
#include <iostream>

#ifndef MOVFORTH_PRINT_H
#define MOVFORTH_PRINT_H

inline void print(){
    std::cout << std::endl;
}

template <typename Arg, typename... Args>
inline void print(Arg&& arg, Args&&... args)
{
    std::cout << std::forward<Arg>(arg);
    ((std::cout << ' ' << std::forward<Args>(args)), ...);
}

template <typename Arg, typename... Args>
inline void println(Arg&& arg, Args&&... args){
    print(arg, args...);
    print("\n");
    std::cout.flush();
}
template <typename Arg, typename... Args>
inline void dln(Arg&& arg, Args&&... args){
    if(false)
    {
        print(arg, args...);
        print("\n");
        std::cout.flush();
    }
}

#endif