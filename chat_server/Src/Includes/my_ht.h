#ifndef MY_HT_H
#define MY_HT_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef char *(*stringify)(void *);
typedef void (*free_h)(void *);

typedef struct hashtable hash_table;


unsigned long hash(const char *str, size_t size);
hash_table *create_ht(stringify, free_h, size_t);
int hash_table_insert(hash_table *, void *);
void *hash_table_lookup(hash_table *, const char *);
bool resize(hash_table *ht);
void destroy_ht(hash_table *);
int remove_entry(hash_table *, char *);


#endif