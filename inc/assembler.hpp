#ifndef ASSEMBLER_HPP_
#define ASSEMBLER_HPP_
#include <iostream>
#include <list>
#include "./symbol.hpp"
#include "./section.hpp"
#include "./literalPool.hpp"
#include "./relocation.hpp"
#include <unordered_map>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

class Assembler{

private:

  static list<Symbol*> symbolList;
  static list<Section*> sectionList;
  static unordered_map<string, vector<char>> machineCode; //za svaku sekciju hex kod
  static unordered_map<string, vector<LiteralSym*>> literalPool;
  static unordered_map<string, vector<RelocationSymbol*>> relocationList;

public:

  enum Instruction{
    HALT = 0b00000000,
    INT = 0b00010000,
    IRET,
    JMP = 0b00110000, //ako mora da se pristupa bazenu literala niza 4 bita su 1000
    CALL = 0b00100000, //ako mora da se pristupa bazenu literala niza 4 bita su 0001
    RET = 0b10010011, //pop pc
    BEQ = 0b00110001,
    BNE = 0b00110010,
    BGT = 0b00110011,
    PUSH = 0b10000001, //trebalo bi da je ok ovako
    POP = 0b10010011,
    XCHG = 0b01000000,
    ADD = 0b01010000,
    SUB = 0b01010001,
    MUL = 0b01010010,
    DIV = 0b01010011,
    NOT = 0b01100000,
    AND = 0b01100001,
    OR = 0b01100010,
    XOR = 0b01100011,
    SHL = 0b01110000,
    SHR = 0b01110001,
    LD = 0b10010000, //neke se ORuju sa modom adresiranja
    ST = 0b10000000,
    CSRRD = 0b10010000,
    CSRWR = 0b10010100
  };

  static int locationCounter;
  static Section* currSection;
  static void printSymbols();
  static void addSymbol(Symbol* s);
  static void printSections();
  static void addSection(Section* s);
  static bool hasSection(Section* s);
  static Symbol* getSymbol(string name);
  static Section* getSection(string name);
  static void addData(string d);
  static void printData();
  static void clear();
  static string prepareData(int ins);
  static string prepareRegister(string reg);
  static string prepareLiteral(int lit);
  static string prepareOffset(int o, string reg);
  static void addToPool(LiteralSym* l);
  static LiteralSym* getFromPool(string n);
  static void addPoolData(string section);
  static void addRelocation(int offset, Symbol* s);
  static void printRelocationData();
};

#endif