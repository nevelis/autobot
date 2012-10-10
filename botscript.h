#ifndef _BOTSCRIPT_H_
#define _BOTSCRIPT_H_

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include <stdio.h>

using namespace std;

#define D( ... )  /* printf( __VA_ARGS__ ); printf( "\n" ) */

enum Opcode {
   O_PUSH
};

class BSIdentifier;
class BSExpression;
class BSFunctionDefinition;

class SymbolTable {
public:
   static string RegisterSymbol( const string& s );
   static string RegisterSymbol( BSFunctionDefinition* func );
   static string RegisterLabel();

   static void codegen( ostream& oss );

private:
   static string NewSymbolName() {
      ostringstream oss;
      oss << "#" << symnum++;
      return oss.str();
   }

   static size_t symnum;
   static map<string, string> symbols;
};

class BSNode {
public:
   virtual ~BSNode() {
   }

   virtual void codegen( ostream& oss ) {}
};

class BSExpression : public BSNode {};

class BSStatement : public BSNode {};

typedef vector<BSStatement*> StatementList;
typedef vector<BSExpression*> ExpressionList;
typedef vector<BSIdentifier*> IdentifierList;

class BSExpressionStatement : public BSStatement {
public:
   BSExpression* expression;

   BSExpressionStatement( BSExpression* expr ) : expression( expr ) {
      D( "BSExpressionStatement" );
   }

   virtual void codegen( ostream& oss ) {
      expression->codegen( oss );
   }
};

class BSNil : public BSExpression {
public:
   BSNil() { D( "BSNil" ); }

   virtual void codegen( ostream& oss ) {
      oss << "   PUSH NIL" << endl;
   }
};

class BSInteger : public BSExpression {
public:
   long long value;
   BSInteger( long long value_ ) : value( value_ ) { D( "BSInteger: %lld", value ); }

   virtual void codegen( ostream& oss ) {
      oss << "   PUSH INT " << value << endl;
   }
};

class BSDouble : public BSExpression {
public:
   double value;
   BSDouble( double value_ ) : value( value_ ) { D( "BSDouble" ); }
};

class BSString : public BSExpression {
public:
   string value;
   BSString( const string& value_ ) : value( value_ ) { D( "BSString: '%s'", value.c_str() ); }

   virtual void codegen( ostream& oss ) {
      oss << "   PUSH STR " << SymbolTable::RegisterSymbol( value ) << endl;
   }
};

class BSBlock : public BSExpression {
public:
   StatementList statements;
   BSBlock() { D( "BSBlock" ); }

   virtual void codegen( ostream& oss ) {
      StatementList::iterator it;
      for( it = statements.begin(); it != statements.end(); ++it ) {
         if( *it ) {
            ( *it )->codegen( oss );
         } else {
            oss << "   NOP???" << endl;
         }
      }
   }
};

class BSReturn : public BSStatement {
public:
   BSExpression* expression;
   BSReturn() : expression( new BSNil() ) { D( "BSReturn" ); }
   BSReturn( BSExpression* expr ) : expression( expr ) { D( "BSReturn expr" ); }

   virtual void codegen( ostream& oss ) {
      expression->codegen( oss );
      oss << "   RET" << endl;
   }
};

class BSIdentifier : public BSExpression {
public:
   string value;
   string subscript;
   BSIdentifier( const string& value_ ) : value( value_ ) { D( "BSIdentifier: %s", value.c_str() ); }
   BSIdentifier( const string& value_, const string& sub_ )
      : value( value_ ), subscript( sub_ ) { D( "BSIdentifier" ); }

   virtual void codegen( ostream& oss ) {
      oss << "   PUSH IDENT " << value;
      if( subscript.size() ) {
         oss << "." << subscript;
      }
      oss << endl;
   }

   bool operator < ( const BSIdentifier& other ) const
   {
      return ( value == other.value )
         ? subscript < other.subscript
         : value < other.value;
   }
};

class BSDictionaryItem : public BSExpression {
public:
   BSIdentifier* key;
   BSExpression* value;

   BSDictionaryItem( BSIdentifier* k, BSExpression* v )
      : key( k ), value( v ) { D( "BSDictionaryItem" ); }
};

class BSDictionary : public BSExpression {
public:
   map<BSIdentifier*,BSExpression*> dict;
   BSDictionary() { D( "BSDictionary" ); }

   void addItem( const BSDictionaryItem* item ) {
      dict[ item->key ] = item->value;
   }
};

class BSFunctionCall : public BSExpression {
public:
   BSIdentifier* function_name;
   ExpressionList* args;

   BSFunctionCall( BSIdentifier* fname_, ExpressionList* args_ )
      : function_name( fname_ ), args( args_ ) { D( "BSFunctionCall" ); }

   virtual void codegen( ostream& oss ) {
      // Evaluate args in reverse order & put on the stack
      for( ExpressionList::reverse_iterator it = args->rbegin(); it !=
         args->rend(); ++it ) {
         if( *it ) {
            ( *it )->codegen( oss );
         } else {
            printf( "Null statement\n" );
         }
      }
      oss << "   CALL " << function_name->value << " " << args->size() << endl;
   }
};

class BSFunctionDefinition : public BSExpression {
public:
   IdentifierList* args;
   BSBlock* body;

   BSFunctionDefinition( IdentifierList* args_,
                         BSBlock* body_ )
      : args( args_ ), body( body_ ) { }

   virtual void codegen( ostream& oss ) {
      string symname = SymbolTable::RegisterSymbol( this );
      oss << "   PUSH FUNC " << symname << endl;
   }
};

class BSConditionalBlock : public BSStatement {
public:
   BSExpression* expression;
   BSBlock* statements;

protected:
   BSConditionalBlock( BSExpression* expr, BSBlock* stmts )
      : expression( expr ), statements( stmts ) {}
};

class BSIfBlock : public BSConditionalBlock {
public:
   BSBlock* else_statements;

   BSIfBlock( BSExpression* expr,
              BSBlock* stmts )
      : BSConditionalBlock( expr, stmts ), else_statements( new BSBlock() ) { D( "BSIfBlock" ); }

   BSIfBlock( BSExpression* expr,
              BSBlock* stmts,
              BSBlock* else_stmts )
      : BSConditionalBlock( expr, stmts ), else_statements( else_stmts ) { D( "BSIfBlock with else" ); }

   virtual void codegen( ostream& oss ) {
      string first = SymbolTable::RegisterLabel();
      string second = SymbolTable::RegisterLabel();

      expression->codegen( oss );
      oss << "   JZ " << first << endl;
      statements->codegen( oss );

      if( else_statements->statements.size() ) {
         oss << "   JZ " << second << endl;
         oss << first << endl;
         else_statements->codegen( oss );
         oss << second << endl;
      } else {
         oss << first << endl;
      }
   }
};

class BSForEachLoop : public BSConditionalBlock {
public:
   IdentifierList* identifiers;

   BSForEachLoop( IdentifierList* idents,
                  BSExpression* expr,
                  BSBlock* stmts )
      : BSConditionalBlock( expr, stmts ), identifiers( idents ) { D( "BSForEachLoop" ); }
};

class BSAddAssign : public BSStatement {
public:
   BSIdentifier* identifier;
   BSExpression* expression;

   BSAddAssign( BSIdentifier* ident, BSExpression* expr )
      : identifier( ident ), expression( expr ) { D( "BSAdd" ); }
};

class BSAdd : public BSExpression {
public:
   BSExpression* a;
   BSExpression* b;

   BSAdd( BSExpression* a_, BSExpression* b_ ) : a( a_ ), b( b_ ) {}

   virtual void codegen( ostream& oss ) {
      a->codegen( oss );
      b->codegen( oss );
      oss << "   ADD" << endl;
   }
};

class BSAssign : public BSStatement {
public:
   BSIdentifier* lhs;
   BSExpression* rhs;

   BSAssign( BSIdentifier* ident, BSExpression* expr )
      : lhs( ident ), rhs( expr ) { D( "BSAssign" ); }

   virtual void codegen( ostream& oss )
   {
      lhs->codegen( oss );
      rhs->codegen( oss );
      oss << "   ASSIGN" << endl;
   }
};

class BSBinaryOperation : public BSExpression {
public:
   BSExpression* a;
   BSExpression* b;
   int op;

   BSBinaryOperation( BSExpression* a_, BSExpression* b_, int op_ )
      : a( a_ ), b( b_ ), op( op_ ) {}

   virtual void codegen( ostream& oss );
};

#endif
