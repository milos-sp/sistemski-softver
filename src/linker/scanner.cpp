#include "../../inc/linker/scanner.hpp"

Scanner::Scanner(string file){
  this->fileName = file;
}

list<Section*> Scanner::getSectionsFromFile(){
  fstream input;
  input.open(fileName, std::ios::in | std::ios::binary);
  string line;
  list<Section*> sections;
  while(getline(input, line)){
    if(line.compare("SECTION_TABLE_START") == 0) break;
  }
  while(getline(input, line) && line.compare("SECTION_TABLE_END") != 0){
    istringstream sstream(line);
    string word;
    vector<string> v;

    while(sstream >> word){
      v.push_back(word);
    }
    Section* sec;
    if(v.size() == 3){
      sec = new Section(stoi(v.at(0)), v.at(1), this->fileName);
      sec->setSize(stoi(v.at(2)));
    }
    sections.push_back(sec);
  }
  input.close();
  return sections;
}

list<Symbol*> Scanner::getSymbolsFromFile(){
  fstream input;
  input.open(fileName, std::ios::in | std::ios::binary);
  string line;
  list<Symbol*> symbols;
  while(getline(input, line)){
    if(line.compare("SYMBOL_TABLE_START") == 0) break;
  }
  while(getline(input, line) && line.compare("SYMBOL_TABLE_END") != 0){
    istringstream sstream(line);
    string word;
    vector<string> v;

    while(sstream >> word){
      v.push_back(word);
    }
    Symbol* sym;
    if(v.size() == 5){
      bool isGlobal = false;
      if(v.at(3).compare("GLOBAL") == 0) isGlobal = true;
      sym = new Symbol(v.at(1), v.at(2), isGlobal, stoi(v.at(4)), this->fileName);
    }
    symbols.push_back(sym);
  }
  input.close();
  return symbols;
}

list<RelocationSymbol*> Scanner::getRelocFromFile(){
  fstream input;
  input.open(fileName, std::ios::in | std::ios::binary);
  string line;
  list<RelocationSymbol*> rels;
  while(getline(input, line)){
    if(line.compare("REL_SYMBOL_TABLE_START") == 0) break;
  }
  while(getline(input, line) && line.compare("REL_SYMBOL_TABLE_END") != 0){
    istringstream sstream(line);
    string word;
    vector<string> v;

    while(sstream >> word){
      v.push_back(word);
    }
    RelocationSymbol* sym;
    if(v.size() == 6){
      string sec = v.at(0);
      int offset = stoi(v.at(1));
      string type = v.at(2);
      int id = stoi(v.at(3));
      string name = v.at(4);
      int addend = stoi(v.at(5));
      sym = new RelocationSymbol(offset, type, name, id, sec, addend, this->fileName);
    }
    rels.push_back(sym);
  }
  input.close();
  return rels;
}

void Scanner::getSectionData(Section* sec){
  fstream input;
  input.open(fileName, std::ios::in | std::ios::binary);
  string line;
  list<char> data;
  while(getline(input, line)){
    if(line.compare("MACHINE_CODE_START") == 0) break;
  }
  string section;
  while(getline(input, line)){
    if(line.substr(0, 5) == "START"){
      int pos = line.find_first_of('_');
      section = line.substr(pos + 1);
      if(sec->getName().compare(section) == 0) break;
    }
  }
  while(getline(input, line) && line.compare("END_" + section) != 0){
    for(char c: line){
      data.push_back(c);
    }
  }
  sec->setData(data);
  input.close(); 
}