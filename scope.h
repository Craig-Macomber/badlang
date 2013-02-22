#pragma once

#include <string>
#include <map>


class ScopeEntry{
public:
    ScopeEntry();
};


class Scope final{
public:
    ~Scope();
    Scope(Scope const *next);
    ScopeEntry* Resolve(const std::string &key) const;
    void Add(const std::string &key, ScopeEntry* entry);
private:
    const Scope * const Next;
    std::map<const std::string, ScopeEntry*> Entries;
};

extern Scope emptyScope;

