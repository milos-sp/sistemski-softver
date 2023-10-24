#ifndef LINKER_HPP_
#define LINKER_HPP_
#include "../section.hpp"
#include "../symbol.hpp"
#include "../relocation.hpp"
#include "./scanner.hpp"
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>
#include <map>

using namespace std;

class Linker{
  private:
    list<string> inputs;
    list<Section*> sectionList;
    list<Symbol*> symbolList;
    list<RelocationSymbol*> relocationList;
    list<char> data;
    map<uint32_t, string> dataMap;
    string output;
    map<uint32_t, string> secPositions;
    int memory;
  public:
    Linker(){}
    Linker(string output, list<string> inputs);
    void addSections(list<Section*> s);
    void symValidation(Symbol* sym);
    Section* getSection(string s, string file);
    void addSymbols(list<Symbol*> s);
    void addRelSymbols(list<RelocationSymbol*> rs);
    Symbol* getSymbol(string s, string file);
    void addData(list<char> data, uint32_t mem);
    void resolveSymbolsU();
    Symbol* searchGlobalSymbols(Symbol* s);
    string prepareValue(uint32_t val);
    void addData();
    void getSecDataFromFile();
    void collectData();
    void printSymbols();
    void printSections();
    void printRelocs();
    void printData();
    void placeSections();
    void setPositions(map<uint32_t, string> pos);
    void resolve();
    void resolveRelocations();
    void printToHexFile();
    string prepareWord(string word);
};

#endif