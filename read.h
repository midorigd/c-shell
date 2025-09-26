#ifndef READ_H
#define READ_H

char* read_cmd(void);
char** split_cmd(char*);
int execute_cmd(char**);
int launch_cmd(char**);

#endif
