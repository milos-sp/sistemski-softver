#ifndef EMULATOR_HPP_
#define EMULATOR_HPP_
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
#include <list>
#include <cstring>
#include <map>

using namespace std;

class Emulator{
  private:
    uint registers[16];
    uint status;
    uint handler;
    uint cause;
    string inputFile;
    
    map<uint32_t, string> mem;
    bool halted;

  public:
    Emulator(string file);
    void printRegisters();
    void setMemory();
    void executeInstr();
    int strToHex(string s);
    void emulate();
    int16_t getDisplacement(uint16_t d);
    list<string> getRegisterBytes(uint reg);
    void test(uint32_t addr, uint reg);
};

#endif