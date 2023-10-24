#ifndef SECTION_HPP_
#define SECTION_HPP_

#include <iostream>
#include <vector>
#include <list>
using namespace std;

class Section{
  private:
    static int cnt;
    string name;
    int id;
    int size;
    string inputFile;
    list<char> data;
    uint32_t startMem;
    bool visited;

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
    string getFileName();
    void setData(list<char> list);
    list<char> getData();
    void printData();
    void setStart(uint32_t mem);
    uint32_t getStart();
    void setVisited(); //za pomoc prilikom rasporeda sekcija
    bool isVisited();
};

#endif