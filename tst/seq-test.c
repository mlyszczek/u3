/* ==========================================================================
    Licensed under BSD 2clause license See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ==========================================================================
                   _               __            __
                  (_)____   _____ / /__  __ ____/ /___   _____
                 / // __ \ / ___// // / / // __  // _ \ / ___/
                / // / / // /__ / // /_/ // /_/ //  __/(__  )
               /_//_/ /_/ \___//_/ \__,_/ \__,_/ \___//____/

   ========================================================================== */


#include "config.h"

#if HAVE_LINUX_LIMITS_H
#   include <linux/limits.h>
#endif

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "fops.h"
#include "mtest.h"
#include "std-redirects.h"
#include "u3.h"


/* ==========================================================================
                         __       ____ _
                    ____/ /___   / __/(_)____   ___   _____
                   / __  // _ \ / /_ / // __ \ / _ \ / ___/
                  / /_/ //  __// __// // / / //  __/(__  )
                  \__,_/ \___//_/  /_//_/ /_/ \___//____/

   ========================================================================== */


mt_defs();
#define EXPECTED_DIR TEST_DATA_DIR"/seq"
#define SEQ_TEST_STDOUT "./seq-test-stdout"
#define SEQ_TEST_STDERR "./seq-test-stderr"


/* ==========================================================================
                          __
                         / /_ __  __ ____   ___   _____
                        / __// / / // __ \ / _ \ / ___/
                       / /_ / /_/ // /_/ //  __/(__  )
                       \__/ \__, // .___/ \___//____/
                           /____//_/
   ========================================================================== */


struct valid_params
{
    long first;
    long increment;
    long last;
};


struct invalid_params
{
    char  *first;
    char  *increment;
    char  *last;
    int    errnum;
};


/* ==========================================================================
                                   _         __     __
              _   __ ____ _ _____ (_)____ _ / /_   / /___   _____
             | | / // __ `// ___// // __ `// __ \ / // _ \ / ___/
             | |/ // /_/ // /   / // /_/ // /_/ // //  __/(__  )
             |___/ \__,_//_/   /_/ \__,_//_.___//_/ \___//____/

   ========================================================================== */


char long_max[32];  /* string representation of LONG_MAX */
char long_min[32];  /* string representation of LONG_MIN */


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
    stdout_to_file(SEQ_TEST_STDOUT);
}


/* ==========================================================================
   ========================================================================== */


static void cleanup_test(void)
{
    restore_stdout();
    unlink(SEQ_TEST_STDOUT);
    unlink(SEQ_TEST_STDERR);
}


/* ==========================================================================
    Replaces "LONG_MAX" or "LONG_MIN" strings with proper pointer to numeric
    value.
   ========================================================================== */


static void long_to_num
(
    struct invalid_params *p
)
{
    if (p->first && strcmp(p->first, "LONG_MAX") == 0)
    {
        p->first = long_max;
    }

    if (p->first && strcmp(p->first, "LONG_MIN") == 0)
    {
        p->first = long_min;
    }

    if (p->increment && strcmp(p->increment, "LONG_MAX") == 0)
    {
        p->increment = long_max;
    }

    if (p->increment && strcmp(p->increment, "LONG_MIN") == 0)
    {
        p->increment = long_min;
    }

    if (p->last && strcmp(p->last, "LONG_MAX") == 0)
    {
        p->last = long_max;
    }

    if (p->last && strcmp(p->last, "LONG_MIN") == 0)
    {
        p->last = long_min;
    }
}


/* ==========================================================================
    Runs test with valid <last> parameter
   ========================================================================== */


static void valid_test_1
(
    struct valid_params  *p
)
{
    int                   argc   = 2;
    char                 *argv[] = { "seq", NULL, NULL };
    char                  last_s[10 + 1];
    char                  expected_file[PATH_MAX];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    sprintf(last_s, "%ld", p->last);
    sprintf(expected_file, "%s/%s", EXPECTED_DIR, last_s);
    argv[1] = last_s;

    mt_fok(u3_seq_main(argc, argv));
    rewind_stdout_file();
    mt_fail(file_equal(expected_file, SEQ_TEST_STDOUT) == 1);
}


/* ==========================================================================
    Runs test with valid <first> and <last> parameters.
   ========================================================================== */


static void valid_test_2
(
    struct valid_params  *p
)
{
    int                   argc   = 3;
    char                 *argv[] = { "seq", NULL, NULL, NULL };
    char                  first_s[10 + 1];
    char                  last_s[10 + 1];
    char                  expected_file[PATH_MAX];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    sprintf(first_s, "%ld", p->first);
    sprintf(last_s, "%ld", p->last);
    sprintf(expected_file, "%s/%s_%s", EXPECTED_DIR, first_s, last_s);
    argv[1] = first_s;
    argv[2] = last_s;

    mt_fok(u3_seq_main(argc, argv));
    rewind_stdout_file();
    mt_fail(file_equal(expected_file, SEQ_TEST_STDOUT) == 1);
}


/* ==========================================================================
    Runs test with all 3, <first>, <increment> and <last> parameters
   ========================================================================== */


static void valid_test_3
(
    struct valid_params  *p
)
{
    int                   argc   = 4;
    char                 *argv[] = { "seq", NULL, NULL, NULL, NULL };
    char                  first_s[10 + 1];
    char                  increment_s[10 + 1];
    char                  last_s[10 + 1];
    char                  expected_file[PATH_MAX];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    sprintf(first_s, "%ld", p->first);
    sprintf(increment_s, "%ld", p->increment);
    sprintf(last_s, "%ld", p->last);
    sprintf(expected_file, "%s/%s_%s_%s", EXPECTED_DIR, first_s,
        increment_s, last_s);
    argv[1] = first_s;
    argv[2] = increment_s;
    argv[3] = last_s;

    mt_fok(u3_seq_main(argc, argv));
    rewind_stdout_file();
    mt_fail(file_equal(expected_file, SEQ_TEST_STDOUT) == 1);
}


/* ==========================================================================
    Runs test with <last> parameter which will be invalid
   ========================================================================== */


static void invalid_test_1
(
    struct invalid_params  *p
)
{
    int                     argc   = 2;
    char                   *argv[] = { "seq", NULL, NULL };
    char                    buf;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    argv[1] = p->last;
    buf = '\0';

    stderr_to_file(SEQ_TEST_STDERR);
    mt_ferr(u3_seq_main(argc, argv), p->errnum);
    restore_stderr();
    rewind_stdout_file();
    read_stdout_file(&buf, 1);
    mt_fail(buf == '\0');
}


/* ==========================================================================
    Runs test with <first> and <last> parameters which at least on of them
    must be invalid
   ========================================================================== */


static void invalid_test_2
(
    struct invalid_params  *p
)
{
    int                     argc   = 3;
    char                   *argv[] = { "seq", NULL, NULL, NULL };
    char                    buf;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    argv[1] = p->first;
    argv[2] = p->last;
    buf = '\0';

    stderr_to_file(SEQ_TEST_STDERR);
    mt_ferr(u3_seq_main(argc, argv), p->errnum);
    restore_stderr();
    rewind_stdout_file();
    read_stdout_file(&buf, 1);
    mt_fail(buf == '\0');
}


/* ==========================================================================
    Runs test with <first>, <last> and increment  parameters which at least
    on of them must be invalid
   ========================================================================== */


static void invalid_test_3
(
    struct invalid_params  *p
)
{
    int                     argc   = 4;
    char                   *argv[] = { "seq", NULL, NULL, NULL, NULL };
    char                    buf;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    argv[1] = p->first;
    argv[2] = p->increment;
    argv[3] = p->last;
    buf = '\0';

    stderr_to_file(SEQ_TEST_STDERR);
    mt_ferr(u3_seq_main(argc, argv), p->errnum);
    restore_stderr();
    rewind_stdout_file();
    read_stdout_file(&buf, 1);
    mt_fail(buf == '\0');
}


/* ==========================================================================
                           __               __
                          / /_ ___   _____ / /_ _____
                         / __// _ \ / ___// __// ___/
                        / /_ /  __/(__  )/ /_ (__  )
                        \__/ \___//____/ \__//____/

   ========================================================================== */


/* ==========================================================================
    Performs multiple tests, only valid values are generated here. After
    arguments are generated they are passed to seq_main() and output is
    compared to previously generated data in 'data/seq'.
   ========================================================================== */


static void valid_tests(void)
{
    /* values that will be used as <first>, <increment> and <last>
     * arguments, only positive numbers goes here, every number will
     * have generated negative value as well. It is typed as char,
     * because testing with bigger numbers gives no advantage but
     * tests take much longer. Also it is easier to know array size
     */

    const char    vals[] = { 0, 1, 2, 3, 5, 13, 21, 34, 55, 89, 127 };
    struct valid_params p;
    int           first;
    int           increment;
    int           last;
    char          tname[256];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    /* first we test when only <last> argument is passed
     */

    for (last = 0; last != sizeof(vals); ++last)
    {
        /* positive number test
         */

        p.last = vals[last];
        sprintf(tname, "seq_test %4ld", p.last);
        mt_run_param_named(valid_test_1, &p, tname);

        /* and negative, but only when not 0, try to negate 0
         */

        if (last == 0)
        {
            continue;
        }

        p.last = -vals[last];
        sprintf(tname, "seq_test %4ld", p.last);
        mt_run_param_named(valid_test_1, &p, tname);
    }

    /* now it's time for tests with <first> and <last> defined
     */

    for (first = 0; first != sizeof(vals); ++first)
    {
        for (last = 0; last != sizeof(vals); ++last)
        {
            /* positive number test
             */

            p.first = vals[first];
            p.last = vals[last];
            sprintf(tname, "seq_test %4ld %4ld", p.first, p.last);
            mt_run_param_named(valid_test_2, &p, tname);

            /* and negative, but only when not 0
             */

            if (first == 0 || last == 0)
            {
                continue;
            }

            p.first = -vals[first];
            p.last = -vals[last];
            sprintf(tname, "seq_test %4ld %4ld", p.first, p.last);
            mt_run_param_named(valid_test_2, &p, tname);
        }
    }

    /* and now but not least, test with all 3 parameters,
     * <first>, <increment> and <last>
     *
     * increment starts from 1, because increment == 0
     * is not valid, and we test only valid values here
     */

    for (first = 0; first != sizeof(vals); ++first)
    {
        for (increment = 1; increment != sizeof(vals); ++increment)
        {
            for (last = 0; last != sizeof(vals); ++last)
            {
                /* positive number test
                */

                p.first = vals[first];
                p.increment = vals[increment];
                p.last = vals[last];

                sprintf(tname, "seq_test %4ld %4ld %4ld", p.first,
                    p.increment, p.last);
                mt_run_param_named(valid_test_3, &p, tname);

                /* and negative, but only when not 0,
                 * increment will not be 0 here, never
                 */

                if (first == 0 || last == 0)
                {
                    continue;
                }

                p.first = -vals[first];
                p.increment = -vals[increment];
                p.last = -vals[last];
                sprintf(tname, "seq_test %4ld %4ld %4ld", p.first,
                    p.increment, p.last);
                mt_run_param_named(valid_test_3, &p, tname);
            }
        }
    }
}


/* ==========================================================================
    Runs multiple tests with invalid arguments
   ========================================================================== */


static void invalid_tests(void)
{
    char tname[256];
    struct invalid_params *p;

    struct invalid_params params_1[] =
    {
        /* only <last> argument is passed
         */

        {         "",         "",         "", EINVAL },
        {         "",         "",     "123f", EINVAL },
        {         "",         "",     "f123", EINVAL },
        {         "",         "", "LONG_MAX", ERANGE },
        {         "",         "", "LONG_MIN", ERANGE },

        {       NULL,       NULL,       NULL,      0 }
    };

    struct invalid_params params_2[] =
    {
        /* <first> and <last> arguments passed
         */

        {        "2",         "",     "123f", EINVAL },
        {        "3",         "",     "f123", EINVAL },
        {        "5",         "", "LONG_MAX", ERANGE },
        {        "6",         "", "LONG_MIN", ERANGE },

        {     "123f",         "",        "2", EINVAL },
        {     "f123",         "",        "3", EINVAL },
        { "LONG_MAX",         "",        "5", ERANGE },
        { "LONG_MIN",         "",        "6", ERANGE },

        {       NULL,       NULL,       NULL,      0 }
    };

    struct invalid_params params_3[] =
    {
        /* all 3 arguments are passed
         */

        {        "2",        "2",     "123f", EINVAL },
        {        "3",        "3",     "f123", EINVAL },
        {        "5",        "5", "LONG_MAX", ERANGE },
        {        "6",        "6", "LONG_MIN", ERANGE },

        {        "2",     "123f",        "2", EINVAL },
        {        "3",     "f123",        "3", EINVAL },
        {        "5", "LONG_MAX",        "5", ERANGE },
        {        "6", "LONG_MIN",        "6", ERANGE },
        {        "1",        "0",        "7", EINVAL },

        {        "2",     "123f",     "123f", EINVAL },
        {        "3",     "f123",     "f123", EINVAL },
        {        "5", "LONG_MAX", "LONG_MAX", ERANGE },
        {        "6", "LONG_MIN", "LONG_MIN", ERANGE },
        {        "7",        "0", "LONG_MIN", ERANGE },

        {     "123f",        "2",        "2", EINVAL },
        {     "f123",        "3",        "3", EINVAL },
        { "LONG_MAX",        "5",        "5", ERANGE },
        { "LONG_MIN",        "6",        "6", ERANGE },

        {     "123f",        "2",     "123f", EINVAL },
        {     "f123",        "3",     "f123", EINVAL },
        { "LONG_MAX",        "5", "LONG_MAX", ERANGE },
        { "LONG_MIN",        "6", "LONG_MIN", ERANGE },

        {     "123f",     "123f",        "2", EINVAL },
        {     "f123",     "f123",        "3", EINVAL },
        { "LONG_MAX", "LONG_MAX",        "5", ERANGE },
        { "LONG_MIN", "LONG_MIN",        "6", ERANGE },
        { "LONG_MIN",        "0",        "7", ERANGE },

        {         "",         "",         "", EINVAL },
        {     "123f",     "123f",     "123f", EINVAL },
        {     "f123",     "f123",     "f123", EINVAL },
        { "LONG_MAX", "LONG_MAX", "LONG_MAX", ERANGE },
        { "LONG_MIN", "LONG_MIN", "LONG_MIN", ERANGE },
        { "LONG_MIN",        "0", "LONG_MIN", ERANGE },

        {       NULL,       NULL,       NULL,      0 }
    };
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    sprintf(long_max, "%ld", LONG_MAX);
    sprintf(long_min, "%ld", LONG_MIN);

    for (p = params_1; p->errnum != 0; ++p)
    {
        long_to_num(p);
        sprintf(tname, "seq_test (invalid) %s", p->last ? p->last : "(null)");
        mt_run_param_named(invalid_test_1, p, tname);
    }

    for (p = params_2; p->errnum != 0; ++p)
    {
        long_to_num(p);
        sprintf(tname, "seq_test (invalid) %s %s",
                p->first ? p->first : "(null)",
                p->last ? p->last : "(null)");
        mt_run_param_named(invalid_test_2, p, tname);
    }

    for (p = params_3; p->errnum != 0; ++p)
    {
        long_to_num(p);
        sprintf(tname, "seq_test (invalid) %s %s %s",
                p->first ? p->first : "(null)",
                p->increment ? p->increment : "(null)",
                p->last ? p->last : "(null)");
        mt_run_param_named(invalid_test_3, p, tname);
    }
}


/* ==========================================================================
   ========================================================================== */


static void seq_print_help(void)
{
    int    argc = 2;
    char  *argv[] = { "seq", "-h", NULL };
    char   buf[128] = {0};
    char  *expected =
        "usage: seq <last>\n"
        "       seq <first> <last>\n"
        "       seq <first> <increment> <last>\n";
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    stderr_to_file(SEQ_TEST_STDERR);
    mt_fok(u3_seq_main(argc, argv));

    rewind_stderr_file();
    read_stderr_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    restore_stderr();
}


/* ==========================================================================
   ========================================================================== */


static void seq_print_version(void)
{
    int    argc = 2;
    char  *argv[] = { "seq", "-v", NULL };
    char   buf[128] = {0};
    char  *expected = "seq v";
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    stderr_to_file(SEQ_TEST_STDERR);
    mt_fok(u3_seq_main(argc, argv));

    rewind_stderr_file();
    read_stderr_file(buf, sizeof(buf));
    mt_fail(strncmp(buf, expected, strlen(expected)) == 0);
    restore_stderr();
}


/* ==========================================================================
   ========================================================================== */


static void seq_stdout_error(void)
{
    int    argc = 2;
    char  *argv[] = { "seq", "5" };
    char   buf[128] = {0};
    char  *expected = "e/printf()";
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    stderr_to_file(SEQ_TEST_STDERR);
    stdout_sabotage(SEQ_TEST_STDOUT);
    mt_fail(u3_seq_main(argc, argv) == -1);
    stdout_recover();

    rewind_stderr_file();
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
    mt_run(seq_stdout_error);

    mt_prepare_test = prepare_test;
    mt_cleanup_test = cleanup_test;

    valid_tests();
    invalid_tests();

    mt_run(seq_print_help);
    mt_run(seq_print_version);
    mt_return();
}
