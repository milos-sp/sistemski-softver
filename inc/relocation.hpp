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
    string inputFile;
    string symName;
    string secName;
    int symId;

  public:
    RelocationSymbol(int o, string t, Symbol* s, int a);
    RelocationSymbol(int o, string t, string symName, int symId, string section, int a, string file);
    friend ostream& operator <<(ostream& os, const RelocationSymbol& rs);
    void binPrint(ostream& os);
};

#endif