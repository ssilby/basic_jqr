/** @file my_ht.c
 * 
 * @brief A hash table data structure library that is data type independent
 *
 * @par       
 * Steven Silbert, stevensilbertjr@gmail.com
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "my_ht.h"

#define MAX_NAME 128
#define TABLE_SIZE 23


struct hashtable {
    void **entries;
    size_t size;
    size_t max_size;
    stringify string_it;
    free_h free_it;
};


/*
 *hash algorithm from Dan Bernstein
 *http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned long
hash(char *str, size_t size)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % size;
}


hash_table *
create_ht(stringify string_it, free_h fr, size_t capacity)
{
    hash_table *ht = calloc(sizeof(hash_table), 1);
    if (!ht) {
        fprintf(stderr, "Uh oh, you ran out of memory");
        return NULL;
    }
    ht->entries = calloc(sizeof(*(ht->entries)), capacity);
    if (!ht->entries) {
        fprintf(stderr, "Uh oh, you ran out of memory");
        free(ht);
        return NULL;
    }
    ht->size = 0;
    ht->max_size = capacity;
    ht->string_it = string_it;
    ht->free_it = fr;

    return ht;
}


int 
hash_table_insert(hash_table *ht, void*u)
{
    if (u == NULL || ht == NULL){
        return -1;
    }
    int index = hash(ht->string_it(u), ht->max_size);
    printf("name = %s, hash = %d\n", ht->string_it(u), index);
    while (ht->entries[index] != NULL) {
        index += 1;
        if (index == ht->max_size) {
            index = 0;
        }
    }
    ht->entries[index] = u;
    ht->size += 1;

    if ((ht->size / ht->max_size) > .5) {
        resize(ht);
    }

    return 0;
}


void *
hash_table_lookup(hash_table *ht, char *key)
{
    int index = hash(key, ht->max_size);

    // will not endlessly loop because it will be resized before becoming full
    while (ht->entries[index] != NULL && strncmp(ht->string_it(ht->entries[index]), key, strlen(key)) != 0) {
        index += 1;
        if (index == ht->max_size) {
            index = 0;
        }
    }

    return ht->entries[index];

}


bool
resize(hash_table *ht)
{
    size_t old_max = ht->max_size;
    ht->max_size = ht->max_size * 2 - 1;
    void **old_entries = ht->entries;
    ht->entries = calloc(ht->max_size, sizeof(void *));
    
    for (int i = 0; i < old_max; i++) {
        if (old_entries[i] != NULL) {
            hash_table_insert(ht, old_entries[i]);
        }
    }
    free(old_entries);

    return true;
}


void 
destroy_ht(hash_table *ht)
{
    for (int i = 0; i < ht->max_size; i++) {
        if (ht->entries[i] != NULL) {
            ht->free_it(ht->entries[i]);
        }
    }
    free(ht);
    
    return;
}


int
remove_entry(hash_table *ht, char *key)
{
    int index = hash(key, ht->max_size);

    // will not endlessly loop because it will be resized before becoming full
    while (ht->entries[index] != NULL && strncmp(ht->string_it(ht->entries[index]), key, strlen(key)) != 0) {
        index += 1;
        if (index == ht->max_size) {
            index = 0;
        }
    }
    
    if (ht->string_it(ht->entries[index]) != NULL) {
        ht->free_it(ht->entries[index]);
        ht->entries[index] = NULL;
    }

    return 0;
}

