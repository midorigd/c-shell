#ifndef READ_H
#define READ_H

char* resize_str_buffer(char*, size_t);
char** resize_token_buffer(char**, size_t);

char* read_cmd(void);
char** split_cmd(char*);
int execute_cmd(char**);
int launch_cmd(char**);

#endif
