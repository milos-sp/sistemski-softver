#ifndef SCANNER_HPP_
#define SCANNER_HPP_
#include "../symbol.hpp"
#include "../section.hpp"
#include "../relocation.hpp"
#include <list>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

class Scanner{

  private:
    string fileName;
  public:
    Scanner(string file);
    list<Section*> getSectionsFromFile();
    list<Symbol*> getSymbolsFromFile();
    list<RelocationSymbol*> getRelocFromFile();
    list<char> getSecDataFromFile();
    void getSectionData(Section* sec);
};

#endif