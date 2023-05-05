#ifndef JQR_362_SECURE_TESTS_H
#define JQR_362_SECURE_TESTS_H

#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#include "../include/fixme_pls.h"

#define TEST_STDOUT_CAPTURE_FILE "/tmp/CUnit_JQR_3115_SM_TEST_stdout.log"
#define TEST_STDIN_FILE "/tmp/CUnit_JQR_3115_SM_TEST_stdin.log"

static int    saved_stdout;
static fpos_t saved_stdout_pos;

static int saved_stdin;

/**
 * @brief Internal helper function to redirect stdout to a file.
 */
static void
_redirect_stdout()
{
    fgetpos(stdout, &saved_stdout_pos);
    saved_stdout = dup(STDOUT_FILENO);
    freopen(TEST_STDOUT_CAPTURE_FILE, "w+", stdout);
    setvbuf(stdout, NULL, _IONBF, 0);
}

/**
 * @brief Internal helper function to read stdout and then restore the stdout
 * fd.
 */
static char *
_capture_and_restore_stdout()
{
    char *p_output = NULL;

    // OK because we know that it's pointing to a file and files are seekable
    fseek(stdout, 0L, SEEK_END);
    uint64_t file_size = ftell(stdout);
    fseek(stdout, 0L, SEEK_SET);
    p_output = calloc(1, file_size + 1);
    fread(p_output, file_size, 1, stdout);

    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
    clearerr(stdout);
    fsetpos(stdout, &saved_stdout_pos);

    return p_output;
}

/**
 * @brief Internal helper function to redirect stdin to read from a file.
 */
static void
_redirect_stdin()
{
    saved_stdin = dup(STDIN_FILENO);
    freopen(TEST_STDIN_FILE, "r", stdin);
    setvbuf(stdin, NULL, _IONBF, 0);
}

/**
 * @brief Internal helper function to restore the stdin fd.
 */
static void
_restore_stdin()
{
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);
}

/**
 * @brief Internal helper function to write the command input to the file that
 * we are using for stdin.
 * @param input The input to write to the file
 * @param fname The filename to write to
 */
static int
_write_file(const char *input, const char *fname)
{
    FILE *input_file = fopen(fname, "w");
    int   wrote      = fputs(input, input_file);
    fclose(input_file);

    return wrote;
}

/**
 * @brief Internal helper function to redirect stdin/stdout, call the main
 * event loop. Captures the output of the run and then restores stdin/stdout.
 *
 * @param input The input for the specific test run
 * @return Pointer to the captured stdout
 */
static char *
_setup_run_teardown(void)
{
    char *p_output;
    _redirect_stdin();
    _redirect_stdout();
    my_main();
    p_output = _capture_and_restore_stdout();
    _restore_stdin();
    return p_output;
}

void jqr_test_name_valid(void)
{
    CU_ASSERT_TRUE(name_is_valid("superlongBUTVALIDname29384_"));
    CU_ASSERT_TRUE(name_is_valid(""));
    CU_ASSERT_TRUE(name_is_valid("_1111111111"));
    CU_ASSERT_FALSE(name_is_valid(" "));
    CU_ASSERT_FALSE(name_is_valid("John Doe"));
    CU_ASSERT_FALSE(name_is_valid("this-is-my-name"));
}

void jqr_test_sanitize_name(void)
{
#define SIZE 6
    char *new;
    bool res;
    char *inputs[SIZE] = {"superlongBUTVALIDname29384_", "", "_11111", " ", "John Doe", "this-is-my-name"};
    char *outputs[SIZE] = {"superlongBUTVALIDname29384_", "", "_11111", "", "JohnDoe", "thisismyname"};

    for (int i = 0; i < SIZE; ++i)
    {
        new = sanitize_name(inputs[i]);
        res = strcmp(new, outputs[i]);
        CU_ASSERT_EQUAL(0, res);
        if (res != 0)
        {
            printf("Expected '%s'\nReceived '%s'\n", outputs[i], new);
        }
        CU_ASSERT_EQUAL(0, strcmp(new, outputs[i]));
        free(new);
    }
#undef SIZE
}

void jqr_test_bss_overflow(void)
{
    char *capture;
    int res;
    char *input = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n3\n0\n";
    char *expected = "What is your name?\n"
        "> Welcome, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
        "0) Exit\n1) Echo Chamber\n2) Play game\n\n"
        "Congrats you win\n"
        "0) Exit\n1) Echo Chamber\n2) Play game\n\nBye\n";

    _write_file(input, TEST_STDIN_FILE);
    capture = _setup_run_teardown();
    res = strcmp(capture, expected);
    CU_ASSERT_NOT_EQUAL(0, res);
    if (res == 0)
    {
        printf("Your code is still vulnerable\n");
    }
    free(capture);
}

void jqr_test_main_format(void)
{
    char *capture;
    int res;
    char *input = "%p%p%p%p\n0\n";
    char *expected = "What is your name?\n"
        "> Welcome, pppp"
        "0) Exit\n1) Echo Chamber\n2) Play game\n\nBye\n";

    _write_file(input, TEST_STDIN_FILE);
    capture = _setup_run_teardown();
    res = strcmp(capture, expected);
    CU_ASSERT_EQUAL(0, res);
    if (res)
    {
        printf("Your code is still vulnerable\n");
    }
    free(capture);
}

void jqr_test_echo(void)
{
    char *capture;
    int res;
    char *input = "A\n1\n%p%p%p%p\n0\n";
    char *expected = "What is your name?\n"
        "> Welcome, A"
        "0) Exit\n1) Echo Chamber\n2) Play game\n\n"
        "What do you want to say?\n"
        "%p%p%p%p"
        "0) Exit\n1) Echo Chamber\n2) Play game\n\nBye\n";

    _write_file(input, TEST_STDIN_FILE);
    capture = _setup_run_teardown();
    res = strcmp(capture, expected);
    CU_ASSERT_EQUAL(0, res);
    if (res)
    {
        printf("Your code is still vulnerable\n");
    }
    free(capture);
}

void jqr_test_play_game(void)
{
    char *capture;
    int res;
    char *input = "A\n2\nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\xef\xbe\xad\xde\n0\n";
    char *expected = "What is your name?\n"
        "> Welcome, A"
        "0) Exit\n1) Echo Chamber\n2) Play game\n\n"
        "Try your luck\nGood job\n"
        "0) Exit\n1) Echo Chamber\n2) Play game\n\nBye\n";

    _write_file(input, TEST_STDIN_FILE);
    capture = _setup_run_teardown();
    res = strcmp(capture, expected);
    CU_ASSERT_NOT_EQUAL(0, res);
    if (!res)
    {
        printf("Your code is still vulnerable\n");
    }
    free(capture);
}

void jqr_test_zero_memory(void)
{
    char secret[256];
    void *ret;
    for (int i = 0; i < 256; ++i)
    {
        secret[i] = i;
    }

    ret = zero_memory(secret, 256);
    CU_ASSERT_PTR_EQUAL(ret, secret);
    for (int i = 0; i < 256; ++i)
    {
        if (secret[i] != 0)
        {
            CU_FAIL("Failed to zero out memory");
        }
    }
}

#endif
