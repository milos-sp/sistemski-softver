#include <iostream>
#include <fstream>
#include "../../inc/symbol.hpp"
#include "../../inc/section.hpp"
#include <sstream>
#include "../../inc/linker/linker.hpp"
#include "../../inc/linker/scanner.hpp"
#include <iostream>
using namespace std;

int main(int argc, char** argv){
  Linker link;
  fstream file;
  if(argc < 5 || string(argv[1]).compare("-hex") != 0){
    cerr << "Bad arguments" << endl;
    exit(-1);
  }
  int ind;
  map<uint32_t, string> places;
  for(ind = 2; ind < argc; ind++){
    string arg = string(argv[ind]);
    if(arg.substr(0, 7).compare("-place=") == 0){
      string section;
      int pos = arg.find_first_of('@');
      section = arg.substr(7, pos - 7);
      cout << section << " ";
      string hex = arg.substr(pos + 1);
      cout << hex << endl;
      places.insert(make_pair(stoi(hex, 0, 16), section));
    }else{
      break;
    }
  }
  if(string(argv[ind]).compare("-o") != 0){
    cerr << "Bad arguments: -o is missing" << endl;
    exit(-1);
  }
  ind++;
  list<string> inputs;
  string output = argv[ind++]; // prvo ide izlazni fajl
  for(; ind < argc; ind++){ // pa ulazni fajlovi
    cout << argv[ind] << endl;
    inputs.push_back(argv[ind]);
  }
  Linker linker = Linker(output, inputs);
  linker.setPositions(places);
  /*Scanner scanner = Scanner("out.o");
  list<Section*> sections = scanner.getSectionsFromFile();
  list<Symbol*> symbols = scanner.getSymbolsFromFile();
  list<RelocationSymbol*> relSymbols = scanner.getRelocFromFile();

  linker.addSections(sections);
  linker.addSymbols(symbols);
  linker.addRelSymbols(relSymbols);
  linker.getSecDataFromFile();*/
  //umesto svega ovog gore linker.collectData();
  linker.collectData();
  linker.placeSections();
  linker.resolve();
  linker.resolveSymbolsU();
  linker.resolveRelocations();
  linker.printSections();
  linker.printSymbols();
  linker.printRelocs();
  linker.printData();
  linker.printToHexFile();
  
  return 0;
}