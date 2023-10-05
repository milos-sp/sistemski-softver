#include "../inc/assembler.hpp"

list<Symbol*> Assembler::symbolList;
list<Section*> Assembler::sectionList;
int Assembler::locationCounter = 0;
Section* Assembler::currSection = nullptr;
unordered_map<string, vector<char>> Assembler::machineCode;

void Assembler::printSymbols(){
  cout << "ID\t" << "SYMBOL\t\t" << "SECTION\t\t" << "TYPE\t   " << "OFFSET" << endl;
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

string Assembler::prepareOffset(int o){
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
  s = s + res[i-1] + res[i] + '0' + res[i-2];
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
    return "0f";
  }else{
    return "0e"; //sp
  }
}