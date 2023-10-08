#ifndef LITEREAL_POOL_HPP
#define LITERAL_POOL_HPP
#include <iostream>

using namespace std;

class LiteralSym{
private:
  string name;
  int value;
  int offset;

public:
  LiteralSym(string n, int v);
  void setOffset(int o);
  string getName();
  int getOffset();
  int getValue();
};

#endif