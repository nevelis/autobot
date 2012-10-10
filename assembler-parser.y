%error-verbose
%locations

%{
   #include <stdio.h>

   int yylex();
   extern int yyparse();
   extern void yyerror( const char* s );
   extern FILE* yyin;
   extern int yylineno;
%}

%union {
   long long ival;
   double dval;
   char* sval;
}

%token TADD TAND TASSIGN TCALL TCMP TCMPGT TCMPGTE TCMPLT TCMPLTE TCMPNE TDOUBLE
%token TFUNC TIDENT TINT TJZ TOR TPOP TPUSH TRET TSTR TENDL
%token <sval> TREF TSTRVAL TIDENTVAL
%token <ival> TINTVAL
%token <dval> TDOUBLEVAL

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
           ;

string_definition : TREF TSTR TSTRVAL TENDL
                  ;

function_definition : TREF TFUNC TINTVAL TENDL instructions
                    ;

reference_point : TREF TENDL instructions

instructions : TENDL
             | instruction TENDL
             | instructions instruction TENDL
             ;

instruction : TADD
            | TAND
            | TASSIGN
            | TCALL TIDENTVAL TINTVAL
            | TCMP
            | TCMPGT
            | TCMPGTE
            | TCMPLT
            | TCMPLTE
            | TCMPNE
            | TJZ TREF
            | TOR
            | TPUSH TSTR TREF
            | TPUSH TIDENT TIDENTVAL
            | TPUSH TINT TINTVAL
            | TPUSH TFUNC TREF
            | TPOP TIDENTVAL
            | TRET
            ;

%%

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

   yyin = f;
   do {
      yyparse();
   } while( !feof( yyin ) );
}

void yyerror( const char* s ) {
   printf( "autobot.bs:%d: %s\n", yylineno, s );
   exit( -1 );
}

