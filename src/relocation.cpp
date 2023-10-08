#include "../inc/relocation.hpp"
#include <iomanip>

RelocationSymbol::RelocationSymbol(int o, string t, Symbol* s, int a){
  this->offset = o;
  this->type = t;
  this->sym = s;
  this->addend = a;
}

ostream& operator<<(ostream& os, const RelocationSymbol& rs){
  os << left << setw(8) << hex << rs.offset;
  os << left << setw(16) << rs.type;
  os << rs.sym->getId() << " (" << rs.sym->getName() << ")" << "\t";
  os << left << setw(11) << dec << rs.addend << endl;
  return os;
}