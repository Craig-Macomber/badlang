#pragma once

#include <string>
#include <map>

class TypedValue;
class DeclarationASTNode;

class ScopeEntry final{
public:
    ~ScopeEntry();
    ScopeEntry(const DeclarationASTNode *declaration);
    const DeclarationASTNode *Declaration;
    TypedValue *Variable;
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

