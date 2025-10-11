#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "read.h"
#include "builtin.h"

enum ReadMode {
    DEFAULT,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
};

/*
    Reallocates memory for and resizes a string buffer to the buffer_size.
*/
char* resize_str_buffer(char* buffer, size_t buffer_size) {
    char* new_buffer = realloc(buffer, buffer_size);

    if (!new_buffer) {
        fprintf(stderr, "String buffer reallocation failed\n");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    return new_buffer;
}

/*
    Reallocates memory for and resizes a token buffer to the buffer_size.
*/
char** resize_token_buffer(char** buffer, size_t buffer_size) {
    char** new_buffer = realloc(buffer, buffer_size);

    if (!new_buffer) {
        fprintf(stderr, "Token buffer reallocation failed\n");

        // strtok modifies in-place, so *buffer points to the original string
        free(*buffer);
        free(buffer);

        exit(EXIT_FAILURE);
    }

    return new_buffer;
}

/*
    Reads chars from input into a dynamic buffer until a newline or null-terminator is reached.
*/
char* read_cmd(void) {
    size_t buffer_size = 128;
    char* line = malloc(buffer_size);

    if (!line) {
        fprintf(stderr, "Line allocation failed\n");
        exit(EXIT_FAILURE);
    }

    size_t i = 0;
    int chr;

    // read input char by char
    while (1) {
        chr = getc(stdin);

        // add null-terminator and return if end of line reached
        if (chr == EOF || chr == '\n') {
            line[i] = '\0';
            return line;
        }

        line[i] = chr;
        ++i;

        // reallocate if block is at capacity
        if (i >= buffer_size) {

            // doubling strategy used for amortized constant dsa whatever
            buffer_size *= 2;
            line = resize_str_buffer(line, buffer_size);
        }
    }
}

/*
    Tokenizes a command into its respective arguments, splitting by whitespace.
*/
char** split_cmd(char* command) {
    size_t buffer_size = 8;
    char** tokens = malloc(buffer_size); // model args as null-terminated array of strings

    if (!tokens) {
        fprintf(stderr, "Argument allocation failed\n");
        exit(EXIT_FAILURE);
    }

    int readMode = DEFAULT;

    // split by space, escape sequences and string literals not currently supported
    size_t i = 0;
    const char* delim = " ";
    char* token = strtok(command, delim);

    do {
        tokens[i] = token;
        ++i;

        // resize using doubling strategy
        if (i >= buffer_size) {
            buffer_size *= 2;
            tokens = resize_token_buffer(tokens, buffer_size);
        }

        token = strtok(NULL, delim);

    } while (token != NULL);

    tokens[i] = NULL;
    return tokens;
}

int execute_cmd(char** args) {
    for (size_t i = 0; builtin_names[i] != NULL; ++i) {
        if (strcmp(*args, builtin_names[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return launch_cmd(args);
}

int launch_cmd(char** args) {
    int status;

    pid_t pid = fork();

    // child process gets returned pid 0
    if (pid == 0) {
        int exec_stat = execvp(*args, args);

        // exec returns if error has occurred
        if (exec_stat == -1) {
            perror("csh: execvp error");
            exit(EXIT_FAILURE);
        }

    // fork returns -1 if error has occurred
    } else if (pid < 0) {
        perror("csh: fork error");

    // parent process gets returned child's pid
    } else {
        // pid_t wpid;

        do {
            /* wpid = */ waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}
