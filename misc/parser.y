%{
  //defs
  #include <cstdio>
  #include <iostream>
  #include "inc/assembler.hpp"
  #include "inc/symbol.hpp"
  #include "inc/section.hpp"
  using namespace std;

  extern int yylex();
  extern int yyparse();
  extern FILE* yyin;
  extern int linenum;
  void yyerror(const char* s);
  bool firstPass = true;
%}

%output "parser.cpp"
%defines "parser.hpp"

%union{
  int ival;
  float fval;
  char* sval;
}

%token HALT INT IRET JMP CALL RET BEQ BNE BGT PUSH POP XCHG ADD SUB MUL DIV NOT AND OR XOR 
SHL SHR LD ST CSRRD CSRWR L_PAR R_PAR COMMA PLUS MINUS COLON SEMICOLON PERCENT DOLLAR REGISTER INSTR
%token END ENDL

%token<ival> INTEGER HEX
%token<sval> STRING COMMENT LABEL GLOBAL EXTERN SECTION WORD SKIP
%type<ival> HALT
%type<ival> INT INSTR CALL

%type<sval> REGISTER

/* rules */
%%

file:
  template footer {
    if(!firstPass){
      Assembler::currSection->setSize(Assembler::locationCounter);
    }
    Assembler::locationCounter = 0;
    Assembler::currSection = nullptr;
    cout << "Done with a file!" << endl;
    YYACCEPT; //ignorise sve posle .end
  };
template:
  lines;
lines:
  lines line
  | line;
line:
  ext_glob ENDLS
  |
  word ENDLS {
    cout << "Word directive" << endl;
  }
  |
  SECTION STRING ENDLS {
    //cout << "New section: " << $2 << endl;
    Section *s = new Section($2);
    if(firstPass){
      string n = ".";
      Symbol* sym = new Symbol(n+$2, s, false); //dodavanje i u tabelu simbola
      Assembler::addSymbol(sym);
      if(!Assembler::hasSection(s)) Assembler::addSection(s);
      else{
        cerr << "Section " << $2 << " already exists" << endl;
        exit(-1);
      }
      if(Assembler::currSection) Assembler::currSection->setSize(Assembler::locationCounter);
      Assembler::currSection = Assembler::getSection($2);
      Assembler::locationCounter = 0;
    }else{
      delete s;
     // if(Assembler::currSection) Assembler::currSection->setSize(Assembler::locationCounter);
      //dodavanje bazena literala u masinski kod
      Assembler::addPoolData(Assembler::currSection->getName()); //da li povecavam section size?
      Assembler::currSection = Assembler::getSection($2);
      Assembler::locationCounter = 0;
    }
    free($1);
    free($2);
  }
  | SKIP INTEGER ENDLS{
    Assembler::locationCounter += $2;
    if(!firstPass){
      string s = "";
      for(int i = 0; i < $2; i++){
        s += "00"; //literal odredjuje koliko bajtova nula se upisuje
      }
      Assembler::addData(s);
    }
    free($1);
  }
  | COMMENT ENDLS {
    //cout << "Found a comment " << endl;
  }
  | STRING COLON ENDLS {
    Symbol* s = new Symbol($1, nullptr, false);
    if(firstPass){
      s->setSection(Assembler::currSection);
      s->setOffset(Assembler::locationCounter);
      if(!Assembler::getSymbol($1)){
        Assembler::addSymbol(s); //mozda je vec dodat sa global
      }
      else{
        delete s;
        Assembler::getSymbol($1)->setSection(Assembler::currSection);
        Assembler::getSymbol($1)->setOffset(Assembler::locationCounter);
      }
    }
    else{
      s = Assembler::getSymbol($1);
     // s->setSection(Assembler::currSection);
      s->setOffset(Assembler::locationCounter);
    }
    free($1);
  }
  |
  STRING COLON {
    Symbol* s = new Symbol($1, nullptr, false);
    if(firstPass) {
      s->setSection(Assembler::currSection);
      s->setOffset(Assembler::locationCounter);
      Assembler::addSymbol(s);
    }
    else{
      s = Assembler::getSymbol($1);
     // s->setSection(Assembler::currSection);
      
    }
    free($1);
  }
  | INSTR HEX ENDLS { //za jmp
    //drugi mod da bi se pristupilo bazenu literala
    int opcode = $1 | 8;
    int hex = $2;
    string h = to_string(hex);
    if(firstPass){
      if(Assembler::getFromPool(h) == nullptr){ //ako nije vec dodat
        LiteralSym* l = new LiteralSym(h, hex);
        Assembler::addToPool(l);
      }
    }else{
      //upisuje se pomeraj do bazena gde se nalazi literal
      //a literal ce se na kraju dodati u masinski kod
      string s = "";
      int offset = Assembler::currSection->getSize() + Assembler::getFromPool(h)->getOffset() - 4 - Assembler::locationCounter;
      //cout << "Offset to pool: " << offset << endl;
      if(offset > 4095){ //displacement moze biti samo pozitivan
        cerr << "Offset greater than 2^12 - 1" << endl;
        exit(-1);
      }
      s = s + Assembler::prepareOffset(offset, "r0");
      s = s + "f0" + Assembler::prepareData(opcode);
      Assembler::addData(s);
    }
    Assembler::locationCounter += 4;
  }
  | INSTR STRING ENDLS {
    //prvi mod ostaje ako je simbol iz iste sekcije
    //u suprotnom oruje se opcode sa 8 da se dobije drugi mod
    if(!firstPass){
      if(!Assembler::getSymbol($2)){
      cerr << "On line " << linenum << ": symbol '" << $2 << "' not defined" << endl;
      exit(-1);
      }else{
        if(Assembler::getSymbol($2)->getSection() == Assembler::currSection){
          //prvi mod ostaje, u displacement pomeraj do simbola
          int displacement = Assembler::getSymbol($2)->getOffset() - Assembler::locationCounter - 4;
          string s = "";
          s = s + Assembler::prepareOffset(displacement, "r0");
          s = s + "f0" + Assembler::prepareData($1);
          Assembler::addData(s);
        }else{
          //oruje se sa 8 i ostavlja relokacioni zapis
          int opcode = $1 | 8;
        }
      }
    }
    Assembler::locationCounter += 4;
    free($2);
  }
  | INSTR PERCENT REGISTER ENDLS {
    Assembler::locationCounter += 4;
    if(!firstPass){
      if($1 == Assembler::NOT){
        string s = "0000";
        s = s + Assembler::prepareRegister($3) + Assembler::prepareRegister($3);
        s += Assembler::prepareData($1);
        Assembler::addData(s);  
      }else if($1 == Assembler::POP){
        string s = "0400";
        s = s + Assembler::prepareRegister($3) + "e";
        s += Assembler::prepareData($1);
        Assembler::addData(s);
      }else{
        //push
        string s = "fc" + Assembler::prepareRegister($3) + "f"; //-4 je displacement
        s = s + "e0";
        s += Assembler::prepareData($1);
        Assembler::addData(s);
      }
    }
    free($3);
  }
  | INSTR PERCENT REGISTER COMMA PERCENT REGISTER COMMA HEX ENDLS {
    //branch sa adresom
    int hex = $8;
    int opcode = $1 | 8;
    string h = to_string(hex);
    if(firstPass){
      if(Assembler::getFromPool(h) == nullptr){ //ako nije vec dodat
        LiteralSym* l = new LiteralSym(h, hex);
        Assembler::addToPool(l);
      }
    }else{
      //upisuje se pomeraj do bazena gde se nalazi literal
      //a literal ce se na kraju dodati u masinski kod
      string s = "";
      int offset = Assembler::currSection->getSize() + Assembler::getFromPool(h)->getOffset() - 4 - Assembler::locationCounter;
      //cout << "Offset to pool: " << offset << endl;
      if(offset > 4095){ //displacement moze biti samo pozitivan
        cerr << "Offset greater than 2^12 - 1" << endl;
        exit(-1);
      }
      s = s + Assembler::prepareOffset(offset, $6);
      s = s + "f" + Assembler::prepareRegister($3) + Assembler::prepareData(opcode);
      Assembler::addData(s);
    }
    Assembler::locationCounter += 4;
  }
  | INSTR PERCENT REGISTER COMMA PERCENT REGISTER COMMA STRING ENDLS {
    //branch sa simbolom
    //prvi mod ostaje ako je simbol iz iste sekcije
    //u suprotnom oruje se opcode sa 8 da se dobije drugi mod
    if(!firstPass){
      if(!Assembler::getSymbol($8)){
      cerr << "On line " << linenum << ": symbol '" << $8 << "' not defined" << endl;
      exit(-1);
      }else{
        if(Assembler::getSymbol($8)->getSection() == Assembler::currSection){
          //prvi mod ostaje, u displacement pomeraj do simbola
          int displacement = Assembler::getSymbol($8)->getOffset() - Assembler::locationCounter - 4;
          string s = "";
          s = s + Assembler::prepareOffset(displacement, $6);
          s = s + "f" + Assembler::prepareRegister($3) + Assembler::prepareData($1);
          Assembler::addData(s);
        }else{
          //oruje se sa 8 i ostavlja relokacioni zapis
          int opcode = $1 | 8;
        }
      }
    }
    Assembler::locationCounter += 4;
    free($8);
  }
  | INSTR PERCENT REGISTER COMMA PERCENT REGISTER ENDLS {
    // add %r1, %r2 ili ove sa csrrd, csrwr
    Assembler::locationCounter += 4;
    if(!firstPass) { //radi za aritmeticke, logicke(bez not) i pomeracke
      string s = "00" + Assembler::prepareRegister($3) + "0";
      if($1 != Assembler::XCHG) s = s + Assembler::prepareRegister($6) + Assembler::prepareRegister($6);
      else s = s + Assembler::prepareRegister($6) + "0"; //kod xchg je AAAA deo 0
      s += Assembler::prepareData($1);
      Assembler::addData(s);
    }
    free($3); free($6);
  }
  | INSTR DOLLAR INTEGER COMMA PERCENT REGISTER ENDLS {
    // ld $5, %r3
    cout << "Instruction opcode: " << $1 << endl; //treba orovati sa modom i videti da nije slucaj kada
    //zauzima 2 instrukcije, odnosno 8 bajta
    cout << "Register: " << $6  << endl;
    Assembler::locationCounter += 4;
    free($6);
  }
  | INSTR PERCENT REGISTER COMMA DOLLAR INTEGER ENDLS {
    // st %r3, $5
    cout << "Instruction opcode: " << $1 << endl;
    cout << "Register: " << $3  << endl;
    Assembler::locationCounter += 4;
    free($3);
  }
  | CALL HEX ENDLS {
    //ovaj hex se dodaje u tabelu literala
    //oruje se opcode sa 1 da bi bio drugi mod
    int hex = $2;
    cout << "Hex address: " << hex << endl;
    int opcode = $1 | 1;
    string h = to_string(hex);
    if(firstPass){
      if(Assembler::getFromPool(h) == nullptr){ //ako nije vec dodat
        LiteralSym* l = new LiteralSym(h, hex);
        Assembler::addToPool(l);
      }
    }else{
      //upisuje se pomeraj do bazena gde se nalazi literal
      //a literal ce se na kraju dodati u masinski kod
      string s = "";
      int offset = Assembler::currSection->getSize() + Assembler::getFromPool(h)->getOffset() - 4 - Assembler::locationCounter;
      //cout << "Offset to pool: " << offset << endl;
      if(offset > 4095){ //displacement moze biti samo pozitivan
        cerr << "Offset greater than 2^12 - 1" << endl;
        exit(-1);
      }
      s = s + Assembler::prepareOffset(offset, "r0");
      s = s + "f0" + Assembler::prepareData(opcode);
      Assembler::addData(s);
    }
    Assembler::locationCounter += 4;
  }
  | CALL STRING ENDLS {
    //ako je labela u istoj sekciji kao trenutna, prvi mod i odredi se displacement
    //ako nije, drugi mod i upisu se nule pa se ostavi relokacioni zapis
    if(!firstPass){
      if(!Assembler::getSymbol($2)){
      cerr << "On line " << linenum << ": symbol '" << $2 << "' not defined" << endl;
      exit(-1);
      }else{
        if(Assembler::getSymbol($2)->getSection() == Assembler::currSection){
          //prvi mod ostaje, u displacement pomeraj do simbola
          int displacement = Assembler::getSymbol($2)->getOffset() - Assembler::locationCounter - 4;
          string s = "";
          s = s + Assembler::prepareOffset(displacement, "r0");
          s = s + "f0" + Assembler::prepareData($1);
          Assembler::addData(s);
        }else{
          //oruje se sa 1 i ostavlja relokacioni zapis za bazen literala
          //u displacement ide pomeraj do bazena
          int opcode = $1 | 1;
          string s = "";
          int offset = Assembler::currSection->getSize() + Assembler::getFromPool($2)->getOffset() - 4 - Assembler::locationCounter;
          if(offset > 4095){ //displacement moze biti samo pozitivan
            cerr << "Offset greater than 2^12 - 1" << endl;
            exit(-1);
          }
          s = s + Assembler::prepareOffset(offset, "r0");
          s = s + "f0" + Assembler::prepareData(opcode);
          Assembler::addData(s);
          //dodati relokacioni zapis
          int off = Assembler::currSection->getSize() + Assembler::getFromPool($2)->getOffset();
          Assembler::addRelocation(off, Assembler::getSymbol($2));
        }
      }
    }else{ //prvi prolaz
      //moram dodati simbol u bazen iako mozda nije potreban
      string h = $2;
      if(Assembler::getFromPool(h) == nullptr){ //ako nije vec dodat
        LiteralSym* l = new LiteralSym(h, 0);
        Assembler::addToPool(l);
      }
    }
    Assembler::locationCounter += 4;
    //free($2);
  }
  | INSTR ENDLS {
    if($1 == Assembler::HALT || $1 == Assembler::INT){
      if(!firstPass){
        string s = "000000" + Assembler::prepareData($1);
        Assembler::addData(s);
      }
      Assembler::locationCounter += 4;
    }else if($1 == Assembler::RET){
      if(!firstPass){
        string s = "0400fd" + Assembler::prepareData($1);
        Assembler::addData(s);
      }
      Assembler::locationCounter += 4;
    }else{
        //IRET
      Assembler::locationCounter += 12; //3 instrukcije
    }
  }
  | STRING ENDLS {
    cout << "Neki string: " << $1 << endl;
    free($1);
  }
  ;
footer:
  END ENDLS
  | END;
ENDLS:
  ENDLS ENDL
  | ENDL;

ext_glob:
  extern_list
  ;
extern_list:
  EXTERN {
    cout << "Extern symbols: " << endl;
  } extern_loop
  | GLOBAL {
    cout << "Global symbols: " << endl;
  } extern_loop
  ;
extern_loop:
  STRING {
    cout << $1 << endl;
    Symbol *s = new Symbol($1, nullptr, false);
    if(firstPass) Assembler::addSymbol(s);
    else{
      s = Assembler::getSymbol($1);
      //s->setSection(Assembler::currSection);
      s->setOffset(Assembler::locationCounter);
      s->setGlobal();
    }
    free($1);
  }
  | extern_loop COMMA STRING{
    cout << $3 << endl;
    Symbol *s = new Symbol($3, nullptr, false);
    if(firstPass) Assembler::addSymbol(s);
    else{
      s = Assembler::getSymbol($3);
      //s->setSection(Assembler::currSection);
      s->setOffset(Assembler::locationCounter);
      s->setGlobal();
    }
    free($3);
  };

word:
  word_list;
word_list:
  WORD word_loop;
word_loop:
  INTEGER {
    Assembler::locationCounter += 4;
    string s = "";
    if(!firstPass){
      s = Assembler::prepareLiteral($1);
      Assembler::addData(s);
    }
  }
  | word_loop COMMA INTEGER{
    Assembler::locationCounter += 4;
    string s = "";
    if(!firstPass){
      s = Assembler::prepareLiteral($3);
      Assembler::addData(s);
    }
  };
%%

int main(int argc, char** argv){
  FILE* myFile = fopen("in.snazzle", "r");

  if(!myFile){
    cout << "Ne postoji fajl" << endl;
    return -1;
  }

  yyin = myFile;

  yyparse();
  cout << linenum << endl;
  cout << "LC (first pass): " << Assembler::locationCounter << endl;
  fseek(myFile, 0, SEEK_SET); //drugi prolaz
  firstPass = false;
  linenum = 0;
  yyparse();
  Assembler::printSymbols();
  Assembler::printSections();
  Assembler::printData();
  Assembler::printRelocationData();
  
  Assembler::clear();
}

void yyerror(const char* s){
  cout << "ERROR on line " << linenum << ": " << s << endl;
  exit(-1);
}