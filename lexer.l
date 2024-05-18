%option noyywrap
%option yylineno


%{
#include <stdio.h>
#include <string.h>
#include "parser.tab.h"
void showError(char *token);
#define YYERROR_VERBOSE 1
%}
%x C_COMMENT
NUMBER    [-+]?[0-9]+
//FLOATS      [+-]?([0-9]+[.][0-9]*|[.][0-9]+)
ID        [a-zA-Z_][a-zA-Z0-9_]
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
"=="            {yylval.comp_type = EQUALS_COMP; return COMPARISON;}
"!="            {yylval.comp_type = NOT_EQUALS_COMP; return COMPARISON;}
">"             {yylval.comp_type = GREATER_COMP; return COMPARISON;}
"<"             {yylval.comp_type = LESS_COMP; return COMPARISON;}
">="            {yylval.comp_type = GREATER_EQUALS_COMP; return COMPARISON;}
"<="            {yylval.comp_type = LESS_EQUALS_COMP; return COMPARISON;}
"+"             {yylval.operation_type = ADD_OP; return OPERATION;}
"-"             {yylval.operation_type = SUB_OP; return OPERATION;}
"*"             {yylval.operation_type = MUL_OP; return OPERATION;}
"/"             {yylval.operation_type = DIV_OP; return OPERATION;}
"("             {return LB;}
")"             {return RB;}
"{"             {return LCB;}
"}"             {return RCB;}
";"             {return SEMICOLON;}
"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>\n   { }
<C_COMMENT>.    { }

{ID}       { strcpy(yylval.name,yytext); return NAME; }
{NUMBER}  { yylval.intVal = atoi(yytext); return NUMBER; }
[ \t\n]+    ;
.           {showError(yytext);}

%%

void showError(char* token) {
    printf("Unknown token: %s\n", token);
}
