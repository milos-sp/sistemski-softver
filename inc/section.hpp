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

  public:
    Section(string name);
    string getName();
    friend ostream& operator <<(ostream& os, const Section& sec);
    bool operator ==(const Section& sec);
    void setSize(int s);
    int getSize();
};

#endif