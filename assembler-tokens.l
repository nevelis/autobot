%option noyywrap
%option yylineno

%{
   #include "assembler-parser.tab.h"

   #define TOK(x) yylval.token = x; return x
   #define TOK2(x) return x
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
"NIL"           { TOK( TNIL ); }
"OR"            { TOK( TOR ); }
"POP"           { TOK( TPOP ); }
"PUSH"          { TOK( TPUSH ); }
"RET"           { TOK( TRET ); }
"STR"           { TOK( TSTR ); }
#({L}|{D})*     { yylval.sval = strdup( yytext ); TOK2( TREF ); }
{D}+\.{D}*      { yylval.dval = strtod( yytext, NULL ); TOK2( TDOUBLEVAL ); }
{D}+            { yylval.ival = strtoll( yytext, NULL, 10 ); TOK2( TINTVAL ); }
\"[^"]*\"       { yylval.sval = strdup( yytext ); TOK2( TSTRVAL ); }
{L}+({L}|{D})*  { yylval.sval = strdup( yytext ); TOK2( TIDENTVAL ); }
{L}+({L}|{D})*(\.{L}+({L}|{D})*)* { yylval.sval = strdup( yytext ); TOK2( TIDENTVAL ); }
(\t|\ )         ;
\r?\n           { /* printf( "\n" ); */ return TENDL; }
.

%%

