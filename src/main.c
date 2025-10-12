#include <stdlib.h>

#include "csh.h"

int main(int argc, char** argv) {
    // INITIALIZE: load configs

    // INTERPRET: run main loop
    csh_loop();

    // TERMINATE: free resources, shut down

    return EXIT_SUCCESS;
}
