#include "ast.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int symhash(char *sym)
{
    unsigned int hash = 0;
    unsigned c;

    while ((c = (*sym++)))
        hash = hash * 9 ^ c;

    return hash;
}


struct symbol symtab[NHASH];

struct symbol *lookup(char *sym)
{
    struct symbol *sp = &symtab[symhash(sym) % NHASH];
    int scount = NHASH;

    while (--scount >= 0)
    {
        if (sp->name && !strcmp(sp->name, sym))
        {
            return sp;
        }

        /* new entry */
        if (!sp->name)
        {
            sp->name = strdup(sym);
            return sp;
        }

        if (++sp >= symtab + NHASH)
            sp = symtab; /* try the next entry */
    }
    yyerror("Symbol not found\n");
    abort(); /* tried them all, table is full */
}

struct ast *newast(int nodetype, struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    return a;
}


struct ast *newnum(int d)
{
    struct numval *a = malloc(sizeof(struct numval));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = 'N';
    a->number = d;
    return (struct ast *)a;
}


struct ast *newcmp(int cmptype, struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = cmptype;
    a->l = l;
    a->r = r;
    return a;
}


struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el)
{
    struct flow *a = malloc(sizeof(struct flow));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = nodetype;
    a->cond = cond;
    a->tl = tl;
    a->el = el;
    return (struct ast *)a;
}


struct ast *newref(char *s)
{
    struct symbol *sym = lookup(s);
    if (sym == NULL)
    {
        yyerror("NameError: name '%s' is not defined", s);
        exit(0);
    }
    struct symref *a = malloc(sizeof(struct symref));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = 'R';
    a->s = sym;
    return (struct ast *)a;
}


struct ast *newasgn(char *s, struct ast *v)
{

    struct symbol *sym = lookup(s);
    struct symasgn *a = malloc(sizeof(struct symasgn));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = '=';
    a->s = sym;
    a->v = v;
    return (struct ast *)a;
}

struct ast* newfor(struct ast* init, struct ast* cond, struct ast* inc, struct ast* tl)
{
    struct for_loop* a = malloc(sizeof(struct for_loop));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = 'F';
    a->init = init;
    a->cond = cond;
    a->inc = inc;
    a->tl = tl;
    return (struct ast *)a;
}



void treefree(struct ast *a)
{
    switch (a->nodetype)
    {
        // clang-format off
        /* Nodes with two subtrees */
        case '+':
        case '-':
        case '*':
        case '/':
        case 1: case 2: case 3: case 4: case 5: case 6: // comparisons
        case '&': case '|': // logical operators
        case 'L':
            treefree(a->r);
            /* Nodes with one subtree */
        case 'U':
        case '!':
            treefree(a->l);
            /* Nodes with no subtree */
        case 'N': case 'R':
            break;
        case '=':  // Assignment node, free the value
            free(((struct symasgn *)a)->v);
            break;
        case 'I': case 'W':  // If and While nodes, condition and branches
            free(((struct flow *)a)->cond);
            if(((struct flow *)a)->tl) treefree(((struct flow *)a)->tl);
            if(((struct flow *)a)->el) treefree(((struct flow *)a)->el);
            break;
        case 'F':  // For node, free the init, condition, increment and body
            if(((struct for_loop *)a)->init) treefree(((struct for_loop *)a)->init);
            if(((struct for_loop *)a)->cond) treefree(((struct for_loop *)a)->cond);
            if(((struct for_loop *)a)->inc) treefree(((struct for_loop *)a)->inc);
            if(((struct for_loop *)a)->tl) treefree(((struct for_loop *)a)->tl);
            break;
        
        default: printf("internal error: free bad node %c\n", a->nodetype);
        // clang-format on
    }
}

static void print_indent(FILE *stream, int level)
{
    char *indent = malloc(sizeof(char) * (level * 2 + 1));
    memset(indent, ' ', level * 2 + 1);
    indent[level * 2] = '\0';
    fprintf(stream, "%s", indent);
    free(indent);
}

static void print_node(FILE *stream, int level, char *arguments, ...)
{
    va_list args;
    va_start(args, arguments);
    print_indent(stream, level);
    vfprintf(stream, arguments, args);
    va_end(args);
}

int seq_reg_num = 1;
int if_num = -1;
int while_num = -1;

int next_reg()
{
    seq_reg_num = seq_reg_num != 31 ? seq_reg_num + 1 : 1;
    return seq_reg_num;
}
void free_reg()
{
    seq_reg_num = seq_reg_num != 1 ? seq_reg_num - 1 : 31;
}

void interpret_prog(FILE *stream, struct ast *ast, int reg)
{
    if (!ast)
    {
        return;
    }
    int rs2, temp1, temp2 = 0;
    int num = 0;
    switch (ast->nodetype)
    {
    case 'L':
        interpret_prog(stream, ast->l, reg);
        interpret_prog(stream, ast->r, reg);
        break;
    case 'N':
        fprintf(stream, "li x%d, %d\n", reg, ((struct numval *)ast)->number);
        break;
    case 'R':
        fprintf(stream, "lw x%d, x0, %d\n", reg, ((struct symref *)ast)->s->offset);
        break;
    case '=':
        interpret_prog(stream, ((struct symasgn *)ast)->v, reg);
        fprintf(stream, "sw x0, %d, x%d\n", ((struct symasgn *)ast)->s->offset, reg);
        break;
    case '+':
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "add x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case '-':
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "sub x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case '*':
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "mul x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case '/':
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "div x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case 1: // >
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        temp1 = next_reg();
        temp2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "sne x%d, x%d, x%d\n", temp1, reg, rs2);
        fprintf(stream, "sge x%d, x%d, x%d\n", temp2, reg, rs2);
        fprintf(stream, "and x%d, x%d, x%d\n", reg, temp1, temp2);
        free_reg();
        free_reg();
        free_reg();
        break;
    case 2: // <
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "slt x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case 3: // !=
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "sne x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case 4: // ==
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "seq x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case 5: // >=
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "sge x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case 6: // <=
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        temp1 = next_reg();
        temp2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "seq x%d, x%d, x%d\n", temp1, reg, rs2);
        fprintf(stream, "slt x%d, x%d, x%d\n", temp2, reg, rs2);
        fprintf(stream, "or x%d, x%d, x%d\n", reg, temp1, temp2);
        free_reg();
        free_reg();
        free_reg();
        break;
    case '&':
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "and x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case '|':
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "or x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case '!':
        interpret_prog(stream, ast->l, reg);
        fprintf(stream, "xori x%d, x%d, 1\n", reg, reg);
        break;
    case 'U':
        interpret_prog(stream, ast->l, reg);
        fprintf(stream, "sub x%d, x0, x%d\n", reg, reg);
        break;
    case 'I':
        num = ++if_num;
        temp1 = next_reg();
        interpret_prog(stream, ((struct flow *)ast)->cond, temp1);
        if (((struct flow *)ast)->el != NULL)
        {
            fprintf(stream, "beq x%d, x0, ELSE%d\n", temp1, num);
        }
        else
        {
            fprintf(stream, "beq x%d, x0, ENDIF%d\n", temp1, num);
        }
        free_reg();
        interpret_prog(stream, ((struct flow *)ast)->tl, reg);
        fprintf(stream, "jal x%d, ENDIF%d\n", reg, num);
        if (((struct flow *)ast)->el != NULL)
        {
            fprintf(stream, "ELSE%d:\n", num);
            interpret_prog(stream, ((struct flow *)ast)->el, reg);
        }
        fprintf(stream, "ENDIF%d:\n", num);
        break;
    case 'W':
        num = ++while_num;
        fprintf(stream, "WHILE%d:\n", num);
        temp1 = next_reg();
        interpret_prog(stream, ((struct flow *)ast)->cond, temp1);
        fprintf(stream, "beq x%d, x0, ENDWHILE%d\n", temp1, num);
        free_reg();
        interpret_prog(stream, ((struct flow *)ast)->tl, reg);
        fprintf(stream, "jal x%d, WHILE%d\n", reg, num);
        fprintf(stream, "ENDWHILE%d:\n", num);
        break;
    case 'F':
        num = ++while_num;
        interpret_prog(stream, ((struct for_loop *)ast)->init, reg);
        fprintf(stream, "WHILE%d:\n", num);
        temp1 = next_reg();
        interpret_prog(stream, ((struct for_loop *)ast)->cond, temp1);
        fprintf(stream, "beq x%d, x0, ENDWHILE%d\n", temp1, num);
        free_reg();
        interpret_prog(stream, ((struct for_loop *)ast)->tl, reg);
        interpret_prog(stream, ((struct for_loop *)ast)->inc, reg);
        fprintf(stream, "jal x%d, WHILE%d\n", reg, num);
        fprintf(stream, "ENDWHILE%d:\n", num);
        break;
    }
}

void print_asm(FILE *stream, struct ast *ast)
{
    if (!ast)
    {
        return;
    }
    int stack_offset = 1;
    fprintf(stream, "jal x1, MAIN\n");
    for (int i = 0; i < NHASH; i++)
    {
        if (symtab[i].name != NULL)
        {
            fprintf(stream, "%s:\ndata 0 * 1\n", symtab[i].name);
            symtab[i].offset = stack_offset;
            stack_offset++;
        }
    }
    fprintf(stream, "MAIN:\n");
    interpret_prog(stream, ast, 1);
    fprintf(stream, "ebreak\n");
}

void print_ast(FILE *stream, struct ast *ast, int level)
{
    if (!ast)
    {
        return;
    }
    switch (ast->nodetype)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6: // comparisons
        print_node(stream, level, "Comparison %s\n", cmp2str[ast->nodetype]);
        print_ast(stream, ast->l, level + 1);
        print_ast(stream, ast->r, level + 1);
        break;
    case '+':
    case '-':
    case '*':
    case '/':
        print_node(stream, level, "Operator %c\n", ast->nodetype);
        print_ast(stream, ast->l, level + 1);
        print_ast(stream, ast->r, level + 1);
        break;

    case 'L':
        print_node(stream, level, "List\n");
        print_ast(stream, ast->l, level + 1);
        print_ast(stream, ast->r, level + 1);
        break;
    case 'U':
        print_node(stream, level, "Unary Minus \n");
        print_ast(stream, ast->l, level + 1);
        break;
    case 'N':
        print_node(stream, level, "Constant %d\n", ((struct numval *)ast)->number);
        break;
    case 'R':
        print_node(stream, level, "Symbol %s\n", ((struct symref *)ast)->s->name);
        break;
    case '=':
        print_node(stream, level, "Assignment %s\n", ((struct symasgn *)ast)->s->name);
        print_ast(stream, ((struct symasgn *)ast)->v, level + 1);
        break;
    case 'I':
        print_node(stream, level, "If\n");
        print_ast(stream, ((struct flow *)ast)->cond, level + 1);
        print_ast(stream, ((struct flow *)ast)->tl, level + 1);
        print_ast(stream, ((struct flow *)ast)->el, level + 1);
        break;
    case 'W':
        print_node(stream, level, "While\n");
        print_ast(stream, ((struct flow *)ast)->cond, level + 1);
        print_ast(stream, ((struct flow *)ast)->tl, level + 1);
        break;
    case 'F':
        print_node(stream, level, "For\n");
        print_ast(stream, ((struct for_loop *)ast)->init, level + 1);
        print_ast(stream, ((struct for_loop *)ast)->cond, level + 1);
        print_ast(stream, ((struct for_loop *)ast)->inc, level + 1);
        print_ast(stream, ((struct for_loop *)ast)->tl, level + 1);
        break;
    case '&':
    case '|':
        print_node(stream, level, "Operator %s\n", ast->nodetype);
        print_ast(stream, ast->l, level + 1);
        print_ast(stream, ast->r, level + 1);
        break;
    case '!':
        print_node(stream, level, "Operator %s\n", ast->nodetype);
        print_ast(stream, ast->l, level + 1);
        break;
    default:
        fprintf(stream, "internal error: bad node %c\n", ast->nodetype);
    }
}


void yyerror(char *s, ...)
{
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}