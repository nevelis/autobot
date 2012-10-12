%error-verbose
%locations

%{
   #include <stdio.h>
   #include <iostream>
   #include <signal.h>
   #include <stdlib.h>
   #include "bit-stream.h"
   #include "symbol-table.h"
   #include "code-generator.h"

   #define BINARY_MODE

   #define ASSERT( x ) \
      if( ! ( x ) ) { \
         printf( "ASSERT FAILED (" #x "), %s:%d\n", \
                 __FILE__, \
                 __LINE__ ); \
         \
         raise( SIGABRT ); \
      } else {}

   using namespace std;

   int yylex();
   extern int yyparse();
   extern void yyerror( const char* s );
   extern FILE* yyin;
   extern int yylineno;

   void codegen( const Instruction& ins );
%}

%union {
   long long ival;
   double dval;
   char* sval;
   int token;
}

%token TADD TAND TASSIGN TCALL TCMP TCMPGT TCMPGTE TCMPLT TCMPLTE TCMPNE TDOUBLE
%token TFUNC TIDENT TINT TJZ TNIL TOR TPOP TPUSH TRET TSTR TENDL
%token <sval> TREF TSTRVAL TIDENTVAL
%token <ival> TINTVAL
%token <dval> TDOUBLEVAL

%type <token> TFUNC TIDENT TINT TSTR TNIL

%start bsil

%%

bsil : definitions
     ;

definitions : definition
            | definitions definition
            ;

definition : string_definition
           | function_definition
           | reference_point
           | instructions
           ;

string_definition : TREF TSTR TINTVAL TSTRVAL TENDL    { codegen( STRINGDEF( $1, $3, $4 ) ); }
                  ;

function_definition : TREF TFUNC TINTVAL TENDL { codegen( FUNCDEF( $1, $3 ) ); }
                    ;

reference_point : TREF TENDL                   { codegen( REFERENCEDEF( $1 ) ); }

instructions : TENDL
             | instruction TENDL
             | instructions instruction TENDL
             ;

instruction : TADD                             { codegen( ADD() ); }
            | TAND                             { codegen( AND() ); }
            | TASSIGN                          { codegen( ASSIGN() ); }
            | TCALL TIDENTVAL TINTVAL          { codegen( CALL( $2, $3 ) ); }
            | TCMP                             { codegen( CMP() ); }
            | TCMPGT                           { codegen( CMPGT() ); }
            | TCMPGTE                          { codegen( CMPGTE() ); }
            | TCMPLT                           { codegen( CMPLT() ); }
            | TCMPLTE                          { codegen( CMPLTE() ); }
            | TCMPNE                           { codegen( CMPNE() ); }
            | TJZ TREF                         { codegen( JZ( $2 ) ); }
            | TOR                              { codegen( OR() ); }
            | TPUSH TNIL                       { codegen( PUSH( $2, 0.0 ) ); }
            | TPUSH TSTR TREF                  { codegen( PUSH( $2, $3 ) ); }
            | TPUSH TIDENT TIDENTVAL           { codegen( PUSH( $2, $3 ) ); }
            | TPUSH TINT TINTVAL               { codegen( PUSH( $2, $3 ) ); }
            | TPUSH TFUNC TREF                 { codegen( PUSH( $2, $3 ) ); }
            | TPOP TIDENTVAL                   { codegen( POP( $2 ) ); }
            | TRET                             { codegen( RET() ); }
            ;

%%

class BinaryHeader {
public:
   BinaryHeader()
      : num_symbols( 0 ), symtab_loc( 0 ) {}

   void
   update( const SymbolTable& sym_table )
   {
      num_symbols = sym_table.num_symbols();
      symtab_loc = sym_table.offset();
   }

private:
   unsigned short num_symbols;
   unsigned int   symtab_loc;

   friend ostream &
   operator<<( ostream& oss, const BinaryHeader& header )
   {
      // We should be at the start of the stream when we write the header
      ASSERT( oss.tellp() == 0 );

      oss << U16( 0x100 );       // Version
      oss << U16( header.num_symbols ); // Number of symbols needing linked
      oss << U32( header.symtab_loc );  // Location of symbol table
   }
};

SymbolTable sym_table;
stringstream file;

void
codegen( const Instruction& ins )
{
   ins.text( file );
}

void
usage( const char* cmd )
{
   printf( "Usage: %s <filename>\n", cmd );
   exit( -1 );
}

main( int argc, char* argv[] )
{
   if( argc != 2 ) {
      usage( argv[ 0 ] );
   }

   FILE* f = fopen( argv[ 1 ], "r" );
   if( !f ) {
      perror( "fopen" );
      exit( -1 );
   }

   // Reserve space for the header
   BinaryHeader header;
   file << header;

   yyin = f;
   do {
      yyparse();
   } while( !feof( yyin ) );

   sym_table.text( file );
   sym_table.link( file );

   header.update( sym_table );

   // Write the updated header
   file.seekp( 0 );
   file << header;

   cout << file.str();
}

void yyerror( const char* s ) {
   printf( "autobot.bs:%d: %s\n", yylineno, s );
   exit( -1 );
}

