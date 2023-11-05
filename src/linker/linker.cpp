#include "../../inc/linker/linker.hpp"

Linker::Linker(string output, list<string> inputs){
  this->output = output;
  this->inputs = inputs;
  this->memory = 0;
  this->sectionList.push_back(new Section(-1, "UND", ""));
}

Section* Linker::getSection(string s, string file){
  for(Section* el: this->sectionList){
    if(el->getFileName().compare(file) == 0 && 
    el->getName().compare(s) == 0) return el;
  }
  return nullptr;
}

void Linker::addSections(list<Section*> s){
  while(!s.empty()){
    this->sectionList.push_back(s.front());
    s.pop_front();
  }
}

Symbol* Linker::getSymbol(string s, string file){
  for(Symbol* el: this->symbolList){
    if(el->getFileName().compare(file) == 0 && 
    el->getName().compare(s) == 0) return el;
  }
  return nullptr;
}

void Linker::symValidation(Symbol* sym){
  string section = sym->getNameOfSection();
  string name = sym->getName();
  bool isGlobal = sym->isGlobalSymbol();
  for(Symbol* el: this->symbolList){
    //treba proveriti da li postoje dva lokalna simbola istog imena u istoj sekciji
    if(section.compare(el->getNameOfSection()) == 0 && el->getNameOfSection().compare("UND") != 0 &&
    name.compare(el->getName()) == 0 && section.compare(name) != 0){ //ne vazi za ime sekcije
      cerr << "Two symbols with same name in section " << section << endl;
      cerr << "Symbol: " << el->getName() << endl;
      cerr << el->getFileName() << endl;
      exit(-1);
    }
    //ne sme biti ni dva globalna simbola istog imena
    if(isGlobal && el->isGlobalSymbol() && name.compare(el->getName()) == 0 &&
    section.compare("UND") != 0 && el->getNameOfSection().compare("UND") != 0){
      cerr << "Two global symbols with same name " << name << endl;
      exit(-1);
    }
  }
}

void Linker::addSymbols(list<Symbol*> s){
  while(!s.empty()){
    Symbol* el = s.front();
    //proveriti ispravnost simbola
    this->symValidation(el);
    Section* sec = this->getSection(el->getNameOfSection(), el->getFileName());
    el->setSection(sec);

    this->symbolList.push_back(el);
    s.pop_front();
  }
}

void Linker::addRelSymbols(list<RelocationSymbol*> rs){
  while(!rs.empty()){
    RelocationSymbol* r = rs.front();
    Symbol* s = this->getSymbol(r->getNameOfSymbol(), r->getNameOfFile());
    r->setSymbol(s);
    this->relocationList.push_back(r);
    rs.pop_front();
  }
}

void Linker::resolveSymbolsU(){
  for(Symbol* el: this->symbolList){
    if(el->getNameOfSection().compare("UND") == 0){
      //pokusaj da se pronadje globalni simbol koji je u nekoj sekciji
      Symbol* glob = this->searchGlobalSymbols(el);
      if(!glob){
        cerr << "Symbol " << el->getName() << " can't be resolved" << endl;
        exit(-1);
      }
      el->setSection(glob->getSection());
      el->setValue(glob->getValue());
    }
  }
}

Symbol* Linker::searchGlobalSymbols(Symbol* s){
  for(Symbol* el: this->symbolList){
    if(el->isGlobalSymbol() && el->getName().compare(s->getName()) == 0 && el->getNameOfSection().compare("UND") != 0){
      return el;
    }
  }
  return nullptr;
}

void Linker::getSecDataFromFile(){
  for(Section* el: this->sectionList){
    Scanner scan = Scanner(el->getFileName());
    if(el->getSize() > 0) scan.getSectionData(el);
  }
}

void Linker::addData(list<char> data, uint32_t mem){
  string word = "";
  uint32_t addr = mem;
  while(!data.empty()){
    char c = data.front();
    this->data.push_back(c);
    //dodavanje u mapu
    word += c;
    if(word.length() == 8){
      this->dataMap.insert(make_pair(addr, word));
      word.clear();
      addr += 4; //dodata 4B
    }
    data.pop_front();
  }
}

void Linker::collectData(){
  for(string el: this->inputs){
    Scanner scanner = Scanner(el);
    list<Section*> sections = scanner.getSectionsFromFile();
    list<Symbol*> symbols = scanner.getSymbolsFromFile();
    list<RelocationSymbol*> relSymbols = scanner.getRelocFromFile();

    this->addSections(sections);
    this->addSymbols(symbols);
    this->addRelSymbols(relSymbols);
  }
  //data svake sekcije
  for(Section* el: this->sectionList){
    Scanner scan = Scanner(el->getFileName());
    if(el->getSize() > 0) scan.getSectionData(el);
  }
}

void Linker::placeSections(){
  //ako je za neku sekciju naveden place prvo idu one, pa onda ostale iza one na najvisoj adresi
  uint32_t memory = 0;
  //list<Section*> visited(this->sectionList);
  //prvo treba proci ako ima nekih sa place opcijom
  for(auto pair: this->secPositions){
    Section* sec;
    //treba proveriti da li se prethodna sekcija zavrsila na memorijskoj lokaciji koja pripada sledecoj sekciji
    if(memory >= pair.first){
      cerr << "Sections memory overlap" << endl;
      exit(-1);
    }
    memory = pair.first;
    for(auto itr = this->sectionList.begin(); itr != this->sectionList.end(); itr++){
      sec = *itr;
      if(sec->getName().compare(pair.second) != 0) continue;
      if(sec->getSize() > 0){
        this->addData(sec->getData(), memory);
        Symbol* sym = this->getSymbol(sec->getName(), sec->getFileName());
        sym->setValue(memory);
        sec->setStart(memory);
        memory += sec->getSize();
        sec->setVisited();
      }
    }
  }
  //ako nije bilo place opcije mem ce biti 0
  //ako je bilo, sve idu od te memorijske lokacije do koje se stiglo
  for(auto itr = this->sectionList.begin(); itr != this->sectionList.end(); itr++){
    Section* sec = *itr;
    if(!sec->isVisited() && sec->getSize() > 0){
      this->addData(sec->getData(), memory);
      Symbol* sym = this->getSymbol(sec->getName(), sec->getFileName());       
      sym->setValue(memory);
      sec->setStart(memory);
      memory += sec->getSize();
      sec->setVisited();
    }
  }
  if(memory > UINT32_MAX){ //memorijski prostor je 2^32
    cerr << "Memory overflow" << endl;
    exit(-1);
  }
}

void Linker::setPositions(map<uint32_t, string> pos){
  this->secPositions = pos;
}

void Linker::resolve(){
  for(Symbol* el: this->symbolList){
    if(el->getNameOfSection().compare("UND") != 0){
      Section* s = el->getSection();
      el->setValue(s->getStart() + el->getOffset());
    }
  }
}

string Linker::prepareValue(uint32_t val){
  stringstream stream;
  stream << hex << setw(8) << setfill('0') << val;
  string i = stream.str();
  string res = "";
  int ind = i.length();
  res = res + i.at(ind-2) + i.at(ind-1) + i.at(ind-4) + i.at(ind-3) + i.at(ind-6) + i.at(ind-5) + i.at(ind-8) + i.at(ind-7);
  return res;
}

void Linker::resolveRelocations(){
  for(RelocationSymbol* el: this->relocationList){
    //na adresu sekcije + pomeraj relokacionog simbola treba upisati vrednost simbola + addend
    uint32_t val = el->getSymbol()->getValue() + el->getAddend();
    Section* sec = this->getSection(el->getRelSection(), el->getNameOfFile());
    //pogresna sekcija, meni treba sekcija u kojoj je relokacioni zapis!!!!!
    uint32_t address = sec->getStart() + el->getOffset();
    auto itr = this->dataMap.find(address);
    if(itr != this->dataMap.end()){
      itr->second = this->prepareValue(val);
      stringstream stream;
      stream << hex << setw(8) << setfill('0') << itr->first;
    //  cout << stream.str() << ": " << itr->second << endl;
    }
  }
}

void Linker::printSymbols(){
  cout << "SYMBOL_TABLE_START" << endl;
  for(Symbol* el: this->symbolList){
    stringstream stream;
    stream << hex << setw(8) << setfill('0') << el->getValue();
    cout << stream.str() << " " << *el << endl;
  }
  cout << "SYMBOL_TABLE_END" << endl;
}

void Linker::printSections(){
  cout << "SECTION_TABLE_START" << endl;
  for(Section* el: this->sectionList){
    stringstream stream;
    stream << hex << setw(8) << setfill('0') << el->getStart();
    cout << *el << stream.str() << endl;
  }
  cout << "SECTION_TABLE_END" << endl;
}

void Linker::printRelocs(){
  cout << "REL_SYMBOL_TABLE_START" << endl;
  for(RelocationSymbol* el: this->relocationList){
    cout << *el << endl;
  }
  cout << "REL_SYMBOL_TABLE_END" << endl;
}

void Linker::printData(){
  cout << "DATA_START" << endl;
  for(Section* el: this->sectionList){
    if(el->getName().compare("UND") == 0) continue;
    cout << el->getName() + "_START" << endl;
    el->printData();
    cout << el->getName() + "_END" << endl;
  }
  cout << "DATA_END" << endl;
  int cnt = 0;
  ofstream out("proba.txt");
  for(auto pair: this->dataMap){
    stringstream stream;
    stream << hex << setw(8) << setfill('0') << pair.first;
    out << stream.str() << ": " << pair.second << endl;
  }
  out.close();
}

void Linker::printToHexFile(){
  ofstream out(this->output);
  int line = 0;
  uint32_t prev;
  for(auto pair: this->dataMap){
    if(line % 2 == 1 && pair.first - prev != 4){
      line = 0; //da mi ne ispise novu sekciju na pogresno mesto
      out << endl;
    }
    if(line % 2 == 0){
      stringstream stream;
      stream << hex << setw(8) << setfill('0') << pair.first;
      out << stream.str() << ": " << this->prepareWord(pair.second);
    }else{
     out << this->prepareWord(pair.second) << endl;
    }
    line++;
    prev = pair.first;
  }
}

string Linker::prepareWord(string word){
  stringstream stream;
  int cnt = 0;
  for(char el: word){
    stream << el;
    cnt++;
    if(cnt % 2 == 0) stream << " ";
  }
  return stream.str();
}