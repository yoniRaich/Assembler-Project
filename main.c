#include "assembler.h"
void assembler(char *);
void freeStr(TypeLabel *);
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Error: Not enough arguments, please insert files names.\n");
        return 1;
    }
    while (--argc > 0)
    {
        assembler(*++argv);
    }
    return 0;
}

void assembler(char *file_name)
{
    FILE *expanded_file_handler;
    char *expanded_file_name;
    if ((expanded_file_name = (char *)malloc(sizeof(char) * (strlen(file_name) + 14))) == NULL)
    {
        fprintf(stderr, "Error: Out of memory\n");
        exit(EXIT_FAILURE);
    }
    printf("Pre Assembly process at work...");
    fflush(stdout);
    if (!preAssembler(expanded_file_name, file_name)) /*go to pre-assembler.c for more info*/
    {
        free(expanded_file_name);
        return;
    }
    printf("===Assembly process for %s.as started===\n", file_name);
    if (!(expanded_file_handler = fopen(expanded_file_name, "r")))
    {
        fprintf(stderr, "Error: File '%s' open failed.\n", expanded_file_name);
        exit(EXIT_FAILURE);
    }
    printf("Assembly First Pass at work...");
    fflush(stdout);

    if (!firstPass(expanded_file_handler)) /*go to first-pass.c for more info*/
    {
        fclose(expanded_file_handler);
        free(expanded_file_name);
        freeStr(symbol_table);
        free(symbol_table);
        return;
    }
    printf("Assembly Second Pass at work...");
    fflush(stdout);
    if (!secondPass(expanded_file_handler))
    {
        fclose(expanded_file_handler);
        free(expanded_file_name);
        freeStr(symbol_table);
        free(symbol_table);
        return;
    }

    freeStr(symbol_table);
    free(symbol_table);
    fclose(expanded_file_handler);
    free(expanded_file_name);
    printf("===Assembly process for %s.as finshed===\n\n", file_name);
}
void freeStr(TypeLabel *symbol_table)
{
    unsigned int i = 0;
    while (strcmp(symbol_table[i].name, ""))
    {
        free(symbol_table[i].name);
        free(symbol_table[i++].attribute);
    }
    free(symbol_table[i].name);
    free(symbol_table[i].attribute);
}
