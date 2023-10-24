#include "../inc/assembler.hpp"

list<Symbol*> Assembler::symbolList;
list<Section*> Assembler::sectionList;
int Assembler::locationCounter = 0;
Section* Assembler::currSection = nullptr;
string Assembler::outputFile = "";
unordered_map<string, vector<char>> Assembler::machineCode;
unordered_map<string, vector<unsigned long>> Assembler::machineCodeBin;
unordered_map<string, vector<LiteralSym*>> Assembler::literalPool;
unordered_map<string, vector<RelocationSymbol*>> Assembler::relocationList;

void Assembler::printSymbols(){
  ofstream out;
  out.open(Assembler::outputFile, std::ios::binary);
  out << "SYMBOL_TABLE_START" << endl;
  int pos = Assembler::outputFile.find('.');
  string f = Assembler::outputFile.substr(0, pos) + ".obj";
  ofstream outFile(f); //tekstualni fajl
  outFile << left << setw(8) << "ID";
  outFile << left << setw(16) << "SYMBOL";
  outFile << left << setw(16) << "SECTION";
  outFile << left << setw(11) << "TYPE";
  outFile << left << setw(12) << "VALUE" << endl;
  outFile << "--------------------------------------------------------------" << endl;
  for(Symbol* s: Assembler::symbolList){
    outFile << *s << endl;
    s->binPrint(out);
  }
  out << "SYMBOL_TABLE_END" << endl;
  outFile.close();
  out.close();
}

void Assembler::addSymbol(Symbol* s){
  Assembler::symbolList.push_back(s);
}

void Assembler::addSection(Section* s){
  Assembler::sectionList.push_back(s);
}

void Assembler::printSections(){
  ofstream out;
  out.open(Assembler::outputFile, std::ios::binary | std::ios::app);
  int pos = Assembler::outputFile.find('.');
  string f = Assembler::outputFile.substr(0, pos) + ".obj";
  std::ofstream outFile(f, std::ios::app); //tekstualni fajl
  outFile << left << setw(8) << "ID";
  outFile << left << setw(16) << "SECTION";
  outFile << left << setw(5) << "SIZE" << endl;
  outFile << "----------------------------------------------" << endl;
  out << "SECTION_TABLE_START" << endl;
  for(Section* s: Assembler::sectionList){
    outFile << *s << endl;
   // out.write((char*)&(*s), sizeof(Section)); out << endl;
   //out << *s << endl;
   s->binPrint(out);
  }
  out << "SECTION_TABLE_END" << endl;
  outFile.close();
  out.close();
  // fstream out1;
  // out1.open("out.o", std::ios::in | std::ios::binary);
  // Section sec("");
  // out1.read((char*)&sec, sizeof(Section));
  // cout << "IZ FAJLA: " << sec << endl;
  // out1.close();
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

unsigned long Assembler::toBinary(string i){
  string n = "";
  int ind = i.length();
  n = n + i.at(ind-2) + i.at(ind-1) + i.at(ind-4) + i.at(ind-3) + i.at(ind-6) + i.at(ind-5) + i.at(ind-8) + i.at(ind-7);
  return stoul(n, nullptr, 16);
}

void Assembler::addData(string d){
  if(Assembler::machineCode.find(Assembler::currSection->getName()) == Assembler::machineCode.end()){
    //prvi put ova sekcija
    vector<char> v;
    for(char c: d){
      v.push_back(c);
    }
    Assembler::machineCode.insert(make_pair(Assembler::currSection->getName(), v));
    vector<unsigned long> vi;
    vi.push_back(toBinary(d));
    Assembler::machineCodeBin.insert(make_pair(Assembler::currSection->getName(), vi));
  }else{
    //samo se dodaje
    for(char c: d){
      Assembler::machineCode.at(Assembler::currSection->getName()).push_back(c);
    }
    Assembler::machineCodeBin.at(Assembler::currSection->getName()).push_back(toBinary(d));
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
  int pos = Assembler::outputFile.find('.');
  string f = Assembler::outputFile.substr(0, pos) + ".obj";
  std::ofstream outFile(f, std::ios::app); //tekstualni fajl
  unordered_map<string, vector<char>>::iterator itr;
  outFile << "MACHINE CODE" << endl;
  for(itr = Assembler::machineCode.begin(); itr != Assembler::machineCode.end(); itr++){
    outFile << "#." << itr->first << endl;
    int br = 0;
    for(char c: itr->second){
      outFile << c;
      br++;
      if(br % 2 == 0 && br != 16) outFile << " ";
      if(br == 16){
        outFile << endl;
        br = 0;
      }
    }
    outFile << endl;
  }
  ofstream out;
  out.open(Assembler::outputFile, std::ios::binary | std::ios::app);
  out << "MACHINE_CODE_START" << endl;
  for(auto itrb = Assembler::machineCode.begin(); itrb != Assembler::machineCode.end(); itrb++){
    out << "START_" << itrb->first << endl;
    int br = 0;
    for(char c: itrb->second){
      out << c;
      br++;
      if(br == 8){
        out << endl;
        br = 0;
      }
    }
    out << "END_" << itrb->first << endl;
  }
  /*for(auto itrb = Assembler::machineCodeBin.begin(); itrb != Assembler::machineCodeBin.end(); itrb++){
    cout << "#." << itrb->first << endl;
    out << "START_" << itrb->first << endl;
    for(auto i: itrb->second){
      stringstream stream;
      stream << hex << setw(8) << setfill('0') << i;
      cout << "HEX: " << stream.str() << endl;
      out << stream.str() << endl;
      //out << i << endl;
    }
    out << "END_" << itrb->first << endl;
  }*/
  out << "MACHINE_CODE_END" << endl;
  out.close();
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
  if(reg.compare("%status") == 0){
    return s + "0";
  }
  if(reg.compare("%handler") == 0){
    return s + "1";
  }
  if(reg.compare("%cause") == 0){
    return s + "2";
  }
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
      Assembler::currSection->setSize(Assembler::currSection->getSize() + 4); //povecava se velicina sekcije
    }
  }
}

void Assembler::addRelocation(int offset, Symbol* s){
  RelocationSymbol* rs;
  if(!s->isGlobalSymbol()){
    //mora sekcija u kojoj se nalazi da bude dodata, a addend je onda njegova vrednost
    string sec = s->getSection()->getName();
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
  ofstream out;
  out.open(Assembler::outputFile, std::ios::binary | std::ios::app);
  out << "REL_SYMBOL_TABLE_START" << endl;
  int pos = Assembler::outputFile.find('.');
  string f = Assembler::outputFile.substr(0, pos) + ".obj";
  std::ofstream outFile(f, std::ios::app); //tekstualni fajl
  unordered_map<string, vector<RelocationSymbol*>>::iterator itr;
  outFile << "RELOCATION DATA" << endl;
  for(itr = Assembler::relocationList.begin(); itr != Assembler::relocationList.end(); itr++){
    outFile << "#.rela." << itr->first << endl;
    int br = 0;
    outFile << left << setw(8) << "OFFSET";
    outFile << left << setw(16) << "TYPE";
    outFile << left << setw(16) << "SYMBOL";
    outFile << left << setw(11) << "ADDEND" << endl;
    for(RelocationSymbol* c: itr->second){
      outFile << *c;
      out << itr->first << " ";
      c->binPrint(out);
    }
  }
  out << "REL_SYMBOL_TABLE_END" << endl;
  outFile.close();
  out.close();
}