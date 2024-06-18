#include <stdio.h>
extern int yyparse(void);
extern FILE *yyin;
#ifdef YYDEBUG
    extern int yydebug;
#endif
int main(int argc, char **argv)
{
    #ifdef YYDEBUG
        yydebug = 1;
    #endif
    if (argc > 2)
    {
        printf("Usage: %s <input_file>\n", argv[0]);
        return -1;
    }
    if (argc == 1)
    {
        printf("Interactive mode\nTo execute press Ctrl+D\n");
        yyin = stdin;
        printf("> ");
        return yyparse();
    }

    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        printf("Error: cannot open file %s\n", argv[1]);
        return -1;
    }

    printf("> ");
    return yyparse();
}