%option noyywrap
%option yylineno


%{
#include <stdio.h>
#include <string.h>
#include "parser.tab.h"
void showError(char *token);
#define YYERROR_VERBOSE 1
%}
%x ML_COMMENT
NUMBER    [-+]?[0-9]+
//FLOATS      [+-]?([0-9]+[.][0-9]*|[.][0-9]+)
NAME        [a-zA-Z_][a-zA-Z0-9_]
//STRING     \"([^\\\"]|\\.)*\"

%%
"INT"           {return INT;}
"FOR"           {return FOR;}
"IF"            {return IF;}
"ELSE"          {return ELSE;}
"ELIF"          {return ELIF;}
"AND"           {yylval.logOp = AND_OP; return AND;}
"OR"            {yylval.logOp = OR_OP; return OR;}
"NOT"           {yylval.logOp = NOT_OP; return NOT;}
"="             {return EQUALS;}
">"             { yylval.fn = 1; return CMP; }
"<"             { yylval.fn = 2; return CMP; }
"<>"            { yylval.fn = 3; return CMP; }
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

{NAME}          { strcpy(yylval.name,yytext); return NAME; }
{NUMBER}        { yylval.intVal = atoi(yytext); return NUMBER; }
[ \t\n]+        ;
.               {showError(yytext);}

%%

void showError(char* token) {
    printf("Unknown token: %s\n", token);
}
