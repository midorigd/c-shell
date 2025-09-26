#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "read.h"
#include "builtin.h"

char* read_cmd(void) {
    size_t buffer_size = 128;
    char* line = malloc(buffer_size);

    if (line == NULL) {
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

        // reallocate if block is at capacity
        if (++i >= buffer_size) {

            // doubling strategy used for amortized constant dsa whatever
            buffer_size *= 2;
            char* new_line = realloc(line, buffer_size);

            if (new_line == NULL) {
                fprintf(stderr, "Line reallocation failed\n");
                free(line);
                exit(EXIT_FAILURE);
            }

            line = new_line;
        }
    }
}

char** split_cmd(char* command) {
    size_t buffer_size = 8;
    char** args = malloc(buffer_size); // model args as null-terminated array of strings

    if (args == NULL) {
        fprintf(stderr, "Argument allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // split by space, escape sequences and string literals not currently supported
    size_t i = 0;
    const char* delim = " ";
    char* arg = strtok(command, delim);

    do {
        args[i] = arg;

        // resize using doubling strategy
        if (++i >= buffer_size) {
            buffer_size *= 2;
            char** new_args = realloc(args, buffer_size);

            if (new_args == NULL) {
                fprintf(stderr, "Argument reallocation failed\n");

                // strtok modifies in-place, so *args points to the original string
                free(*args);
                free(args);

                exit(EXIT_FAILURE);
            }

            args = new_args;
        }

        arg = strtok(NULL, delim);

    } while (arg != NULL);

    args[i] = NULL;
    return args;
}

int execute_cmd(char** args) {
    for (size_t i = 0; builtin_names[i] != NULL; ++i) {
        if (!strcmp(*args, builtin_names[i])) {
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
            perror("shell: execvp error");
            exit(EXIT_FAILURE);
        }

    // fork returns -1 if error has occurred
    } else if (pid < 0) {
        perror("shell: fork error");

    // parent process gets returned child's pid
    } else {
        // pid_t wpid;

        do {
            /* wpid = */ waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}
