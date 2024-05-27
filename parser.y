/* Companion source code for "flex & bison", published by O'Reilly
 * Media, ISBN 978-0-596-15597-1
 * Copyright (c) 2009, Taughannock Networks. All rights reserved.
 * See the README file for license conditions and contact info.
 * $Header: /home/johnl/flnb/code/RCS/fb3-2.y,v 2.1 2009/11/08 02:53:18 johnl Exp $
 */
/* calculator with AST */

%{
#  include <stdio.h>
#  include <stdlib.h>
#  include "fb3-2.h"
%}

%union {
  struct ast *a;
  int intVal;
  struct symbol *s;		/* which symbol */
  struct symlist *sl;
  int fn;			/* which function */
}

/* declare tokens */
%token <intVal> NUMBER
%token <s> NAME
%token <fn> FUNC
%token <fn> CMP 

%token IF ELSE FOR INT AND OR NOT 


%nonassoc <fn> CMP
%right '='
%left '+' '-'
%left '*' '/'
%nonassoc NOT UMINUS

%type <a>program exp statement statements if_statement for_statement while_statement assigment declaration tail else_if_part else_part 
%type <sl> symlist

%start calclist

%%
program: 
        statements                
;
statements: 
            statements statement  {$$ = newast('L', $1, $2);}
          | statement             
;

statement:
          if_statement            
        | for_statement           
        | while_statement         
        | assigment               
        | declaration             
;


exp: exp CMP exp          { $$ = newcmp($2, $1, $3); }
   | exp '+' exp          { $$ = newast('+', $1,$3); }
   | exp '-' exp          { $$ = newast('-', $1,$3);}
   | exp '*' exp          { $$ = newast('*', $1,$3); }
   | exp '/' exp          { $$ = newast('/', $1,$3); }
   | exp AND exp          { $$ = newast('&', $1,$3); }
   | exp OR exp           { $$ = newast('|', $1,$3); }
   | NOT exp              { $$ = newast('!', $1,$3); }
   | '(' exp ')'          { $$ = $2; }
   | '-' exp %prec UMINUS { $$ = newast('M', $2, NULL); }
   | NUMBER               { $$ = newnum($1); }
   | NAME                 { $$ = newref($1); }
;

declaration: 
            INT NAME ';'         {$$ = newast('D', $2, NULL); }
        |   INT NAME '=' exp ';' {$$ = newast('D', $2, $4); }     
;

assigment: 
            NAME '=' exp ';'    { $$ = newasgn($1, $3); }
;



tail:       
            statement         
      | '{' statements '}'    { $$ = $2; }
;

if_statement: 
            IF '(' exp ')' tail else_if_part else_part  {$$ = newif($3,$5, $6, $7)} 
;

else_if_part: 
            else_if_part ELSE IF '(' exp ')' tail       { $$ = newelif($1, $5, $7); }
          | ELSE IF '(' exp ')' tail                    { $$ = newast('E', $4, $6); }
          | 
;

else_part: 
            ELSE tail         { $$ = $2; }
          |  
; 

while_statement:
            WHILE '(' exp ')' tail             { $$ = newast('W', $3, $5); }

for_statement: 
            FOR '(' declaration exp ';' exp ')' tail  { $$ = newfor($3, $4, $6, $8) }
;

%%

