/* ==========================================================================
    Licensed under BSD 2clause license See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */


/* ==========================================================================
                   _               __            __
                  (_)____   _____ / /__  __ ____/ /___   _____
                 / // __ \ / ___// // / / // __  // _ \ / ___/
                / // / / // /__ / // /_/ // /_/ //  __/(__  )
               /_//_/ /_/ \___//_/ \__,_/ \__,_/ \___//____/

   ========================================================================== */


#include "config.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "mtest.h"
#include "std-redirects.h"
#include "u3.h"
#include "u3defs.h"


/* ==========================================================================
                         __       ____ _
                    ____/ /___   / __/(_)____   ___   _____
                   / __  // _ \ / /_ / // __ \ / _ \ / ___/
                  / /_/ //  __// __// // / / //  __/(__  )
                  \__,_/ \___//_/  /_//_/ /_/ \___//____/

   ========================================================================== */


mt_defs();

#define REV_TEST_FILE "./rev-test-file"


/* ==========================================================================
               ____                     __   _
              / __/__  __ ____   _____ / /_ (_)____   ____   _____
             / /_ / / / // __ \ / ___// __// // __ \ / __ \ / ___/
            / __// /_/ // / / // /__ / /_ / // /_/ // / / /(__  )
           /_/   \__,_//_/ /_/ \___/ \__//_/ \____//_/ /_//____/

   ========================================================================== */


/* ==========================================================================
   ========================================================================== */


static void prepare_test(void)
{
    stdout_to_file();
}


/* ==========================================================================
   ========================================================================== */


static void cleanup_test(void)
{
    restore_stdout();
    unlink(REV_TEST_FILE);
}


/* ==========================================================================
    Generates some ascii data that is stored to 'file' and reverse of the
    data is stored into 'revers'. Data is generated according to 'n'. 'n'
    defines how much data will be there in a single line, each element
    in array is single line. Well, look at the example

    int n[] = { 4, 2, 6, 0, 3, INT_MAX } will generate file

    1234
    12
    123456

    123

    yes, there is one empty line where there is 0.

    At the end of 'n' must be INT_MAX - which is our NULL terminator.
   ========================================================================== */


static int rev_gen_data
(
    int         *n,         /* array of number for data generator */
    int          nl,        /* add new line character at the end or not */
    const char  *file,      /* data will be generated here */
    char        *revers     /* data in file in reversed order */
)
{
    int          num;       /* current number of elements in line to generate */
    FILE        *f;         /* file to put generated data to */
    size_t       rpos;      /* position of the revers */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    f = fopen(file, "w");
    if (f == NULL)
    {
        perror("fopen()");
        return -1;
    }

    rpos = 0;
    while ((num = *n++) != INT_MAX)
    {
        char  *s;  /* string with generated data */
        int    i;  /* just an iterator */
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


        /* allocate memory for a line, +1 for '\n' character and another
         * +1 for '\0' terminator
         */

        s = malloc(num + 1 + 1);
        if (s == NULL)
        {
            perror("malloc()");
            return -1;
        }

        for (i = 0; i != num; ++i)
        {
            s[i] = 'a' + (i % 26);
        }

        /* put data in reverse to revers
         */

        for (i = 0; i != num; ++i)
        {
            revers[rpos + i] = s[num - i - 1];
        }

        if (*n == INT_MAX)
        {
            /* this is last line to process
             */

            if (nl)
            {
                /* and caller wants last line to be added
                 */

                s[i] = '\n';
                revers[rpos + i++] = '\n';
            }
        }
        else
        {
            /* there are more lines ahead to generate, add newline
             * unconditionally
             */

            s[i] = '\n';
            revers[rpos + i++] = '\n';
        }

        s[i] = '\0';
        revers[rpos + i++] = '\0';
        rpos += num + 1;

        fputs(s, f);
        free(s);
    }

    fclose(f);
    return 0;
}


/* ==========================================================================
                           __               __
                          / /_ ___   _____ / /_ _____
                         / __// _ \ / ___// __// ___/
                        / /_ /  __/(__  )/ /_ (__  )
                        \__/ \___//____/ \__//____/

   ========================================================================== */


/* ==========================================================================
   ========================================================================== */


static void rev_lib_print_help(void)
{
    int    argc = 2;
    char  *argv[] = { "rev", "-h", NULL };
    char   buf[128] = {0};
    char  *expected = "usage: rev [ -v | -h | <file> ]\n";
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    stderr_to_file();
    mt_fok(u3_rev_main(argc, argv));

    /* check if help really did print, just check first line
     */

    rewind_stderr_file();
    read_stderr_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    restore_stderr();
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_single_empty_line(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[128] = {0};
    char  expected[128];
    int   n[] = { 0, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 1, REV_TEST_FILE, expected);
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] == '\n');
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_single_line(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[128] = {0};
    char  expected[128];
    int   n[] = { 5, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 1, REV_TEST_FILE, expected);
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] == '\n');
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_single_full_line(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[U3_REV_LINE_MAX + 2] = {0};
    char  expected[U3_REV_LINE_MAX + 2];
    int   n[] = { U3_REV_LINE_MAX, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 1, REV_TEST_FILE, expected);
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] == '\n');
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_single_overflow_line(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[4 * U3_REV_LINE_MAX + 2] = {0};
    char  expected[4 * U3_REV_LINE_MAX + 2];
    int   n[] = { 4 * U3_REV_LINE_MAX, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 1, REV_TEST_FILE, expected);

#if ENABLE_MALLOC

    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] == '\n');

#else /* ENABLE_MALLOC */

    stderr_to_file();
    mt_ferr(u3_rev_main(argc, argv), ENOBUFS);
    mt_fail(read_stdout_file(buf, sizeof(buf)) == 0);
    mt_fail(buf[0] == '\0');
    rewind_stderr_file();
    read_stderr_file(buf, sizeof(buf));
    sprintf(expected, "e/line is longer than %d, aborting\n",
        U3_REV_LINE_MAX);
    mt_fail(strcmp(buf, expected) == 0);
    restore_stderr();

#endif /* ENABLE_MALLOC */
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_single_empty_line_no_nl(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[128] = {0};
    char  expected[128];
    int   n[] = { 0, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 0, REV_TEST_FILE, expected);
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    mt_fail(read_stdout_file(buf, sizeof(buf)) == 0);
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[0] == '\0');
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_single_line_no_nl(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[128] = {0};
    char  expected[128];
    int   n[] = { 5, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 0, REV_TEST_FILE, expected);
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] != '\n');
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_single_full_line_no_nl(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[U3_REV_LINE_MAX + 2] = {0};
    char  expected[U3_REV_LINE_MAX + 2];
    int   n[] = { U3_REV_LINE_MAX, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 0, REV_TEST_FILE, expected);
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] != '\n');
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_single_overflow_line_no_nl(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[4 * U3_REV_LINE_MAX + 2] = {0};
    char  expected[4 * U3_REV_LINE_MAX + 2];
    int   n[] = { 4 * U3_REV_LINE_MAX, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 0, REV_TEST_FILE, expected);

#if ENABLE_MALLOC

    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] != '\n');

#else /* ENABLE_MALLOC */

    stderr_to_file();
    mt_ferr(u3_rev_main(argc, argv), ENOBUFS);
    mt_fail(read_stdout_file(buf, sizeof(buf)) == 0);
    mt_fail(buf[0] == '\0');
    rewind_stderr_file();
    read_stderr_file(buf, sizeof(buf));
    sprintf(expected, "e/line is longer than %d, aborting\n",
        U3_REV_LINE_MAX);
    mt_fail(strcmp(buf, expected) == 0);
    restore_stderr();

#endif /* ENABLE_MALLOC */
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_multi_empty_line(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[128] = {0};
    char  expected[128];
    int   n[] = { 0, 5, 2, 0, 0, 2, 0, 1, 0, 0, 1, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 1, REV_TEST_FILE, expected);
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] == '\n');
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_multi_line(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[128] = {0};
    char  expected[128];
    int   n[] = { 5, 21, 12, 4, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 1, REV_TEST_FILE, expected);
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] == '\n');
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_multi_full_line(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[4 * U3_REV_LINE_MAX + 2] = {0};
    char  expected[4 * U3_REV_LINE_MAX + 2];
    int   n[] = { U3_REV_LINE_MAX, U3_REV_LINE_MAX, 20,
        U3_REV_LINE_MAX, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 1, REV_TEST_FILE, expected);
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] == '\n');
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_multi_overflow_line(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[8 * U3_REV_LINE_MAX + 2] = {0};
    char  expected[8 * U3_REV_LINE_MAX + 2];
    int   n[] = { 2 * U3_REV_LINE_MAX, 3 * U3_REV_LINE_MAX, 31,
        2 * U3_REV_LINE_MAX, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 1, REV_TEST_FILE, expected);

#if ENABLE_MALLOC

    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] == '\n');

#else /* ENABLE_MALLOC */

    stderr_to_file();
    mt_ferr(u3_rev_main(argc, argv), ENOBUFS);
    mt_fail(read_stdout_file(buf, sizeof(buf)) == 0);
    mt_fail(buf[0] == '\0');
    rewind_stderr_file();
    read_stderr_file(buf, sizeof(buf));
    sprintf(expected, "e/line is longer than %d, aborting\n",
        U3_REV_LINE_MAX);
    mt_fail(strcmp(buf, expected) == 0);
    restore_stderr();

#endif /* ENABLE_MALLOC */
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_multi_empty_line_no_nl(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[128] = {0};
    char  expected[128];
    int   n[] = { 0, 5, 2, 0, 0, 2, 0, 1, 0, 0, 1, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 0, REV_TEST_FILE, expected);
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] != '\n');
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_multi_line_no_nl(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[128] = {0};
    char  expected[128];
    int   n[] = { 5, 3, 6, 1, 8, 10, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 0, REV_TEST_FILE, expected);
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] != '\n');
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_multi_full_line_no_nl(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[4 * U3_REV_LINE_MAX + 2] = {0};
    char  expected[4 * U3_REV_LINE_MAX + 2];
    int   n[] = { U3_REV_LINE_MAX, U3_REV_LINE_MAX, 20,
        U3_REV_LINE_MAX, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    rev_gen_data(n, 0, REV_TEST_FILE, expected);
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] != '\n');
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_multi_overflow_line_no_nl(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[8 * U3_REV_LINE_MAX + 2] = {0};
    char  expected[8 * U3_REV_LINE_MAX + 2];
    int   n[] = { 2 * U3_REV_LINE_MAX, 3 * U3_REV_LINE_MAX, 45,
        2 * U3_REV_LINE_MAX, INT_MAX };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    rev_gen_data(n, 0, REV_TEST_FILE, expected);

#if ENABLE_MALLOC

    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] != '\n');

#else /* ENABLE_MALLOC */

    stderr_to_file();
    mt_ferr(u3_rev_main(argc, argv), ENOBUFS);
    mt_fail(read_stdout_file(buf, sizeof(buf)) == 0);
    mt_fail(buf[0] == '\0');
    rewind_stderr_file();
    read_stderr_file(buf, sizeof(buf));
    sprintf(expected, "e/line is longer than %d, aborting\n",
        U3_REV_LINE_MAX);
    mt_fail(strcmp(buf, expected) == 0);
    restore_stderr();

#endif /* ENABLE_MALLOC */
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_zero_arg(void)
{
    int   argc = 0;
    char *argv[] = { NULL };
    char  buf[128] = {0};
    char  expected[128];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    stdin_from_file();
    strcpy(expected, "987654321");
    write_stdin_file("123456789", 9);
    rewind_stdin_file();
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] != '\n');
    restore_stdin();
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_one_arg(void)
{
    int   argc = 1;
    char *argv[] = { "", NULL };
    char  buf[128] = {0};
    char  expected[128];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    stdin_from_file();
    strcpy(expected, "987654321");
    write_stdin_file("123456789", 9);
    rewind_stdin_file();
    mt_fok(u3_rev_main(argc, argv));
    rewind_stdout_file();
    read_stdout_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    mt_fail(buf[strlen(buf) - 1] != '\n');
    restore_stdin();
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_three_args(void)
{
    int   argc = 3;
    char *argv[] = { "", "", "", NULL };
    char  buf[128] = {0};
    char  *expected = "usage: rev [ -v | -h | <file> ]\n";
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    stderr_to_file();
    mt_ferr(u3_rev_main(argc, argv), EINVAL);
    rewind_stderr_file();
    mt_fail(read_stdout_file(buf, sizeof(buf)) == 0);
    read_stderr_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    restore_stderr();
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_file_not_found(void)
{
    int   argc = 2;
    char *argv[] = { "rev", "/i/dont/exist", NULL };
    char  buf[128] = {0};
    char  *expected = "e/fopen(): ";
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    stderr_to_file();
    mt_ferr(u3_rev_main(argc, argv), ENOENT);
    rewind_stderr_file();
    mt_fail(read_stdout_file(buf, sizeof(buf)) == 0);
    read_stderr_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    restore_stderr();
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_permision_denied(void)
{
    int   argc = 2;
    char *argv[] = { "rev", REV_TEST_FILE, NULL };
    char  buf[128] = {0};
    char  trash[16];
    int   n[] = { 8, INT_MAX };
    char  *expected = "e/fopen(): ";
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    stderr_to_file();
    rev_gen_data(n, 0, REV_TEST_FILE, trash);
    chmod(REV_TEST_FILE, 0200);
    mt_ferr(u3_rev_main(argc, argv), EACCES);
    fprintf(stderr, "%d\n", errno);
    rewind_stderr_file();
    mt_fail(read_stdout_file(buf, sizeof(buf)) == 0);
    read_stderr_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    restore_stderr();
}


/* ==========================================================================
   ========================================================================== */


static void rev_lib_invalid_arg(void)
{
    int   argc = 2;
    char *argv[] = { "rev", "-a" };
    char  buf[128] = {0};
    char  *expected = "invalid option -a\nusage: rev [ -v | -h | <file> ]\n";
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    stderr_to_file();
    mt_ferr(u3_rev_main(argc, argv), EINVAL);
    rewind_stderr_file();
    mt_fail(read_stdout_file(buf, sizeof(buf)) == 0);
    read_stderr_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    restore_stderr();

}


/* ==========================================================================
                                              _
                           ____ ___   ____ _ (_)____
                          / __ `__ \ / __ `// // __ \
                         / / / / / // /_/ // // / / /
                        /_/ /_/ /_/ \__,_//_//_/ /_/

   ========================================================================== */


int main(void)
{
    mt_prepare_test = &prepare_test;
    mt_cleanup_test = &cleanup_test;

    mt_run(rev_lib_print_help);
    mt_run(rev_lib_single_empty_line);
    mt_run(rev_lib_single_line);
    mt_run(rev_lib_single_full_line);
    mt_run(rev_lib_single_overflow_line);
    mt_run(rev_lib_single_empty_line_no_nl);
    mt_run(rev_lib_single_line_no_nl);
    mt_run(rev_lib_single_full_line_no_nl);
    mt_run(rev_lib_single_overflow_line_no_nl);
    mt_run(rev_lib_multi_empty_line);
    mt_run(rev_lib_multi_line);
    mt_run(rev_lib_multi_full_line);
    mt_run(rev_lib_multi_overflow_line);
    mt_run(rev_lib_multi_empty_line_no_nl);
    mt_run(rev_lib_multi_line_no_nl);
    mt_run(rev_lib_multi_full_line_no_nl);
    mt_run(rev_lib_multi_overflow_line_no_nl);
    mt_run(rev_lib_zero_arg);
    mt_run(rev_lib_one_arg);
    mt_run(rev_lib_three_args);
    mt_run(rev_lib_invalid_arg);
    mt_run(rev_lib_file_not_found);
    mt_run(rev_lib_permision_denied);

    mt_return();
}
