#include "../inc/section.hpp"
#include <iomanip>

int Section::cnt = 0;

Section::Section(string name){
  this->name = name;
  this->id = cnt++;
}

string Section::getName(){
  if(this) return this->name;
  else return "UND";
}

ostream& operator<<(ostream& os, const Section &sec){
  os << left << setw(8) << dec << sec.id ;
  os << left << setw(16) << sec.name;
  os << left << setw(5) << dec << sec.size << endl;
  return os;
}

bool Section::operator==(const Section& s){
  return this->name == s.name;
}

void Section::setSize(int s){
  this->size = s;
}

int Section::getSize(){
  return this->size;
}