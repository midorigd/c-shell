#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "read.h"
#include "builtin.h"
#include "utils.h"

#define DEBUG_MODE
#include "debug.h"

static const size_t INIT_CMD_SIZE = 128;
static const size_t INIT_TOK_SIZE = 8;
static const size_t INIT_BUF_SIZE = 64;

enum ReadMode {
    DEFAULT,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
};

/*
    Reallocates memory for and resizes a string buffer to the buffer_size.
*/
char* resize_str_buffer(char* buffer, size_t buffer_size) {
    char* new_buffer = realloc(buffer, buffer_size * sizeof(char));

    if (!new_buffer) {
        fprintf(stderr, "String buffer reallocation failed\n");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    // initialize newly allocated memory to 0
    memset(new_buffer + (buffer_size / 2), 0, buffer_size / 2 * sizeof(char));

    return new_buffer;
}

/*
    Reallocates memory for and resizes a token buffer to the buffer_size.
*/
char** resize_token_buffer(char** buffer, size_t buffer_size) {
    char** new_buffer = realloc(buffer, buffer_size * sizeof(char*));

    if (!new_buffer) {
        fprintf(stderr, "Token buffer reallocation failed\n");

        // strtok modifies in-place, so *buffer points to the original string
        free(*buffer);
        free(buffer);

        exit(EXIT_FAILURE);
    }

    // initialize newly allocated memory to NULL
    memset(new_buffer + (buffer_size / 2), 0, buffer_size / 2 * sizeof(char*));

    return new_buffer;
}

/*
    Processes a single character, setting escape flags and terminating the token as necessary.
    Returns what the next ReadMode state should be.
*/
int process_char(char c, char** buffer, size_t* buffer_size, char*** tokens, size_t* tokens_size) {
    static size_t buf_index = 0;
    static size_t tok_index = 0;

    static int read_mode = DEFAULT;
    static bool escape_next = false;

    DEBUG_PRINTF("Added '%c' (%d)\t| token: %s\n", c, c, *buffer);

    // end of token sequence, push and reset token index
    if (c == '\0') {
        push_token(tokens, tokens_size, *buffer, &tok_index);
        reset_tokens(tokens, tokens_size, &buf_index, &tok_index);
        return 0;
    }

    switch (read_mode) {

        // default mode: spaces as delimiters, escaping ignores special char meanings
        case DEFAULT:

            // escape in default mode: treat next character literally, turn off escape
            if (escape_next) {
                push_char(buffer, buffer_size, c, &buf_index);
                escape_next = false;
                break;
            }

            switch (c) {
                case '\'':
                    read_mode = SINGLE_QUOTE;
                    break;
                case '"':
                    read_mode = DOUBLE_QUOTE;
                    break;
                case '\\':
                    escape_next = true;
                    break;
                case ' ':
                case '\t':
                case '\n':
                    push_token(tokens, tokens_size, *buffer, &tok_index);
                    reset_buffer(buffer, buffer_size, &buf_index);
                    break;
                default:
                    push_char(buffer, buffer_size, c, &buf_index);
            }
            break;

        // single-quote mode: everything is literal, no escapes processed
        case SINGLE_QUOTE:
            switch (c) {
                case '\'':
                    read_mode = DEFAULT;
                    break;
                default:
                    push_char(buffer, buffer_size, c, &buf_index);
            }
            break;

        // double-quote mode: process escape sequences
        case DOUBLE_QUOTE:
            if (escape_next) {
                c = handle_dq_escape(c);
                push_char(buffer, buffer_size, c, &buf_index);
                escape_next = false;
                break;
            }

            switch (c) {
                case '\\':
                    escape_next = true;
                    break;
                case '"':
                    read_mode = DEFAULT;
                    break;
                default:
                    push_char(buffer, buffer_size, c, &buf_index);
            }
    }

    return read_mode;
}

/*
    Returns an escaped version of the given char, if it exists.
    Only works in double-quoted escape sequences.
*/
char handle_dq_escape(char c) {
    switch (c) {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case '\\':
            return '\\';
        case '"':
            return '"';
        // case '`':
        //     return '`';
        // case '$':
        //     return '$';
        default:
            return c;
    }
}

/*
    Adds another character to the end of the buffer and resizes if necessary.
*/
void push_char(char** buffer, size_t* buffer_size, char c, size_t* buf_index) {
    // add new char at end of dereferenced array
    (*buffer)[*buf_index] = c;
    ++(*buf_index);

    if (*buf_index == *buffer_size) {
        *buffer_size *= 2;
        *buffer = resize_str_buffer(*buffer, *buffer_size);
    }
}

/*
    Adds a new token from the buffer to the end of the tokens array, resizes
    if necessary, and clears the buffer to prepare for the next token.
*/
void push_token(char*** tokens, size_t* tokens_size, char* new_token, size_t* tok_index) {
    // add new token string at end of dereferenced array
    (*tokens)[*tok_index] = new_token;
    ++(*tok_index);

    DEBUG_PRINTF("Token pushed: '%s'\n", new_token);

    if (*tok_index == *tokens_size) {
        *tokens_size *= 2;
        *tokens = resize_token_buffer(*tokens, *tokens_size);
    }
}

/*
    Sets every char in the buffer to 0 and resets the buffer index.
*/
void reset_buffer(char** buffer, size_t* buffer_size, size_t* buf_index) {
    *buffer_size = INIT_BUF_SIZE;
    *buf_index = 0;
    *buffer = calloc(INIT_BUF_SIZE, sizeof(char));
}

/*
    Resets the token array index.
*/
void reset_tokens(char*** tokens, size_t* tokens_size, size_t* buf_index, size_t* tok_index) {
    *buf_index = 0;
    *tok_index = 0;
}

/*
    Reads chars from input into a dynamic buffer until a newline or null-terminator is reached.
*/
char* read_cmd(void) {
    size_t buffer_size = INIT_CMD_SIZE;
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
            DEBUG_PRINTF("Command read: '%s'\n", line);
            return line;
        }

        line[i] = chr;
        ++i;

        // reallocate if block is at capacity
        if (i == buffer_size) {

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
    size_t tokens_size = INIT_TOK_SIZE;
    char** tokens = calloc(INIT_TOK_SIZE, sizeof(char*)); // model tokens as null-terminated array of strings

    if (!tokens) {
        fprintf(stderr, "Argument allocation failed\n");
        exit(EXIT_FAILURE);
    }

    size_t buffer_size = INIT_BUF_SIZE;
    char* buffer = calloc(INIT_BUF_SIZE, sizeof(char));

    if (!buffer) {
        fprintf(stderr, "Token allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // split by space, escape sequences and string literals not currently supported
    size_t cmd_index = 0;

    while (command[cmd_index] != '\0') {
        process_char(command[cmd_index], &buffer, &buffer_size, &tokens, &tokens_size);
        ++cmd_index;
    }
    process_char(command[cmd_index], &buffer, &buffer_size, &tokens, &tokens_size);

    return tokens;
}

int execute_cmd(char** args) {
    DEBUG_PRINTF("Tokens: [");
    for (size_t i = 0; args[i] != NULL; ++i) {
        DEBUG_PRINTF("%s, ", args[i]);
    }
    DEBUG_PRINTF("]\n");

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
        int exec_stat = execvp(args[0], args);

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
