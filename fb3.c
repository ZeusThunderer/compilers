/* Companion source code for "flex & bison", published by O'Reilly
 * Media, ISBN 978-0-596-15597-1
 * Copyright (c) 2009, Taughannock Networks. All rights reserved.
 * See the README file for license conditions and contact info.
 * $Header: /home/johnl/flnb/code/RCS/fb3-2funcs.c,v 2.1 2009/11/08 02:53:18 johnl Exp $
 */
/*
 * helper functions for fb3-2
 */
#  include <stdio.h>
#  include <stdlib.h>
#  include <stdarg.h>
#  include <string.h>
#  include <math.h>
#  include "fb3.h"

/* symbol table */
/* hash a symbol */
static unsigned
symhash(char *sym)
{
  unsigned int hash = 0;
  unsigned c;

  while(c = *sym++) hash = hash*9 ^ c;

  return hash;
}
struct symbol symtab[NHASH];
struct symbol *lookup(char* sym)
{
  struct symbol *sp = &symtab[symhash(sym)%NHASH];
  int scount = NHASH;		/* how many have we looked at */

  while(--scount >= 0) {
    if(sp->name && !strcmp(sp->name, sym)) { return sp; }

    if(!sp->name) {		/* new entry */
      sp->name = strdup(sym);
      sp->value = 0;
      return sp;
    }

    if(++sp >= symtab+NHASH) sp = symtab; /* try the next entry */
  }
  abort(); /* tried them all, table is full */

}



struct ast *newast(int nodetype, struct ast *l, struct ast *r)
{
  struct ast *a = malloc(sizeof(struct ast));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = nodetype;
  a->l = l;
  a->r = r;
  return a;
}

struct ast *newnum(double d)
{
  struct numval *a = malloc(sizeof(struct numval));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = 'K';
  a->number = d;
  return (struct ast *)a;
}

struct ast *newcmp(int cmptype, struct ast *l, struct ast *r)
{
  struct ast *a = malloc(sizeof(struct ast));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = '0' + cmptype;
  a->l = l;
  a->r = r;
  return a;
}

struct ast *
newref(struct symbol *s)
{
  struct symref *a = malloc(sizeof(struct symref));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = 'N';
  a->s = s;
  return (struct ast *)a;
}

struct ast *
newasgn(struct symbol *s, struct ast *v)
{
  struct symasgn *a = malloc(sizeof(struct symasgn));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = '=';
  a->s = s;
  a->v = v;
  return (struct ast *)a;
}

struct ast *
newif(struct ast *cond, struct ast *tl,struct ast *elif, struct ast *el)
{
  struct ifast *a = malloc(sizeof(struct ifast));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = 'I';
  a->cond = cond;
  a->tl = tl;
  a->elif = elif;
  a->el = el;
  return (struct ast *)a;
}

struct ast * 
newelif(struct ast *prev, struct ast *exp, struct ast *tail)
{
    struct ast *next = newast('E', exp, tail);
    return newast('C', prev, next);
}


struct ast * 
newfor(struct ast *init, struct ast *cond, struct ast *upd, struct ast* tl){
    struct forast *a = malloc(sizeof(struct forast));
  
    if(!a) {
      yyerror("out of space");
      exit(0);
    }
    a->nodetype = 'F';
    a->init = init;
    a->cond = cond;
    a->upd = upd;
    a->tl = tl;
    return (struct ast *)a;
}
/* define a function */
void
dodef(struct symbol *name, struct symlist *syms, struct ast *func)
{

}

double
eval(struct ast *a)
{
  double v;

  if(!a) {
    yyerror("internal error, null eval");
    return 0.0;
  }

  switch(a->nodetype) {
    /* constant */
  case 'K': v = ((struct numval *)a)->number; break;

    /* name reference */
  case 'N': v = ((struct symref *)a)->s->value; break;

    /* assignment */
  case '=': v = ((struct symasgn *)a)->s->value =
      eval(((struct symasgn *)a)->v); break;

    /* expressions */
  case '+': v = eval(a->l) + eval(a->r); break;
  case '-': v = eval(a->l) - eval(a->r); break;
  case '*': v = eval(a->l) * eval(a->r); break;
  case '/': v = eval(a->l) / eval(a->r); break;
  case '|': v = fabs(eval(a->l)); break;
  case 'M': v = -eval(a->l); break;

    /* comparisons */
  case '1': v = (eval(a->l) > eval(a->r))? 1 : 0; break;
  case '2': v = (eval(a->l) < eval(a->r))? 1 : 0; break;
  case '3': v = (eval(a->l) != eval(a->r))? 1 : 0; break;
  case '4': v = (eval(a->l) == eval(a->r))? 1 : 0; break;
  case '5': v = (eval(a->l) >= eval(a->r))? 1 : 0; break;
  case '6': v = (eval(a->l) <= eval(a->r))? 1 : 0; break;

  /* control flow */
  /* null if/else/do expressions allowed in the grammar, so check for them */
  case 'I': 
    /*if( eval( ((struct flow *)a)->cond) != 0) {
      if( ((struct flow *)a)->tl) {
	v = eval( ((struct flow *)a)->tl);
      } else
	v = 0.0;		/* a default value 
    } else {
      if( ((struct flow *)a)->el) {
        v = eval(((struct flow *)a)->el);
      } else
	v = 0.0;		/* a default value 
    }*/
    break;

  case 'W':
    /*v = 0.0;		/* a default value 
    
    if( ((struct flow *)a)->tl) {
      while( eval(((struct flow *)a)->cond) != 0)
	v = eval(((struct flow *)a)->tl);
    }*/
    break;			/* last value is value */
	              
  case 'L':  break;

  case 'F':  break;

  case 'C':  break;

  default: printf("internal error: bad node %c\n", a->nodetype);
  }
  return v;
}

void
treefree(struct ast *a)
{
  switch(a->nodetype) {

    /* two subtrees */
  case '+':
  case '-':
  case '*':
  case '/':
  case '1':  case '2':  case '3':  case '4':  case '5':  case '6':
  case 'L':
    treefree(a->r);

    /* one subtree */
  case '|':
  case 'M': case 'C': case 'F':
    treefree(a->l);

    /* no subtree */
  case 'K': case 'N':
    break;

  case '=':
    free( ((struct symasgn *)a)->v);
    break;

  case 'I': case 'W':
    break;

  default: printf("internal error: free bad node %c\n", a->nodetype);
  }	  
  
  free(a); /* always free the node itself */

}


int
main()
{
  printf("> "); 
  return yyparse();
}

/* debugging: dump out an AST */
int debug = 0;
void
dumpast(struct ast *a, int level)
{

  printf("%*s", 2*level, "");	/* indent to this level */
  level++;

  if(!a) {
    printf("NULL\n");
    return;
  }

  switch(a->nodetype) {
    /* constant */
  case 'K': printf("number %4.4g\n", ((struct numval *)a)->number); break;

    /* name reference */
  case 'N': printf("ref %s\n", ((struct symref *)a)->s->name); break;

    /* assignment */
  case '=': printf("= %s\n", ((struct symref *)a)->s->name);
    dumpast( ((struct symasgn *)a)->v, level); return;

    /* expressions */
  case '+': case '-': case '*': case '/': case 'L':
  case '1': case '2': case '3':
  case '4': case '5': case '6': 
    printf("binop %c\n", a->nodetype);
    dumpast(a->l, level);
    dumpast(a->r, level);
    return;

  case '|': case 'M': 
    printf("unop %c\n", a->nodetype);
    dumpast(a->l, level);
    return;

  case 'I': case 'W':
    printf("flow %c\n", a->nodetype);

    return;
	              
  case 'F':
    return;

  case 'C': 

    return;

  default: printf("bad %c\n", a->nodetype);
    return;
  }
}
