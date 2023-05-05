#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


#include "file_lib.h"

struct file_ {
    char *filename;
    FILE *fp;
    int fd;
};

file_t *open_file(char *filename, const char *flag)
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
        free(filebub);
        return NULL;
    }
    filebub->fp = fopen(filename, flag);
    if ( filebub->fp == NULL ) {
        fprintf(stderr, "Error opening file: %s\n", strerror( errno ));
        free(filebub);
        return NULL;
    }
    filebub->fd = fileno(filebub->fp);

    return filebub;
}

int read_file(file_t *filebub, char *buffer, size_t bytes)
{
    if (!buffer) {
        fprintf(stderr, "Invalid buffer\n");
        return -1;
    }
    if (bytes <= 0) {
        fprintf(stderr, "Invalid read size\n");
        return -1;
    }
    if (!filebub->fp) {
        fprintf(stderr, "Invalid file pointer\n");
        return -1;
    }
    // fseek(filebub->fp, 0, SEEK_SET);
    size_t bytes_read = fread(buffer, sizeof(char), bytes, filebub->fp);
    buffer[bytes] = '\0';
    if (ferror(filebub->fp)) {
        fprintf(stderr, "Error reading file\n");
        return -1;
    }
    if (!buffer) {
        fprintf(stderr, "Error writing to buffer\n");
        return -1;
    }
    if (bytes_read != bytes) {
        fprintf(stderr, "Incorrect number of bytes read from file\n");
        return -1;
    }
    
    return bytes_read;
}

int close_file(file_t *filebub)
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

int write_file(file_t *filebub, char *data)
{
    if (!filebub->fp) {
        fprintf(stderr, "Invalid file pointer\n");
        return -1;
    }
    int ret = fwrite(data, sizeof(char), strlen(data), filebub->fp);
    
    return ret;
}

struct stat *get_attrib(file_t *filebub)
{
    struct stat *attribs = malloc(sizeof(struct stat));
    if (!attribs) {
        return NULL;
    }
    if (stat(filebub->filename, attribs) < 0) {
        fprintf(stderr, "File Error: %s\n", strerror(errno));
        free(attribs);
        return NULL;
    }
    return attribs;
}

            // struct stat {
            //    dev_t     st_dev;         /* ID of device containing file */             ino_t     st_ino;         /* Inode number */
            //    mode_t    st_mode;        /* File type and mode */
            //    nlink_t   st_nlink;       /* Number of hard links */
            //    uid_t     st_uid;         /* User ID of owner */
            //    gid_t     st_gid;         /* Group ID of owner */
            //    dev_t     st_rdev;        /* Device ID (if special file) */
            //    off_t     st_size;        /* Total size, in bytes */
            //    blksize_t st_blksize;     /* Block size for filesystem I/O */
            //    blkcnt_t  st_blocks;      /* Number of 512B blocks allocated */
int change_attrib(file_t *filebub, char *mode, uid_t owner, gid_t group)
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

int delete_file(char *name)
{
    if (!name) {
        fprintf(stderr, "invalid filename\n");
        return -1;
    }

    int ret = remove(name);
    if (ret != 0) {  
        printf("Unable to delete the file\n");
        free(name);
        return -1;
    }

    return 0;
}

int get_date_mod(file_t *filebub, char *date)
{
    struct stat *attribs = get_attrib(filebub);
    if (!attribs) {
        return -1;
    }
    strftime(date, 36, "%d.%m.%Y %H:%M:%S", localtime(&attribs->st_mtime));
    free(attribs);
    return 0;
}

int get_date_create(file_t *filebub, char *date)
{
    struct stat *attribs = get_attrib(filebub);
    if (!attribs) {
        return -1;
    }
    strftime(date, 36, "%d.%m.%Y %H:%M:%S", localtime(&attribs->st_ctime));
    free(attribs);
    return 0;
}

// encryption algorithm by andrey.117
// https://cboard.cprogramming.com/c-programming/155191-xor-file-encryption-c.html
// 29JUL2021
int crypt(file_t *filebub, char *key, size_t keylen)
{
    struct stat *attribs = get_attrib(filebub);
    long int fsize = attribs->st_size;
    
    char *buffer = malloc(fsize);

    if (fread(buffer, fsize, 1, filebub->fp) != 1) {
        fprintf(stderr, "Error reading file\n");
        free(buffer);
        free(attribs);
        return -1;
    }
    for (int i = 0; i < fsize; ++i) {
        buffer[i] = buffer[i] ^ key[i % keylen];
    }
    rewind(filebub->fp);
    if (fwrite(buffer, fsize, 1, filebub->fp) != 1) {
        fprintf(stderr, "Error writing encrypted data to file\n");
        free(buffer);
        free(attribs);
        return -1;
    }
    rewind(filebub->fp);
    free(attribs);
    free(buffer);
    return 0;
}

