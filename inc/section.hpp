#ifndef SECTION_HPP_
#define SECTION_HPP_

#include <iostream>
using namespace std;

class Section{
  private:
    static int cnt;
    string name;
    int id;
    int size;
    string inputFile;

  public:
    Section(string name);
    Section(int id, string name, string file);
    Section(){}
    string getName();
    friend ostream& operator <<(ostream& os, const Section& sec);
    bool operator ==(const Section& sec);
    void setSize(int s);
    int getSize();
    void binPrint(ostream& os);
};

#endif