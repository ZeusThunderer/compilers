%{
#include "ast.h"
#include <stdio.h>
#include "parser.tab.h"
%}

%option noyywrap

%x COMMENT

%%

[0-9][a-zA-Z][a-zA-Z0-9]* { yyerror("Names cannot start with a digit");}
[0-9]+          { yylval.i = atoi(yytext); return NUMBER; }


    /* tokens for arithmetic and logical operators. */

    /* single character ops */
"+" |
"-" |
"*" |
"/" | 
"=" |  
"," | 
";" | 
"(" | 
")" |
"{" |
"}"           { return yytext[0]; }

    /* comparation ops, all are a CMP token */
">"                 { yylval.cmp = 1; return CMP; }
"<"                 { yylval.cmp = 2; return CMP; }
"!="                { yylval.cmp = 3; return CMP; }
"=="                { yylval.cmp = 4; return CMP; }
">="                { yylval.cmp = 5; return CMP; }
"<="                { yylval.cmp = 6; return CMP; }

    /* keywords */
"if"                { return IF;     }
"else"              { return ELSE;   }
"while"             { return WHILE;  }
"for"               { return FOR;    }
"elif"              { return ELIF;   }

"and"               { return AND;   }
"or"                { return OR;    }
"not"               { return NOT;   }


    /* booleans */
"true"              { yylval.i = 1; return NUMBER; }
"false"             { yylval.i = 0; return NUMBER;  }

     /* comments */

"/*"                { BEGIN(COMMENT); }
<COMMENT>"*/"       { BEGIN(INITIAL); }
<COMMENT>.          { }
<COMMENT>\n         { }
<COMMENT><<EOF>>    { yyerror("Unterminated comment"); }

"//".*              /* single line comment */

    /* names */
[a-zA-Z][a-zA-Z0-9]*    { yylval.s = strdup(yytext); return NAME; }


[ \t]+                  { /* ignore whitespace */ }
\\\n                    {printf("c> "); }  /* ignore line continuation */
\n                      {  }

.                       { printf("Unexpected character: %c\n", yytext[0]); }

%%