#include "scope.h"

ScopeEntry::ScopeEntry(){}

Scope::~Scope(){
    // TODO : maybe delete entries?
}
Scope::Scope(Scope const *next):Next(next),Entries() {}
ScopeEntry* Scope::Resolve(const std::string &key) const{
    std::map<const std::string, ScopeEntry*>::const_iterator  it = Entries.find(key);
    if (it!=Entries.end()) {
        return it->second;
    }
    if (Next==NULL) {
        return NULL;
    }
    return Next->Resolve(key);
}

Scope emptyScope=Scope(NULL);

void Scope::Add(const std::string &key, ScopeEntry* entry){
    std::map<const std::string, ScopeEntry*>::const_iterator  it = Entries.find(key);
    if (it!=Entries.end()) {
        throw "Duplicate declaration of "+key;
    }
    Entries[key]=entry;
}