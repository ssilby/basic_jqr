#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
static size_t Scramble(const char* source);

int main()
{
    char input[17];
    int truth = 0;
    static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567890!@#$%^&*()_+-=`~[]{}|,./<>?\\ ";
    ulong i;
    char * username = getenv("GITLAB_USER_NAME");
    size_t seed = 0;

    if ( NULL != username ) 
    {
        seed = Scramble(username);
    } 
    else 
    {
    	time_t t;
	seed = (unsigned) time(&t);
    } 
    srand(seed);

    // Create random string of 16 chars

    for (i = 0; i < 16; ++i)
    {
        input[i] = charset[rand() % (sizeof(charset) - 1)];
    }

    input[i] = 0;  // Terminate string

    FILE *randfile = fopen("random.txt", "w");

    fputs(input, randfile);

    fclose(randfile);

    return 0;
}


static size_t Scramble(const char* source)
{
    size_t length = strlen(source);
    size_t code = 0;
    for(size_t i = 0; i < length; i++)
    {
        char c = source[i];
        size_t a = c - i % 2;
        code = (code * 10) + a;
    }

    return code;
}

