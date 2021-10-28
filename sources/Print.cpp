

#include "../headers/Print.h"

std::string replace_indent(const std::string &in) {
    return std::regex_replace(in, std::regex("\n"), tab_string);
}

template<> void
print_single<std::string>(const std::string arg) {
    std::cout << replace_indent(arg);
}

void regen_tab() {
    for(int i = 0; i < indents; i++)
        tab_string += "\t";
}

void unindent() {
    indents--;
    regen_tab();
}

void indent() {
    indents++;
    regen_tab();
}
