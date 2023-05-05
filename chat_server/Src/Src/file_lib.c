/** @file my_ht.c
 * 
 * @brief A file library that allows for storage of file information in a struct
 *
 * @par       
 * Steven Silbert, stevensilbertjr@gmail.com
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


#include "../Includes/file_lib.h"

struct file_ {
    char *filename;
    FILE *fp;
    int fd;
};


file_t *
open_file(const char *filename, const char *flag)
{
    if (!filename) {
        fprintf(stderr, "No filename provided\n");
        return NULL;
    }
    file_t *filebub = malloc(sizeof(file_t));
    if (!filebub) {
        return NULL;
    }
    filebub->filename = strndup(filename, strlen(filename));
    if (!filebub->filename) {
        return NULL;
    }
    filebub->fp = fopen(filename, flag);
    if ( filebub->fp == NULL ) {
        return NULL;
    }
    filebub->fd = fileno(filebub->fp);

    return filebub;
}


int
read_file(file_t *filebub, void *buffer, size_t bytes)
{
    if (!buffer) {
        fprintf(stderr, "Invalid buffer\n");
    }
    if (bytes <= 0) {
        fprintf(stderr, "Invalid read size\n");
    }
    if (!filebub->fp) {
        fprintf(stderr, "Invalid file pointer\n");
    }
    // fseek(filebub->fp, 0, SEEK_SET);
    size_t bytes_read = fread(buffer, 1, bytes, filebub->fp);
    if (ferror(filebub->fp)) {
        fprintf(stderr, "Error reading file\n");
    }
    if (!buffer) {
        fprintf(stderr, "Error writing to buffer\n");
        return -1;
    }
    
    return bytes_read;
}


int
close_file(file_t *filebub)
{
    if (!filebub->fp) {
        fprintf(stderr, "Invalid file pointer\n");
        return -1;
    }
    int ret = fclose(filebub->fp);
    
    if (ret != 0) {
        return -1;
    }
    free(filebub->filename);
    free(filebub);

    return 0;
}


int
write_file(file_t *filebub, void *data, size_t len)
{
    if (!filebub->fp) {
        fprintf(stderr, "Invalid file pointer\n");
        return -1;
    }
    int ret = fwrite(data, 1, len, filebub->fp);
    return ret;
}


struct stat *
get_attrib(file_t *filebub)
{
    struct stat *attribs = malloc(sizeof(struct stat));
    if (!attribs) {
        return NULL;
    }
    if (stat(filebub->filename, attribs) < 0) {
        fprintf(stderr, "File Error: %s\n", strerror(errno));
        return NULL;
    }
    return attribs;
}


int
change_attrib(file_t *filebub, char *mode, uid_t owner, gid_t group)
{
    if (!filebub) {
        fprintf(stderr, "Invalid file_t struct\n");
        return -1;
    }
    int i = strtol(mode, 0, 8);
    chmod(filebub->filename, i);
    chown(filebub->filename, owner, group);

    return 0;
}


int
delete_file(file_t *filebub)
{
    char *name = strndup(filebub->filename, strlen(filebub->filename));
    if (!name) {
        return -1;
    }
    close_file(filebub);
    if (remove(name) == 0) {
        printf("Deleted successfully\n");
    } else {
        printf("Unable to delete the file\n");
        free(name);
        return -1;
    }
    free(name);
    return 0;
}


int
get_date_mod(file_t *filebub, char *date)
{
    struct stat *attribs = get_attrib(filebub);
    if (!attribs) {
        return -1;
    }
    strftime(date, 36, "%d.%m.%Y %H:%M:%S", localtime(&attribs->st_mtime));
    return 0;
}


int
get_date_create(file_t *filebub, char *date)
{
    struct stat *attribs = get_attrib(filebub);
    if (!attribs) {
        return -1;
    }
    strftime(date, 36, "%d.%m.%Y %H:%M:%S", localtime(&attribs->st_ctime));
    return 0;
}


// encryption algorithm by andrey.117
// https://cboard.cprogramming.com/c-programming/155191-xor-file-encryption-c.html
// 29JUL2021
int
crypter(file_t *filebub, char *key, size_t keylen)
{
    struct stat *attribs = get_attrib(filebub);
    long int fsize = attribs->st_size;
    
    char *buffer = malloc(fsize);

    if (fread(buffer, fsize, 1, filebub->fp) != 1) {
        fprintf(stderr, "Error reading file\n");
        return -1;
    }
    for (int i = 0; i < fsize; ++i) {
        buffer[i] = buffer[i] ^ key[i % keylen];
    }
    rewind(filebub->fp);
    if (fwrite(buffer, fsize, 1, filebub->fp) != 1) {
        fprintf(stderr, "Error writing encrypted data to file\n");
        return -1;
    }
    rewind(filebub->fp);
    free(buffer);
    return 0;
}

