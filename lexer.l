%option noyywrap yylineno
%{
#include "fb3.h"
#include "parser.tab.h"
void showError(char *token);
#define YYERROR_VERBOSE 1
%}
%x ML_COMMENT
NUMBER    [-+]?[0-9]+
NAME        [a-zA-Z_][a-zA-Z0-9_]

%%
"int"           {return INT;}
"for"           {return FOR;}
"while"         {return WHILE; }
"if"            {return IF;}
"else"          {return ELSE;}
"and"           {return AND;}
"or"            {return OR;}
"not"           {return NOT;}
">"             { yylval.fn = 1; return CMP; }
"<"             { yylval.fn = 2; return CMP; }
"!="            { yylval.fn = 3; return CMP; }
"=="            { yylval.fn = 4; return CMP; }
">="            { yylval.fn = 5; return CMP; }
"<="            { yylval.fn = 6; return CMP; }
"+" |
"-" |
"*" |
"/" |
"=" |
"|" |
"," |
";" |
"(" |
")" |
"{" |           
"}"             { return yytext[0]; }


"//".*              { }
"/*"		        { BEGIN(ML_COMMENT); }
<ML_COMMENT>"*/"    { BEGIN(INITIAL); }
<ML_COMMENT>[^*\n]+	{ }	
<ML_COMMENT>"*"		{ }	
<ML_COMMENT>"\n"	{ }

{NAME}          { yylval.s = lookup(yytext); return NAME; }
{NUMBER}        { yylval.intVal = atoi(yytext); return NUMBER; }
[ \t\n]+        ;
.               {showError(yytext);}

%%

void showError(char* token) {
    printf("Unknown token: %s\n", token);
}
