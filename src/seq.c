/* ==========================================================================
    Licensed under BSD 2clause license See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */


#define U3_SEQ_VERSION "v0.1.0"


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
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <u3.h>
#include <u3defs.h>


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
        "usage: seq <last>\n"
        "       seq <first> <last>\n"
        "       seq <first> <increment> <last>\n"
        "\n"
        "Prints number from <first> to <last> in steps of <increment>\n"
        "\n"
        "* if <first> or <increment> is not defined, it will be set to 1\n"
        "* <fist>, <increment> and <last> are all of type \"long int\"\n"
        "* possible values are (-LONG_MAX, LONG_MAX)\n"
    );
}


/* ==========================================================================
    Converts string number 'num' into number representation. Converted value
    will be stored in address pointed by 'n'.
   ========================================================================== */


static int get_number
(
    const char  *num,  /* string to convert to number */
    long        *n     /* converted num will be placed here */
)
{
    const char  *ep;   /* endptr for strtol function */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    if (num == NULL)
    {
        /* this shouldn't happen as this situation is forbidden by
         * the C standard, but we all know things just happen
         */

        fprintf(stderr, "e/number string is NULL\n");
        errno = EINVAL;
        return -1;
    }

    if (*num == '\0')
    {
        fprintf(stderr, "e/number is an empty string\n");
        errno = EINVAL;
        return -1;
    }

    *n = strtol(num, (char **)&ep, 10);

    if (*ep != '\0')
    {
        fprintf(stderr, "e/invalid number passed\n");
        errno = EINVAL;
        return -1;
    }

    if (*n == LONG_MAX || *n == LONG_MIN)
    {
        fprintf(stderr, "e/number is out of range\n");
        errno = ERANGE;
        return -1;
    }

    return 0;
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
int u3_seq_main
#endif
(
    int  argc,
    char *argv[]
)
{
    long   first;
    long   increment;
    long   last;
    long   current;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    /* if some arguments are not passed, their default value should be 1
     */

    first = 1;
    increment = 1;
    last = 1;

    /* parse arguments, yup, using 'current' as temporary error value
     * but in this switch-case only
     */

    current = 0;

    /* we are untrustful and assume user passed wrong number of
     * arguments
     */

    errno = EINVAL;

    switch (argc)
    {
        /* yes, only break in this switch is at the very end, no
         * mistake here!
         */

    case 4:
        /* when all arguments are passed, increment is at 'argc == 2'
         */

        current |= get_number(argv[2], &increment);

    case 3:
         /* if more than 2 arguments are passed, 'first' will always
          * be at 'argc == 1' position
          */

        current |= get_number(argv[1], &first);

    case 2:
        /* 'last' argument is always present and always is at 'argc - 1'
         * position
         */

        current |= get_number(argv[argc - 1], &last);

        if (current == 0)
        {
            /* break out of switch only when there was no error parsing
             * arguments, otherwise go to default to exit
             */

            break;
        }

    default:
        /* invalid number of arguments
         */

        print_help();
        return U3_EXIT_FAILURE;
    }

    if (increment == 0)
    {
        /* bonus check, increment cannot be 0, it would cause infinite
         * loop
         */

        fprintf(stderr, "e/increment number cannot be 0\n");
        errno = EINVAL;
        return U3_EXIT_FAILURE;
    }

    /* arguments parsed, now print numbers
     */

    for (current = first;

        /* if increment is positive, we count from lower number to
         * bigger one (like 1 2 3 4)
         *
         * if increment is negative, we count from bigger number to
         * lower one (like 4 3 2 1) thus we swap check or else calling
         * 'seq 10 -1 5' would return without printing anything and
         * we want to count down
         */

        increment > 0 ? current <= last : current >= last;
        current += increment)
    {
        if (printf("%ld\n", current) < 0)
        {
            /* when stdout fails there is still chance stderr will
             * be available (like piped to some other file, whatever)
             */

            fprintf(stderr, "e/printf()");
            return U3_EXIT_FAILURE;
        }
    }

    fflush(stdout);
    return 0;
}
