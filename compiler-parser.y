%error-verbose
%locations

%{
   #include <stdio.h>
   #include <stdlib.h>
   #include <iostream>
   #include "botscript.h"
   using namespace std;

   int yylex();
   extern int yyparse();
   extern void yyerror( const char* s );
   extern FILE* yyin;
   extern int yylineno;

   BSBlock* script;
%}

%union {
   int ival;
   double dval;
   std::string* sval;

   BSBlock* block;
   BSStatement* stmt;
   BSExpression* expr;
   BSIdentifier* ident;
   BSDictionary* dict;
   BSDictionaryItem* dictitem;
   BSFunctionCall* func;
   BSFunctionDefinition* funcdef;
   BSConditionalBlock* cond;
   BSAssign* assign;
   BSIfBlock* ifstmt;
   BSBinaryOperation* bop;
   vector<BSDictionaryItem*>* dictitemlist;
   ExpressionList* exprlist;
   IdentifierList* identlist;
   int token;
}

%token <ival> TINTEGER
%token <dval> TDOUBLE
%token <sval> TSTRING TIDENTIFIER

%token TEQUAL TNEQUAL TASSIGN TLBRACE TRBRACE TLBRACKET TRBRACKET TSUBSCRIPT
%token TCOLON TCOMMA TNIL TENDL TIF TELSE TRETURN TFOR TIN TOR TAND TLT TLTE TGT
%token TGTE TADD TADD_ASSIGN
%token TEOF 0

%type <block> botscript statements statement_block
%type <stmt> statement add_assign assignment branch
%type <ident> identifier
%type <dict> dictionary dictionary_items
%type <dictitem> dictionary_item
%type <expr> expression constant_expression numeric
%type <exprlist> expression_list
%type <identlist> identifier_list
%type <func> function_call
%type <funcdef> function_definition
%type <cond> for_loop
%type <ifstmt> if_stmt
%type <bop> binary_operation
%type <token> comparison

%left TEQUAL TNEQUAL

%start botscript

%%

botscript : statements TEOF { script = $1; }
          ;

statements : statement { $$ = new BSBlock(); if( $1 ) $$->statements.push_back( $1 ); }
           | statements statement { if( $2 ) $1->statements.push_back( $2 ); }
           ;

statement : assignment TENDL
          | add_assign TENDL
          | expression TENDL { $$ = new BSExpressionStatement( $1 ); }
          | branch TENDL
          | TRETURN TENDL { $$ = new BSReturn(); }
          | TRETURN expression TENDL { $$ = new BSReturn( $2 ); }
          | TENDL { $$ = 0; }
          ;

add_assign : identifier TADD_ASSIGN expression { $$ = new BSAddAssign( $<ident>1, $<expr>2 ); }
           ;

assignment : identifier TASSIGN expression { $$ = new BSAssign( $1, $3 ); }
           | identifier TASSIGN function_definition { $$ = new BSAssign( $1, $3 ); }
           ;

function_definition : TLBRACKET identifier_list TRBRACKET TCOLON statement_block { $$ = new BSFunctionDefinition( $2, $5 ); }
                    ;

identifier : TIDENTIFIER { $$ = new BSIdentifier( *$1 ); }
           | TIDENTIFIER TSUBSCRIPT TIDENTIFIER { $$ = new BSIdentifier( *$1, *$3 ); }
           ;

identifier_list : empty      { $$ = new IdentifierList(); }
                | identifier { $$ = new IdentifierList(); $$->push_back( $1 ); }
                | identifier_list TCOMMA identifier { $1->push_back( $3 ); }
                ;

function_call : identifier TLBRACKET TRBRACKET { $$ = new BSFunctionCall( $1, new ExpressionList() ); }
              | identifier TLBRACKET expression_list TRBRACKET { $$ = new BSFunctionCall( $1, $3 ); }
              ;

expression_list : expression { $$ = new ExpressionList(); $$->push_back( $1 ); }
                | expression_list TCOMMA expression { $1->push_back( $3 ); }
                ;

expression : function_call { $$ = $1; }
           | constant_expression { $$ = $1; }
           | binary_operation { $$ = $1; }
           | dictionary { $$ = $1; }
           | identifier { $$ = $1; }
           | expression TADD expression { $$ = new BSAdd( $1, $3 ); }
           ;

dictionary : TLBRACE TRBRACE { $$ = new BSDictionary(); }
           | TLBRACE dictionary_items TRBRACE { $$ = $2; }
           ;

empty :
      | TENDL
      ;

dictionary_items : empty dictionary_item empty { $$ = new BSDictionary(); $$->addItem( $2 ); }
                 | empty dictionary_item empty TCOMMA empty dictionary_items { $$ = new BSDictionary(); $$->addItem( $2 ); }
                 ;

dictionary_item : identifier TCOLON expression { $$ = new BSDictionaryItem( $1, $3 ); }
                ;

constant_expression : TSTRING { $$ = new BSString( *$1 ); }
                    | numeric { $$ = $1; }
                    ;

comparison : TEQUAL | TNEQUAL | TAND | TOR | TGT | TGTE | TLT | TLTE ;

binary_operation : expression comparison expression { $$ = new BSBinaryOperation( $1, $3, $2 ); }
                 ;

numeric : TDOUBLE { $$ = new BSDouble( $1 ); }
        | TINTEGER { $$ = new BSInteger( $1 ); }
        | TNIL { $$ = new BSNil(); }
        ;

branch : if_stmt { $$ = $1; }
       | for_loop { $$ = $1; }
       ;

for_loop : TFOR identifier_list TIN expression statement_block { $$ = new BSForEachLoop( $2, $4, $5 ); }

if_stmt : TIF expression statement_block { $$ = new BSIfBlock( $2, $3 ); }
        | if_stmt TELSE statement_block { $1->else_statements = $3; }
        | if_stmt TELSE if_stmt { $1->else_statements->statements.push_back( $3 ); }
        ;

statement_block : TLBRACE statements TRBRACE { $$ = $2; }

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

   script->ensure_return();

   ostringstream oss;
   oss << "#main FUNC 0" << endl;
   script->codegen( oss );
   SymbolRegistrar::codegen( oss );

   cout << oss.str();
}

void yyerror( const char* s ) {
   printf( "autobot.bs:%d: %s\n", yylineno, s );
   exit( -1 );
}

