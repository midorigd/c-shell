#include <stdlib.h>

#include "utils.h"

size_t arrlen(char** array) {
    size_t i = 0;
    while (array[i] != NULL) {
        ++i;
    }
    return i;
}
