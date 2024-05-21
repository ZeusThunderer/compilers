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
  double d;
  struct symbol *s;		/* which symbol */
  struct symlist *sl;
  int fn;			/* which function */
}

/* declare tokens */
%token <d> NUMBER
%token <s> NAME
%token <fn> FUNC
%token EOL

%token IF ELIF THEN ELSE FOR INT AND OR NOT 


%nonassoc <fn> CMP
%right '='
%left '+' '-'
%left '*' '/'
%nonassoc NOT UMINUS

%type <a> exp stmt list explist
%type <sl> symlist

%start calclist

%%
program: 
        statements
;

statements:
            statements statement | statement
;

statement:
          if_statement 
        | for_statement 
        | assigment 
        | declaration
;

declaration: 
            INT NAME ';' 
;

assigment: 
            NAME '=' exp ';' 
;

statements: 
            statements statement 
          | statement
;

tail:       
            statement 
      | '{' statements '}' 
;

if_statement:
            IF '(' exp ')' tail
            (ELSE IF '(' exp ')' tail)*
            (ELSE tail)?
;

for_statement: 
            FOR '(' exp ';' exp ';' exp ')' tail 
;


stmt:  decl                       { $$ = } 
    |  IF exp THEN list           { $$ = newflow('I', $2, $4, NULL); }
    |  IF exp THEN list ELSE list  { $$ = newflow('I', $2, $4, $6); }
    |  FOR exp DO list           { $$ = newflow('W', $2, $4, NULL); }
    |  exp
;

;

list: /* nothing */ { $$ = NULL; }
   | stmt ';' list { if ($3 == NULL)
	                     $$ = $1;
                      else
                        $$ = newast('L', $1, $3);
                    }
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
   | NAME '=' exp         { $$ = newasgn($1, $3); }
;


;
symlist: NAME       { $$ = newsymlist($1, NULL); }
 | NAME ',' symlist { $$ = newsymlist($1, $3); }
;

calclist: /* nothing */
  | calclist stmt ';' {
    if(debug) dumpast($2, 0);
     printf("= %4.4g\n> ", eval($2));
     treefree($2);
    }
  | calclist INT NAME '(' symlist ')' '=' list ';' {
                       dodef($3, $5, $8);
                       printf("Defined %s\n> ", $3->name); }

  | calclist error ';' { yyerrok; printf("> "); }
 ;
%%

