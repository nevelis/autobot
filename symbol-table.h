#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include <iostream>
#include <map>
using namespace std;

class SymbolTable {
public:
   SymbolTable()
      : symtab_loc( -10 ) {}

   void text( ostream& oss );
   void bytecode( ostream& oss );

   void link( iostream& ios );

   void found_symbol( const string& name, size_t location );
   void found_reference( size_t location, const string& name );

   streampos offset() const { return symtab_loc; }
   int num_symbols() const { return symbols.size(); }

private:
   streampos symtab_loc;

   // Symbols & their locations
   map<string, int> symbols;

   // References to symbols that have not been linked
   map<int, string> references;

};

extern SymbolTable SYMTABLE;

#endif

