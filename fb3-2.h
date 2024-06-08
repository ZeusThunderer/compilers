#ifndef TEST
#define TEST



/* Companion source code for "flex & bison", published by O'Reilly
 * Media, ISBN 978-0-596-15597-1
 * Copyright (c) 2009, Taughannock Networks. All rights reserved.
 * See the README file for license conditions and contact info.
 * $Header: /home/johnl/flnb/code/RCS/fb3-2.h,v 2.1 2009/11/08 02:53:18 johnl Exp $
 */
/*
 * Declarations for a calculator fb3-1
 */

/* symbol table */
struct symbol {		/* a variable name */
  char *name;
  int value;
};

/* simple symtab of fixed size */
#define NHASH 9997
extern struct symbol symtab[NHASH];

struct symbol *lookup(char*);

/* list of symbols, for an argument list */
struct symlist {
  struct symbol *sym;
  struct symlist *next;
};

struct symlist *newsymlist(struct symbol *sym, struct symlist *next);
void symlistfree(struct symlist *sl);

/* node types
 *  + - * / |
 *  0-7 comparison ops, bit coded 04 equal, 02 less, 01 greater
 *  M unary minus
 *  L statement list
 *  I IF statement
 *  E ELIF part 
 *  W WHILE statement
 *  = assignment
 *  D declaration
 *  
 */ 

enum bifs {			/* built-in functions */
  B_sqrt = 1,
  B_exp,
  B_log,
  B_print
};

/* nodes in the Abstract Syntax Tree */
/* all have common initial nodetype */

struct ast {
  int nodetype;
  struct ast *l;
  struct ast *r;
};


struct ifast {
  int nodetype;			/* type I or W */
  struct ast *cond;		/* condition */
  struct ast *tl;		/* then or do list */
  struct ast *elif;
  struct ast *el;		/* optional else list */
};



struct forast { 
  int nodetype;
  struct ast *init;
  struct ast *cond;
  struct ast *upd;
  struct ast *tl;
};

struct numval {
  int nodetype;			/* type K */
  double number;
};

struct symref {
  int nodetype;			/* type N */
  struct symbol *s;
};

struct symasgn {
  int nodetype;			/* type = */
  struct symbol *s;
  struct ast *v;		/* value */
};

/* build an AST */
struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newref(struct symbol *s);
struct ast *newasgn(struct symbol *s, struct ast *v);
struct ast *newnum(double d);
struct ast *newif(struct ast *cond, struct ast *tl,struct ast *elif, struct ast *el);
struct ast *newelif(struct ast *prev, struct ast *exp, struct ast *tail);
struct ast *newfor(struct ast *init, struct ast *cond, struct ast *upd, struct ast* tl);

/* define a function */
void dodef(struct symbol *name, struct symlist *syms, struct ast *stmts);

/* evaluate an AST */
double eval(struct ast *);

/* delete and free an AST */
void treefree(struct ast *);

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(char *s, ...);

extern int debug;
void dumpast(struct ast *a, int level);

#endif