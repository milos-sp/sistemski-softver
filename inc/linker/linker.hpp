#ifndef LINKER_HPP_
#define LINKER_HPP_
#include "../section.hpp"
#include "../symbol.hpp"
#include "../relocation.hpp"
#include <sstream>
#include <vector>
#include <list>

using namespace std;

class Linker{
  private:
    list<string> inputs;
    list<Section*> sectionList;
    list<Symbol*> symbolList;
    list<RelocationSymbol*> relocationList;
  public:
    Linker(){}
    void addSection(Section s);
    Section* prepareSection(string s);
};

#endif