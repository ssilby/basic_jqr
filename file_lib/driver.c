#include <stdio.h>
#include <stdlib.h>

#include "file_lib.h"

int main(void)
{
    char filename[20] = "badname.txt";
    FILE *fp;
    fp = open_file(filename);

    char goodfile[20] = "test/test.txt";
    fp = open_file(goodfile);

    char buffer[20] = {'\0'};
    int bytes_read = 0;
    bytes_read = read_file(buffer, 15, fp);
    printf("%s\n", buffer);
    fclose(fp);
    return 0;
}
