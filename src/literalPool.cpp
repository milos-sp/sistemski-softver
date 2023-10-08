#include "../inc/literalPool.hpp"

LiteralSym::LiteralSym(string n, int v){
  this->name = n;
  this->value = v;
}

void LiteralSym::setOffset(int o){
  this->offset = o;
}

string LiteralSym::getName(){
  return this->name;
}

int LiteralSym::getOffset(){
  return this->offset;
}

int LiteralSym::getValue(){
  return this->value;
}