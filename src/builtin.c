#include <stdio.h>
#include <unistd.h>

#include "builtin.h"

const char* const builtin_names[] = {
    "cd",
    "help",
    "exit",
    NULL
};

int (* const builtin_func[]) (char**) = {
    &builtin_cd,
    &builtin_help,
    &builtin_exit,
};

int builtin_cd(char** args) {
    if (!args[1]) {
        fprintf(stderr, "Usage: cd <newdir>\n");
        return 1;
    }

    int status = chdir(args[1]);
    if (status) {
        perror("csh: cd error");
    }
    return 1;
}

int builtin_help(char** args) {
    printf("Basic custom shell\n\n");
    printf("Built-in commands:\n");

    for (size_t i = 0; builtin_names[i] != NULL; ++i) {
        printf("%s\n", builtin_names[i]);
    }

    return 1;
}

int builtin_exit(char** args) {
    return 0;
}
