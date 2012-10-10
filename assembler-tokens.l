%option noyywrap
%option yylineno

%{
   #include "assembler-parser.tab.h"

   #define TOK(x) printf( #x "\n" ); return x
   #define TOK2(x) printf( #x ": %s\n", yytext ); return x
%}

D [0-9]
L [A-Za-z_]

%%

"ADD"           { TOK( TADD ); }
"AND"           { TOK( TAND ); }
"ASSIGN"        { TOK( TASSIGN ); }
"CALL"          { TOK( TCALL ); }
"CMP"           { TOK( TCMP ); }
"CMPGT"         { TOK( TCMPGT ); }
"CMPGTE"        { TOK( TCMPGTE ); }
"CMPLT"         { TOK( TCMPLT ); }
"CMPLTE"        { TOK( TCMPLTE ); }
"CMPNE"         { TOK( TCMPNE ); }
"DOUBLE"        { TOK( TDOUBLE ); }
"FUNC"          { TOK( TFUNC ); }
"IDENT"         { TOK( TIDENT ); }
"INT"           { TOK( TINT ); }
"JZ"            { TOK( TJZ ); }
"OR"            { TOK( TOR ); }
"POP"           { TOK( TPOP ); }
"PUSH"          { TOK( TPUSH ); }
"RET"           { TOK( TRET ); }
"STR"           { TOK( TSTR ); }
#({L}|{D})*     { yylval.sval = strdup( yytext ); TOK2( TREF ); }
\"[^"]*\"       { yylval.sval = strdup( yytext ); TOK2( TSTRVAL ); }
{D}+\.{D}*      { yylval.dval = atof( yytext ); TOK( TDOUBLEVAL ); }
{D}+            { yylval.ival = atoi( yytext ); TOK( TINTVAL ); }
{L}+({L}|{D})*  { yylval.sval = strdup( yytext ); TOK2( TIDENTVAL ); }
(\t|\ )         ;
\r?\n           { printf( "\n" ); return TENDL; }
.

%%

