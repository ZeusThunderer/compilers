#pragma once

#include <stdio.h>

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(char *s, ...);

/* symbol table */
struct symbol
{ /* a variable name */
    char *name;
    int offset;
};

/* simple symtab of fixes size */
#define NHASH 9997
extern struct symbol symtab[NHASH];

struct symbol *lookup(char *);


static char *cmp2str[] = {
    [1] = ">",
    [2] = "<",
    [3] = "!=",         
    [4] = "==",      
    [5] = ">=",    
    [6] = "<="
};

/* nodes in the abstract syntax tree */
struct ast
{
    int nodetype;
    struct ast *l;
    struct ast *r;
};

struct numval
{
    int nodetype; /* type K for constant */
    int number;
};

struct flow
{
    int nodetype; /* type I or W */
    struct ast *cond; /* condition */
    struct ast *tl;   /* then or do list */
    struct ast *el;   /* optional else list */
};

struct for_loop
{
    int nodetype;
    struct ast *init;
    struct ast *cond;
    struct ast *inc;
    struct ast *tl;
};

struct symref
{
    int nodetype; /* type N */
    struct symbol *s;
};

struct symasgn
{
    int nodetype; /* type = */
    struct symbol *s;
    struct ast *v; /* value */
};

/* build an AST */
struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newcmp(int cmptype, struct ast *l, struct ast *r);
struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el);
struct ast *newfor(struct ast *init, struct ast *cond, struct ast *inc, struct ast *tl);
struct ast *newnum(int d);
struct ast *newref(char *s);
struct ast *newasgn(char *s, struct ast *v);

/* print an AST */
void print_ast(FILE *stream, struct ast *ast, int level);
void print_asm(FILE *stream, struct ast *ast);

/* delete and free an AST */
void treefree(struct ast *);

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(char *s, ...);
