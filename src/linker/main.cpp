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
  for(ind = 2; ind < argc; ind++){
    string arg = string(argv[ind]);
    if(arg.substr(0, 7).compare("-place=") == 0){
      string section;
      int pos = arg.find_first_of('@');
      section = arg.substr(7, pos - 7);
      cout << section << " ";
      string hex = arg.substr(pos + 1);
      cout << hex << endl;
    }else{
      break;
    }
  }
  if(string(argv[ind]).compare("-o") != 0){
    cerr << "Bad arguments: -o is missing" << endl;
    exit(-1);
  }
  ind++;
  for(; ind < argc; ind++){ //prvo ide izlazni fajl pa ulazni fajlovi
    cout << argv[ind] << endl;
  }
  file.open("out.o", std::ios::in);
  string line;
  getline(file, line);
  //getline(file, line);
 /* while(getline(file, line) && line.substr(0, 7) != "SECTION"){
    cout << *link.prepareSection(line) << endl;
  }*/
  /*while(getline(file, line)){
    cout << line << endl;
  }*/
  file.close();
  Scanner scanner = Scanner("out.o");
  list<Section*> sections = scanner.getSectionsFromFile();
  for(Section* el: sections){
    cout << *el << endl;
  }
  list<Symbol*> symbols = scanner.getSymbolsFromFile();
  for(Symbol* el: symbols){
    cout << *el << endl;
  }
  list<RelocationSymbol*> relSymbols = scanner.getRelocFromFile();
  for(RelocationSymbol* el: relSymbols){
    cout << *el << endl;
  }
  return 0;
}