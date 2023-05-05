#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

typedef struct file_ file_t;

file_t *open_file(char *, const char *flag);
int read_file(file_t *, char *, size_t);
int close_file(file_t *);
int write_file(file_t *, char *);
struct stat *get_attrib(file_t *);
int change_attrib(file_t *, char *, uid_t, gid_t);
int delete_file(char *);
int get_date_mod(file_t *, char *);
int get_date_create(file_t *, char *);
int crypt(file_t *, char *, size_t);


#endif
