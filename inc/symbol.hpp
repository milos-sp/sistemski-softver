#ifndef SYMBOL_HPP_
#define SYMBOL_HPP_
#include <iostream>
#include "./section.hpp"
using namespace std;

class Symbol{

private:
  static int cnt;
  string name;
  int id;
  Section* sec;
  bool isGlobal;
  int offset;
  int value;
  string section;
  string inputFile;

public:
  Symbol(string name, Section* sec, bool isGlobal);
  Symbol(string name, string section, bool isGlobal);
  Symbol(string name, string section, bool isGlobal, int offset, string file);

  void setGlobal(bool g);
  void setOffset(int offset);
  void setSection(Section* sec);
  void setGlobal();
  void setValue(int value);
  Section* getSection();
  string getName();
  int getOffset();
  bool isGlobalSymbol();
  int getValue();
  friend ostream& operator <<(ostream& os, const Symbol &sym);
  int getId();
  void binPrint(ostream& os);
};

#endif