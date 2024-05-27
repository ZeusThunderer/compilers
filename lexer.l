%option noyywrap nodefault yylineno
%{
# include "fb3-2.h"
# include "fb3-2.tab.h"
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
"and"           {yylval.logOp = AND_OP; return AND;}
"or"            {yylval.logOp = OR_OP; return OR;}
"not"           {yylval.logOp = NOT_OP; return NOT;}
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
