%option noyywrap
%option yylineno

%{
   #include "botscript.h"
   #include "compiler-parser.tab.h"

   #define TOK( x ) yylval.token = x; return x
%}

D [0-9]
L [A-Za-z_]

%%

"if"           { TOK( TIF ); }
"else"         { TOK( TELSE ); }
"return"       { TOK( TRETURN ); }
"nil"          { TOK( TNIL ); }
"or"           { TOK( TOR ); }
"and"          { TOK( TAND ); }
"for"          { TOK( TFOR ); }
"in"           { TOK( TIN ); }
"=="           { TOK( TEQUAL ); }
"!="           { TOK( TNEQUAL ); }
"="            { TOK( TASSIGN ); }
"{"            { TOK( TLBRACE ); }
"}"            { TOK( TRBRACE ); }
"["            { TOK( TLBRACKET ); }
"]"            { TOK( TRBRACKET ); }
"."            { TOK( TSUBSCRIPT ); }
":"            { TOK( TCOLON ); }
","            { TOK( TCOMMA ); }
"+="           { TOK( TADD_ASSIGN ); }
"+"            { TOK( TADD ); }
">="           { TOK( TGTE ); }
"<="           { TOK( TGTE ); }
">"            { TOK( TGT ); }
"<"            { TOK( TLT ); }
{L}+({L}|{D})* { yylval.sval = new string( yytext, yyleng ); return TIDENTIFIER; }
#.*$           ; // Comment
(\t|\ )        ; // Ignore whitespace
\r?\n?         { TOK( TENDL ); }
\"[^"]*\"      { yylval.sval = new string( yytext, yyleng ); return TSTRING; }
-?{D}+\.{D}+   { yylval.dval = atof( yytext ); return TDOUBLE; }
-?{D}+         { yylval.ival = atoi( yytext ); return TINTEGER; }
.              ;

%%

