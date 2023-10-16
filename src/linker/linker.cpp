#include "../../inc/linker/linker.hpp"

Section* Linker::prepareSection(string s){
  istringstream sstream(s);
  string word;
  vector<string> v;

  while(sstream >> word){
    v.push_back(word);
  }
  Section* sec;
  if(v.size() == 3){
    sec = new Section(v.at(1));
    sec->setSize(stoi(v.at(2)));
  }

  return sec;
}