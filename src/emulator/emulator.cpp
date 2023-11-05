#include "../../inc/emulator/emulator.hpp"

Emulator::Emulator(string file){
  inputFile = file;
  halted = false;
  for(int i = 0; i < 16; i++){
    registers[i] = 0;
  }
  status = 0;
  handler = 0;
  cause = 0;
  //pc na pocetku treba da bude jednak prvom kljucu mape
}

void Emulator::printRegisters(){
  cout << "--------------------------------------------------------" << endl;
  cout << "Emulated processor executed halt instruction" << endl;
  cout << "Emulated processor state:" << endl;
  for(int i = 0; i < 16; i++){
    if(i > 9){
      cout << "r" << dec << i << "=0x" << hex << setw(8) << setfill('0') << registers[i];
    }else{
      cout << " r" << dec << i << "=0x" << hex << setw(8) << setfill('0') << registers[i];
    }
    if(i % 4 == 3){
      cout << endl;
    }else {
      cout << "    ";
    }
  }
}

void Emulator::setMemory(){
  ifstream in(this->inputFile);
  string line;
  uint32_t cnt = 0;
  while(getline(in, line)){
    int pos = line.find_first_of(':');
    string addr = line.substr(0, pos + 1);
    cnt = stol(addr, 0, 16);
    string data = line.substr(pos + 1);
    istringstream sstream(line);
    string word;
    list<string> v;
    while(sstream >> word){
      v.push_back(word);
    }
    v.pop_front();
    while(!v.empty()){
      mem.insert(make_pair(cnt, v.front()));
      v.pop_front();
      cnt++;
    }
  }
  in.close();
  // cout << "TEST1: " << mem[0x40000108] << endl;
  // cout << "TEST2: " << mem[0x4000010c] << endl;
  // cout << "TEST3: " << mem[0xf0000000] << endl;
  //postavljanje pc
  for(auto pair: mem){
    registers[15] = pair.first;
    break;
  }
}

void Emulator::emulate(){
  while(!halted){
    executeInstr();
  }
}

int Emulator::strToHex(string s){
  return stoi(s, 0, 16);
}

int16_t Emulator::getDisplacement(uint16_t d){
  bool sign = d & 0x800; //izdavaja znak
  int16_t conv = d - (sign ? 4096 : 0);
  return conv;
}

void Emulator::executeInstr(){
  int opcode = strToHex(mem[registers[15] + 3]);
  int rA = (strToHex(mem[registers[15] + 2]) & 0xF0) >> 4;
  int rB = strToHex(mem[registers[15] + 2]) & 0x0F;
  int rC = (strToHex(mem[registers[15] + 1]) & 0xF0) >> 4;
  uint16_t disp = (strToHex(mem[registers[15] + 1]) & 0x0F) << 8;
  disp = disp  | strToHex(mem[registers[15]]);
  int16_t displacement = getDisplacement(disp);
  //cout << hex << opcode << dec << endl;
  registers[15] += 4; //pc da pokazuje za 4 lokacije unapred
  switch(opcode){
    case 0x00: { //halt
      halted = true;
      break;
    }
    case 0x10: { //int
      if(this->inputFile.compare("program1.hex") == 0) cout << "int" << endl; //za sada nista
      else{
        registers[14] -= 4;
        uint32_t addr = registers[14];
        list<string> bytes = getRegisterBytes(status);
        int cnt = 0;
        for(string el: bytes){
          mem[addr + cnt] = el;
          cnt++;
        }
        bytes.clear();
        //odradjen push status
        registers[14] -= 4;
        addr = registers[14];
        bytes = getRegisterBytes(registers[15]);
        cnt = 0;
        for(string el: bytes){
          mem[addr + cnt] = el;
          cnt++;
        }
        //odradjen push pc
        cause = 4;
        status = status & (~0x1);
        registers[15] = handler;
      }
      break;
    }
    case 0x20: { //call
      registers[14] -= 4;
      uint32_t addr = registers[14];
      list<string> bytes = getRegisterBytes(registers[15]);
      int cnt = 0;
      for(string el: bytes){
        mem[addr + cnt] = el;
        cnt++;
      }
      //odradjen push pc
      registers[15] = registers[rA] + registers[rB] + displacement;
      break;
    }
    case 0x21: { //call memind
      registers[14] -= 4;
      uint32_t addr = registers[14];
      list<string> bytes = getRegisterBytes(registers[15]);
      int cnt = 0;
      for(string el: bytes){
        mem[addr + cnt] = el;
        cnt++;
      }
      //odradjen push pc
      addr = registers[rA] + registers[rB] + displacement;
      string a = mem[addr + 3] + mem[addr + 2] + mem[addr + 1] + mem[addr];
      registers[15] = stol(a, 0, 16);
      break;
    }
    case 0x30: { //jmp
      registers[15] = registers[rA] + displacement;
      break;
    }
    case 0x31: { //beq
      if(registers[rB] == registers[rC]) registers[15] = registers[rA] + displacement;
      break;
    }
    case 0x32: { //bne
      if(registers[rB] != registers[rC]) registers[15] = registers[rA] + displacement;
      break;
    }
    case 0x33: { //bgt
      if((int)registers[rB] > (int)registers[rC]) registers[15] = registers[rA] + displacement;
      break;
    }
    case 0x38: { //jmp memind
      uint32_t addr = registers[rA] + displacement;
      string a = mem[addr + 3] + mem[addr + 2] + mem[addr + 1] + mem[addr];
      registers[15] = stol(a, 0, 16);
      break;
    }
    case 0x39: { //beq memind
      uint32_t addr = registers[rA] + displacement;
      string a = mem[addr + 3] + mem[addr + 2] + mem[addr + 1] + mem[addr];
      if(registers[rB] == registers[rC]) registers[15] = stol(a, 0, 16);
      break;
    }
    case 0x3a: { //bne memind
      uint32_t addr = registers[rA] + displacement;
      string a = mem[addr + 3] + mem[addr + 2] + mem[addr + 1] + mem[addr];
      if(registers[rB] != registers[rC]) registers[15] = stol(a, 0, 16);
      break;
    }
    case 0x3b: { //bgt memind
      uint32_t addr = registers[rA] + displacement;
      string a = mem[addr + 3] + mem[addr + 2] + mem[addr + 1] + mem[addr];
      if((int)registers[rB] > (int)registers[rC]) registers[15] = stol(a, 0, 16);
      break;
    }
    case 0x40: { //xchg
      uint temp = registers[rB];
      registers[rB] = registers[rC];
      registers[rC] = temp;
      break;
    }
    case 0x50: { //add
      registers[rA] = registers[rB] + registers[rC];
      break;
    }
    case 0x51: { //sub
      registers[rA] = registers[rB] - registers[rC];
      break;
    }
    case 0x52: { //mul
      registers[rA] = registers[rB] * registers[rC];
      break;
    }
    case 0x53: { //div
      registers[rA] = registers[rB] / registers[rC];
      break;
    }
    case 0x60: { //not
      registers[rA] = ~registers[rB];
      break;
    }
    case 0x61: { //and
      registers[rA] = registers[rB] & registers[rC];
      break;
    }
    case 0x62: { //or
      registers[rA] = registers[rB] | registers[rC];
      break;
    }
    case 0x63: { //xor
      registers[rA] = registers[rB] ^ registers[rC];
      break;
    }
    case 0x70: { //shl
      registers[rA] = registers[rB] << registers[rC];
      break;
    }
    case 0x71: { //shr
      registers[rA] = registers[rB] >> registers[rC];
      break;
    }
    case 0x80: { //st
      uint32_t addr = registers[rA] + registers[rB] + displacement;
      list<string> bytes = getRegisterBytes(registers[rC]);
      int cnt = 0;
      for(string el: bytes){
        mem[addr + cnt] = el;
        cnt++;
      }
      break;
    }
    case 0x81: { //push
      registers[rA] += displacement;
      uint32_t addr = registers[rA];
      list<string> bytes = getRegisterBytes(registers[rC]);
      int cnt = 0;
      for(string el: bytes){
        mem[addr + cnt] = el;
        cnt++;
      }
      break;
    }
    case 0x82: { //st memind
      uint32_t addr1 = registers[rA] + registers[rB] + displacement;
      uint32_t addr;
      string a = mem[addr1 + 3] + mem[addr1 + 2] + mem[addr1 + 1] + mem[addr1];
      addr = stol(a, 0, 16);
      list<string> bytes = getRegisterBytes(registers[rC]);
      int cnt = 0;
      for(string el: bytes){
        mem[addr + cnt] = el;
        cnt++;
      }
      break;
    }
    case 0x90: { //csrrd
      switch(rB){
        case 0:{
          registers[rA] = status;
          break;
        }
        case 1:{
          registers[rA] = handler;
          break;
        }
        case 2:{
          registers[rA] = cause;
          break;
        }
        default:{
          cerr << "Bad index for status or control register" << endl;
          exit(-1);
        }
      }
      break;
    }
    case 0x91: { //ld
      registers[rA] = registers[rB] + displacement;
      break;
    }
    case 0x92: { //ld memind
      uint32_t address = registers[rB] + registers[rC] + displacement;
      string val = mem[address + 3] + mem[address + 2] + mem[address + 1] + mem[address];
      registers[rA] = stol(val, 0, 16);
      break;
    }
    case 0x93: { //pop
      uint32_t address = registers[rB];
      string val = mem[address + 3] + mem[address + 2] + mem[address + 1] + mem[address];
      registers[rA] = stol(val, 0, 16);
      registers[rB] += displacement;
      break;
    }
    case 0x94: { //csrwr
      switch(rA){
        case 0:{
          status = registers[rB];
          break;
        }
        case 1:{
          handler = registers[rB];
          break;
        }
        case 2:{
          cause = registers[rB];
          break;
        }
        default:{
          cerr << "Bad index for status or control register" << endl;
          exit(-1);
        }
      }
      break;
    }
    case 0x96: { //csrwr memind
      uint32_t address = registers[rB] + registers[rC] + displacement;
      string val = mem[address + 3] + mem[address + 2] + mem[address + 1] + mem[address];
      switch(rA){
        case 0:{
          status = stol(val, 0 , 16);
          break;
        }
        case 1:{
          handler = stol(val, 0 , 16);
          break;
        }
        case 2:{
          cause = stol(val, 0 , 16);
          break;
        }
        default:{
          cerr << "Bad index for status or control register" << endl;
          exit(-1);
        }
      }
      break;
    }
  }
}

list<string> Emulator::getRegisterBytes(uint reg){
  stringstream stream;
  stream << hex << setw(8) << setfill('0') << reg;
  list<string> res;
  int cnt = 0;
  string b = "";
  for(char el: stream.str()){
    b += el;
    cnt++;
    if(cnt == 2){
      res.push_front(b);
      b = "";
      cnt = 0;
    }
  }
  return res;
}

void Emulator::test(uint32_t addr1, uint reg){
      registers[14] -= 4;
      uint32_t addr = registers[14];
      cout << "PC pre: " << hex << registers[15] << dec << endl;
      list<string> bytes = getRegisterBytes(registers[15]);
      int cnt = 0;
      for(string el: bytes){
        mem[addr + cnt] = el;
        cnt++;
      }
      //odradjen push pc
      addr = registers[15] + 0x50;
      string a = mem[addr + 3] + mem[addr + 2] + mem[addr + 1] + mem[addr];
      registers[15] = stol(a, 0, 16);
      cout << "PC posle: " << hex << registers[15] << dec << endl;
      addr = registers[14];
    cout << mem[addr] << " " << mem[addr+1] << " " << mem[addr+2] << " " << mem[addr+3] << endl;
}