#include "../inc/assembler.hpp"

list<Symbol*> Assembler::symbolList;
list<Section*> Assembler::sectionList;
int Assembler::locationCounter = 0;
Section* Assembler::currSection = nullptr;
unordered_map<string, vector<char>> Assembler::machineCode;
unordered_map<string, vector<LiteralSym*>> Assembler::literalPool;
unordered_map<string, vector<RelocationSymbol*>> Assembler::relocationList;

void Assembler::printSymbols(){
  cout << "ID\t" << "SYMBOL\t\t" << "SECTION\t\t" << "TYPE\t   " << "VALUE" << endl;
  cout << "--------------------------------------------------------------" << endl;
  for(Symbol* s: Assembler::symbolList){
    cout << *s << endl;
  }
}

void Assembler::addSymbol(Symbol* s){
  Assembler::symbolList.push_back(s);
}

void Assembler::addSection(Section* s){
  Assembler::sectionList.push_back(s);
}

void Assembler::printSections(){
  cout << "ID\t" << "SECTION\t\t" << "SIZE" << endl;
  cout << "----------------------------------------------" << endl;
  for(Section* s: Assembler::sectionList){
    cout << *s << endl;
  }
}

bool Assembler::hasSection(Section* sec){
  for(Section* s: Assembler::sectionList){
    if(*sec == *s) return true;
  }
  return false;
}

Section* Assembler::getSection(string name){
  for(Section* s: Assembler::sectionList){
    if(s->getName() == name) return s;
  }
  return nullptr;
}

Symbol* Assembler::getSymbol(string name){
  for(Symbol* s: Assembler::symbolList){
    if(s->getName() == name) return s;
  }
  return nullptr;
}

void Assembler::addData(string d){
  if(Assembler::machineCode.find(Assembler::currSection->getName()) == Assembler::machineCode.end()){
    //prvi put ova sekcija
    vector<char> v;
    for(char c: d){
      v.push_back(c);
    }
    Assembler::machineCode.insert(make_pair(Assembler::currSection->getName(), v));
  }else{
    //samo se dodaje
    for(char c: d){
      Assembler::machineCode.at(Assembler::currSection->getName()).push_back(c);
    }
  }
}

void Assembler::addToPool(LiteralSym* l){
  if(Assembler::literalPool.find(Assembler::currSection->getName()) == Assembler::literalPool.end()){
    //prvi koji se dodaje
    vector<LiteralSym*> v;
    l->setOffset(0);
    v.push_back(l);
    Assembler::literalPool.insert(make_pair(Assembler::currSection->getName(), v));
  }else{
    l->setOffset(Assembler::literalPool.at(Assembler::currSection->getName()).size() * 4); //ili mozda i puta 4
    Assembler::literalPool.at(Assembler::currSection->getName()).push_back(l);
  }
}

LiteralSym* Assembler::getFromPool(string n){
  LiteralSym* l = nullptr;
  if(Assembler::literalPool.find(Assembler::currSection->getName()) != Assembler::literalPool.end()){
    for(LiteralSym* el: Assembler::literalPool.at(Assembler::currSection->getName())){
      if(n == el->getName()){
        l = el;
        return l;
      }
    }
  }
  return l;
}

void Assembler::clear(){
  for(Symbol* s: Assembler::symbolList){
    delete s;
  }
  for(Section* s: Assembler::sectionList){
    delete s;
  }
}

void Assembler::printData(){
  unordered_map<string, vector<char>>::iterator itr;
  cout << "MACHINE CODE" << endl;
  for(itr = Assembler::machineCode.begin(); itr != Assembler::machineCode.end(); itr++){
    cout << "#." << itr->first << endl;
    int br = 0;
    for(char c: itr->second){
      cout << c;
      br++;
      if(br % 2 == 0 && br != 16) cout << " ";
      if(br == 16){
        cout << endl;
        br = 0;
      }
    }
    cout << endl;
  }
}

string Assembler::prepareData(int ins){
  stringstream sstream;
  sstream << hex << ins;
  string res = sstream.str();
  if(ins < 16){
    return '0' + res;
  }else{
    return res;
  }
}

string Assembler::prepareLiteral(int lit){
  stringstream sstream;
  sstream << hex << lit;
  string res = sstream.str();
  int i = res.length();
  while(i < 8){
    res = "0" + res;
    i++;
  }
  string s = "";
  i = res.length() - 1;
  while(i > 0){ //da bude little endian
    s = s + res[i-1] + res[i];
    i -= 2;
  }
  return s;
}

string Assembler::prepareOffset(int o, string reg){
  stringstream sstream;
  sstream << hex << o;
  string res = sstream.str();
  int i = res.length();
  while(i < 3){
    res = "0" + res;
    i++;
  }
  i = res.length() - 1;
  string s = "";
  s = s + res[i-1] + res[i] + Assembler::prepareRegister(reg) + res[i-2];
  return s;
}

string Assembler::prepareRegister(string reg){
  string s = "";
  int r;
  if(reg.find_first_of("r") != string::npos){
    if(reg.size() == 3){
      s = s + reg[1] + reg[2];
    }else{
      s = s + reg[1];
    }
    stringstream o;
    o << s;
    o >> r;
    stringstream sstream;
    sstream << hex << r;
    return sstream.str();
  }else if(reg.compare("pc") == 0){
    return "f";
  }else{
    return "e"; //sp
  }
}

void Assembler::addPoolData(string section){
  if(Assembler::literalPool.find(section) != Assembler::literalPool.end()){
    for(LiteralSym* el: Assembler::literalPool.at(section)){
      string s = Assembler::prepareLiteral(el->getValue());
      Assembler::addData(s);     
    }
  }
}

void Assembler::addRelocation(int offset, Symbol* s){
  RelocationSymbol* rs;
  if(!s->isGlobalSymbol()){
    //mora sekcija u kojoj se nalazi da bude dodata, a addend je onda njegova vrednost
    string sec = "." + s->getSection()->getName();
    Symbol* a = Assembler::getSymbol(sec);
    int addend = s->getOffset();
    rs = new RelocationSymbol(offset, "R_X86_64_32", a, addend);
  }else{
    //ostaje simbol koji je prosledjen, a addend je 0
    rs = new RelocationSymbol(offset, "R_X86_64_32", s, 0);
  }
  if(Assembler::relocationList.find(Assembler::currSection->getName()) == Assembler::relocationList.end()){
    //prvi koji se dodaje za ovu sekciju
    vector<RelocationSymbol*> v;
    v.push_back(rs);
    Assembler::relocationList.insert(make_pair(Assembler::currSection->getName(), v));
  }else{
    Assembler::relocationList.at(Assembler::currSection->getName()).push_back(rs);
  }
}

void Assembler::printRelocationData(){
  unordered_map<string, vector<RelocationSymbol*>>::iterator itr;
  cout << "RELOCATION DATA" << endl;
  for(itr = Assembler::relocationList.begin(); itr != Assembler::relocationList.end(); itr++){
    cout << "#.rela." << itr->first << endl;
    int br = 0;
    cout << left << setw(8) << "OFFSET";
    cout << left << setw(16) << "TYPE";
    cout << left << setw(16) << "SYMBOL";
    cout << left << setw(11) << "ADDEND" << endl;
    for(RelocationSymbol* c: itr->second){
      cout << *c << endl;
    }
  }
}