#include <utility>


#ifndef MOVFORTH_LOCAL_H
#define MOVFORTH_LOCAL_H

struct Local {

    std::string word; std::string name;

    bool operator==(const Local &other) const {
        return name == name && other.word == word;
    }

    Local(std::string word, std::string name)
        : word(std::move(word)), name(std::move(name)){
    }

    std::string to_string() const{
        return "Local(" + word + " " + name + ")";
    }

};

struct LocalHasher
{
    std::size_t operator()(const Local& k) const
    {
        size_t res = 17;
        res = res * 31 + std::hash<std::string>()(k.word);
        res = res * 31 + std::hash<std::string>()(k.name);
        return res;
    }
};

#endif //MOVFORTH_LOCAL_H
