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
                       __     __ _          ____
        ____   __  __ / /_   / /(_)_____   / __/__  __ ____   _____ _____
       / __ \ / / / // __ \ / // // ___/  / /_ / / / // __ \ / ___// ___/
      / /_/ // /_/ // /_/ // // // /__   / __// /_/ // / / // /__ (__  )
     / .___/ \__,_//_.___//_//_/ \___/  /_/   \__,_//_/ /_/ \___//____/
    /_/
   ========================================================================== */


/* ==========================================================================
    Converts string number 'num' into number representation. Converted value
    will be stored in address pointed by 'n'.
   ========================================================================== */


int u3u_get_number
(
    const char  *num,  /* string to convert to number */
    long        *n     /* converted num will be placed here */
)
{
    const char  *ep;   /* endptr for strtol function */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    if (*num == '\0')
    {
        fprintf(stderr, "e/number is an empty string\n");
        errno = EINVAL;
        return -1;
    }

    *n = strtol(num, (char **)&ep, 10);

    if (*ep != '\0')
    {
        fprintf(stderr, "e/invalid number passed: '%s'\n", num);
        errno = EINVAL;
        return -1;
    }

    if (*n == LONG_MAX || *n == LONG_MIN)
    {
        fprintf(stderr, "e/number is out of range: '%s'\n", num);
        errno = ERANGE;
        return -1;
    }

    return 0;
}
