#include "../inc/symbol.hpp"
#include <iomanip>
#include <sstream>

int Symbol::cnt = 0;

Symbol::Symbol(string name, Section* sec, bool isGlobal){
  this->name = name;
  this->sec = sec;
  this->isGlobal = isGlobal;
  this->id = cnt++;
  this->offset = 0;
  this->section = sec->getName();
}

Symbol::Symbol(string name, string section, bool isGlobal){
  this->name = name;
  this->section = section;
  this->isGlobal = isGlobal;
  this->id = cnt++;
}

Symbol::Symbol(string name, string section, bool isGlobal, int offset, string file){
  this->name = name;
  this->section = section;
  this->isGlobal = isGlobal;
  this->id = cnt++;
  this->offset = offset;
  this->inputFile = file;
}

string Symbol::getName(){
  return this->name;
}

Section* Symbol::getSection(){
  return this->sec;
}

bool Symbol::isGlobalSymbol(){
  return this->isGlobal;
}

int Symbol::getId(){
  return this->id;
}

void Symbol::setGlobal(bool g){
  this->isGlobal = g;
}

void Symbol::setOffset(int offset){
  this->offset = offset;
}

void Symbol::setSection(Section* sec){
  this->sec = sec;
  this->section = sec->getName();
}

void Symbol::setGlobal(){
  this->isGlobal = true;
}

void Symbol::setValue(int value){
  this->value = value;
}

int Symbol::getValue(){
  return this->value;
}

int Symbol::getOffset(){
  return this->offset;
}

ostream& operator<<(ostream& os, const Symbol &sym){
  os << left << setw(8) << dec << sym.id;
  os << left << setw(16) << sym.name;
  os << left << setw(16) << sym.section;
  os << left << setw(11) << (sym.isGlobal ? "GLOBAL":"LOCAL");
  os << left << setw(12) << dec << sym.offset << endl;
  return os;
}

void Symbol::binPrint(ostream& os){
  os << dec << this->id << " " << this->name << " " << this->section << " " << (this->isGlobal ? "GLOBAL":"LOCAL")
  << " " << dec << this->offset << endl;
}

