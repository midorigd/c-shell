#ifndef READ_H
#define READ_H

char* resize_str_buffer(char*, size_t);
char** resize_token_buffer(char**, size_t);

int process_char(char, char**, size_t*, char***, size_t*);
char handle_dq_escape(char);
void push_char(char**, size_t*, char, size_t*);
void push_token(char***, size_t*, char*, size_t*);

void reset_buffer(char**, size_t*, size_t*);
void reset_tokens(char***, size_t*, size_t*, size_t*);

char* read_cmd(void);
char** split_cmd(char*);
int execute_cmd(char**);
int launch_cmd(char**);

#endif
