#include "botscript.h"
#include "compiler-parser.tab.h"
#include <sstream>
#include <stdlib.h>

size_t SymbolTable::symnum = 0;
map<string, string> SymbolTable::symbols;

string
SymbolTable::RegisterSymbol( const string& s )
{
   string symname = NewSymbolName();
   ostringstream oss;
   oss << "STR " << s << endl;
   symbols[ symname ] = oss.str();
   return symname;
}


string
SymbolTable::RegisterLabel()
{
   return NewSymbolName();
}

string
SymbolTable::RegisterSymbol( BSFunctionDefinition* func )
{
   ostringstream oss;
   oss << "FUNC " << func->args->size() << endl;
   for( IdentifierList::iterator it = func->args->begin(); it !=
      func->args->end(); ++it ) {
      oss << "   POP " << ( *it )->value << endl;
   }
   func->body->codegen( oss );

   string symname = NewSymbolName();
   symbols[ symname ] = oss.str();
   return symname;
}

void
SymbolTable::codegen( ostream& oss )
{
   map<string, string>::iterator it;
   for( it = symbols.begin(); it != symbols.end(); ++it ) {
      oss << it->first << " " << it->second;
   }
}

void
BSBinaryOperation::codegen( ostream& oss )
{
   b->codegen( oss );
   a->codegen( oss );

   switch( op ) {
   case TEQUAL:  oss << "   CMP" << endl; break;
   case TNEQUAL: oss << "   CMPNE" << endl; break;
   case TAND:    oss << "   AND" << endl; break;
   case TOR:     oss << "   OR" << endl; break;
   case TGT:     oss << "   CMPGT" << endl; break;
   case TGTE:    oss << "   CMPGTE" << endl; break;
   case TLT:     oss << "   CMPLT" << endl; break;
   case TLTE:    oss << "   CMPLTE" << endl; break;
   default:
      fprintf( stderr, "Unknown operator: %d\n", op );
      exit( -1 );
   }
}
