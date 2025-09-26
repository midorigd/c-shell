#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "read.h"

/**
 * future ideas:
 * initialization, config files
 * c printf with ANSI colors
 */

void csh_loop(void);


int main(int argc, char** argv) {
    // INITIALIZE: load configs

    // INTERPRET: run main loop
    csh_loop();

    // TERMINATE: free resources, shut down

    return EXIT_SUCCESS;
}


void csh_loop(void) {
    char* cmd;
    char** args;
    int status;

    do {
        printf(">> ");

        cmd = read_cmd();
        args = split_cmd(cmd);
        status = execute_cmd(args);

        free(cmd);
        free(args);

    } while (status);
}
