#include "../../inc/emulator/emulator.hpp"

int main(int argc, char** argv){
  if(argc < 2){
    cerr << "Bad arguments" << endl;
    exit(-1);
  }
  Emulator emulator(argv[1]);
  emulator.setMemory();
  emulator.emulate();
  emulator.printRegisters();
  return 0;
}