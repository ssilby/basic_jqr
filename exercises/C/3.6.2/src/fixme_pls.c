#include "../include/fixme_pls.h"

struct test usr;

/**
 * @brief Check if the provided name is valid.
 * Valid characters are [a-zA-Z0-9_]
 * @param name The name to check 
 * @return true is the name is valid false otherwise
 */
bool name_is_valid(char *name)
{
    return true;
}

/**
 * @brief Remove nonvalid characters from the name.
 * You should allocate new memory to store the new name.
 * Valid characters are [a-zA-Z0-9_]
 * @param name The name to sanitize 
 * @return A pointer to new memory that contains the sanitized name.
 */
char *sanitize_name(char *name)
{
    char *new = malloc(strlen(name));
    strcpy(new, name);
    return new;
}

void ask_name(void)
{
    char _name[0x20];
    printf("What is your name?\n> ");
    scanf("%s", _name);
    if (name_is_valid(_name))
    {
        strcpy(usr.buf, _name);
    }
    else
    {
        char *new = sanitize_name(_name);
        strcpy(usr.buf, new);
        free(new);
    }
}

int print_menu(void)
{
    unsigned int n;
    puts("0) Exit");
    puts("1) Echo Chamber");
    puts("2) Play game");
    puts("");
    scanf("%d", &n);
    return n;
}

void echo(void)
{
    char buf[0x100];
    puts("What do you want to say?");
    scanf("%s", buf);
    printf(buf);
}

void play_game(void)
{
    getc(stdin);
    struct test goodluck;
    zero_memory(&goodluck, sizeof(struct test));
    puts("Try your luck");
    fgets(goodluck.buf, 0x30, stdin);
    if (goodluck.val == 0xdeadbeef)
    {
        puts("Good job");
    }
}

void check_auth(void)
{
    if (usr.val)
    {
        puts("Congrats you win");
    }
    else 
    {
        puts("I don't think so");
    }
}

void *zero_memory(void *ptr, size_t size)
{
    return ptr;
}

int my_main(void)
{
    int choice;
    zero_memory(&user, sizeof(struct test))
    ask_name();
    printf("Welcome, ");
    printf(usr.buf);
    while (1)
    {
        choice = print_menu();
        switch (choice)
        {
            case 0:
                puts("Bye");
                return 0;
            case 1:
                echo();
                break;
            case 2:
                play_game();
                break;
            case 3:
                check_auth();
                break;
            default:
                puts("I didn't understand that");
        }
    }
}
