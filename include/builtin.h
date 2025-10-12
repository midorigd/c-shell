#ifndef BUILTIN_H
#define BUILTIN_H

extern const char* const builtin_names[];
extern int (* const builtin_func[]) (char**);

int builtin_cd(char**);
int builtin_help(char**);
int builtin_exit(char**);

#endif
