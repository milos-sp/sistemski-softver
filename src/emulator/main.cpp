#include "../../inc/emulator/emulator.hpp"

int main(int argc, char** argv){
  if(argc < 2){
    cerr << "Bad arguments" << endl;
    exit(-1);
  }
  cout << "PROBA EMULATOR" << endl;
  Emulator emulator(argv[1]);
  emulator.setMemory();
  emulator.emulate();
  //emulator.test(0x40000090, 0xf3108859);
  emulator.printRegisters();
  return 0;
}