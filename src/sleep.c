/* ==========================================================================
    Licensed under BSD 2clause license See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */


#define U3_SLEEP_VERSION "v0.1.0"


/* ==========================================================================
                   _               __            __
                  (_)____   _____ / /__  __ ____/ /___   _____
                 / // __ \ / ___// // / / // __  // _ \ / ___/
                / // / / // /__ / // /_/ // /_/ //  __/(__  )
               /_//_/ /_/ \___//_/ \__,_/ \__,_/ \___//____/

   ========================================================================== */


#if HAVE_CONFIG_H
#   include "config.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include "utils.h"
#include "u3.h"
#include "u3defs.h"


/* ==========================================================================
               ____                     __   _
              / __/__  __ ____   _____ / /_ (_)____   ____   _____
             / /_ / / / // __ \ / ___// __// // __ \ / __ \ / ___/
            / __// /_/ // / / // /__ / /_ / // /_/ // / / /(__  )
           /_/   \__,_//_/ /_/ \___/ \__//_/ \____//_/ /_//____/

   ========================================================================== */


static void print_help(void)
{
    fprintf(stderr,
        "usage: sleep <time>[.<fraction>]\n"
        "       sleep <option>\n"
        "\n"
        "Pause execution for time seconds.\n"
        "\n"
        "\t<time>       number of seconds to pause\n"
        "\t<fraction>   number of fraction of seconds to pause\n"
        "\t-h           show this help\n"
        "\t-v           show version and exit\n"
    );
}


/* ==========================================================================
                                              _
                           ____ ___   ____ _ (_)____
                          / __ `__ \ / __ `// // __ \
                         / / / / / // /_/ // // / / /
                        /_/ /_/ /_/ \__,_//_//_/ /_/

   ========================================================================== */


#if U3_STANDALONE
int main
#else
int u3_sleep_main
#endif
(
    int              argc,
    char            *argv[]
)
{
    char            *sfractions;
    struct timespec  request;
    long             nano;
    long             seconds;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    if (argc != 2)
    {
        fprintf(stderr, "wrong number of arguments passed\n");
        print_help();
        return 1;
    }

    if (argv[1][0] == '-')
    {
        if (argv[1][1] == 'v')
        {
            /* '-v' passed, print version and exit
            */

            fprintf(stderr, "sleep " U3_SLEEP_VERSION "\n"
                    "u3 " U3_VERSION "\n");
            return 0;
        }

        if (argv[1][1] == 'h')
        {
            /* '-h' passed, print help and exit
            */

            print_help();
            return 0;
        }

        /* minus encoutered without option, negative number?
         * not likely!
         */

        fprintf(stderr, "negative seconds passed: '%s'\n", argv[1]);
        return 1;
    }

    /* no '-' option, assume argument is number
     */

    nano = 1000000000l;
    request.tv_sec = -1;
    request.tv_nsec  = -1;

    /* check if fractions are passed or whole seconds
     */

    for (sfractions = argv[1]; *sfractions != '\0'; ++sfractions)
    {
        if (*sfractions == '.')
        {
            /* fractions are enabled, change that '.' to '\0', so
             * argv[1] is valid seconds also increment sfractions
             * so it points to first number of fraction string
             */

            *sfractions++ = '\0';
            break;
        }
    }

    if (*sfractions == '-')
    {
        fprintf(stderr, "negative fractions of seconds passed: '%s'\n",
            sfractions);
        return 1;
    }

    if (u3u_get_number(argv[1], &seconds) != 0)
    {
        fprintf(stderr, "error parsing seconds part of argument\n");
        return 1;
    }

    /* tv_sec is time_t type, and u3u_get_number() gets pointer to long,
     * so we cannot directly pass tv_sec to u3u_get_number() as this
     * may lead to wrong values when sizeof(time_t) != sizeof(long)
     */

    request.tv_sec = seconds;

    if (*sfractions)
    {
        if (u3u_get_number(sfractions, &request.tv_nsec) != 0)
        {
            fprintf(stderr, "error parsing fractions of seconds part of "
                "argument\n");
            return 1;
        }
    }

    for (; *sfractions != '\0'; ++sfractions)
    {
        nano /= 10;
    }

    if (nano == 0)
    {
        /* nano will 0 when user passed bigger fraction number than
         * 999999999
         */

        fprintf(stderr, "fractions cannot be bigger than 999999999\n");
        return 1;
    }

    if (request.tv_nsec == -1l)
    {
        /* fractions not set, so don't use them, set tv_nsec to 0,
         * so nanosleep() don't cry about wrong number there
         */

        request.tv_nsec = 0;
    }

    request.tv_nsec *= nano;

    /* number parsed properly, now perform sleep
     */

#if TEST_RUN
    /* when we are running tests, we don't want to sleep because
     * tests would be long and it would be hard to calculate how
     * long did we really sleep. Instead we just print value that
     * we would have slept for in normal execution.
     *
     * use seconds instead of request.tv_sec since, time_t is
     * hard to print
     */

    fprintf(stderr, "sleep for: %ld.%ld\n", seconds, request.tv_nsec);
#else
    nanosleep(&request, NULL);
#endif
    return 0;
}
