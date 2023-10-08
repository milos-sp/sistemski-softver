#ifndef RELOCATION_HPP_
#define RELOCATION_HPP_
#include <iostream>
#include "./symbol.hpp"

using namespace std;

class RelocationSymbol{
  private:
    int offset;
    string type;
    Symbol* sym;
    int addend;

  public:
    RelocationSymbol(int o, string t, Symbol* s, int a);
    friend ostream& operator <<(ostream& os, const RelocationSymbol& rs);
};

#endif