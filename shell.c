//Current code

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

// Limits
#define MAX_TOKENS 100
#define MAX_STRING_LEN 100

size_t MAX_LINE_LEN = 10000;

// Builtin commands
#define EXIT_STR "exit"
#define EXIT_CMD 0
#define UNKNOWN_CMD 99
#define TRUE 1
#define FALSE 0
#define WRITE_TO_FILE 1
#define READ_FROM_FILE -1
#define NO_REDIRECT 0

FILE *fp; 
char **tokens;
char *line;
char **history; 
int history_count = 0;

void initialize()
{
    assert((line = malloc(sizeof(char) * MAX_STRING_LEN)) != NULL);
    assert((tokens = malloc(sizeof(char *) * MAX_TOKENS)) != NULL);
    assert((fp = fdopen(STDIN_FILENO, "r")) != NULL);
    assert((history = malloc(sizeof(char *) * MAX_TOKENS)) != NULL);
}

int tokenize(char *string)
{
    int token_count = 0;
    int size = MAX_TOKENS;
    char *this_token;

    while ((this_token = strsep(&string, " \t\v\f\n\r")) != NULL)
    {
        if (*this_token == '\0')
            continue;

        if (strcmp(this_token, "|") == 0)
        {
            tokens[token_count] = NULL; // Set the previous token to NULL
            token_count++;
        }

        tokens[token_count] = this_token;

        token_count++;

        if (token_count >= size)
        {
            size *= 2;
            assert((tokens = realloc(tokens, sizeof(char *) * size)) != NULL);
        }
    }
    return token_count;
}

int read_command()
{
    assert(getline(&line, &MAX_LINE_LEN, fp) > -1);
    return tokenize(line);
}

int run_command(int token_count)
{
    if (strcmp(tokens[0], EXIT_STR) == 0)
        return EXIT_CMD;
    else if (strcmp(tokens[0], "hist") == 0)
    {
        for (int i = 0; i < history_count; i++)
        {
            printf("%d. %s\n", i + 1, history[i]);
        }
        return TRUE;
    }
    else if (strcmp(tokens[0], "!!") == 0)
    {
        strcpy(line, history[history_count - 1]);
    }
    // Concatenate all tokens to form the command line
    char command_line[MAX_LINE_LEN];
    strcpy(command_line, tokens[0]);
    for (int i = 1; i < token_count; i++)
    {
        strcat(command_line, " ");
        strcat(command_line, tokens[i]);
    }

    // Add command line to history
    history[history_count] = strdup(command_line);
    history_count++;

    // Execute the basic commands
    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process
        tokens[token_count] = NULL; // Terminate the tokens array
        execvp(tokens[0], tokens);
        exit(0);
    }
    else if (pid > 0)
    {
        // Parent process
        wait(NULL);
    }
    else
    {
        // Fork failed
        exit(1);
    }

    return UNKNOWN_CMD;
}

int main()
{
    initialize();
    int token_count;

    do
    {
        printf("sh550> ");
        token_count = read_command();
    } while (run_command(token_count) != EXIT_CMD);
    
    return 0;
}
