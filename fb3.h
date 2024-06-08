#ifndef TEST
#define TEST

/* symbol table */
struct symbol {		/* a variable name */
  char *name;
  int value;
};

/* simple symtab of fixed size */
#define NHASH 9997
extern struct symbol symtab[NHASH];

struct symbol *lookup(char*);


struct symlist *newsymlist(struct symbol *sym, struct symlist *next);
void symlistfree(struct symlist *sl);

/* node types
 *  + - * / | &
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


#endif