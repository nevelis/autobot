#include "symbol-table.h"

void
SymbolTable::text( ostream& oss )
{
   symtab_loc = oss.tellp();

   map<string, int>::iterator it;
   for( it = symbols.begin(); it != symbols.end(); ++it ) {
      oss << "REF " << it->first << " " << it->second << endl;
   }
}

void
SymbolTable::link( iostream& ios )
{

}
