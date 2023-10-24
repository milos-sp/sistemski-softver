#include "../inc/relocation.hpp"
#include <iomanip>

RelocationSymbol::RelocationSymbol(int o, string t, Symbol* s, int a){
  this->offset = o;
  this->type = t;
  this->sym = s;
  this->addend = a;
  this->symId = s->getId();
  this->symName = s->getName();
}

RelocationSymbol::RelocationSymbol(int o, string t, string symName, int symId, string section, int a, string file){
  this->offset = o;
  this->type = t;
  this->symName = symName;
  this->symId = symId;
  this->secName = section;
  this->addend = a;
  this->inputFile = file;
}

ostream& operator<<(ostream& os, const RelocationSymbol& rs){
  os << left << setw(8) << hex << rs.offset;
  os << left << setw(16) << rs.type;
  os << rs.symId << " (" << rs.symName << left << setw(12) << ")";
  os << left << setw(11) << dec << rs.addend << endl;
  return os;
}

int RelocationSymbol::getAddend(){
  return this->addend;
}

int RelocationSymbol::getOffset(){
  return this->offset;
}

void RelocationSymbol::setSymbol(Symbol* s){
  this->sym = s;
}

Symbol* RelocationSymbol::getSymbol(){
  return this->sym;
}

string RelocationSymbol::getRelSection(){
  return this->secName;
}

string RelocationSymbol::getNameOfSymbol(){
  return this->symName;
}

string RelocationSymbol::getNameOfFile(){
  return this->inputFile;
}

void RelocationSymbol::binPrint(ostream& os){
  os << this->offset << " " << this->type << " " << this->sym->getId() << " " << this->sym->getName() << " " << this->addend << endl;
}