
#include <iostream>
#include <regex>

#ifndef MOVFORTH_PRINT_H
#define MOVFORTH_PRINT_H

static std::string color_begin = "\\033[1;32m\\n";
static std::string color_end = "\\033[0m\\n";

static int indents;

static std::string tab_string;

// Debug toggle - not the best, but works for now
static const bool debug = true;

std::string replace_indent(const std::string& in);

void regen_tab();

void indent();

void unindent();

template <typename Arg>
inline void print_single(Arg arg) {
    std::cout << arg;
}

template <typename... Args>
inline void print(Args&&... args)
{
    (print_single(std::forward<Args>(args)), ...);
}

template <typename... Args>
inline void println(Args&&... args){
    print(args...);
    std::cout << std::endl;
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

#endif