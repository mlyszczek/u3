/* ==========================================================================
    Licensed under BSD 2clause license See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */


#define U3_REV_VERSION "v1.0.0"


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
        "usage: rev [ -v | -h | <file> ]\n"
        "\n"
        "  -h       print this help and exit\n"
        "  -v       print version information and exit\n"
        "  <file>   path to file to reverse\n"
        "\n"
        "if <file> is passed, program revers data in given file\n"
        "else it uses piped data from another program\n"
        "else data is read from stdin\n");
}


/* ==========================================================================
                                              _
                           ____ ___   ____ _ (_)____
                          / __ `__ \ / __ `// // __ \
                         / / / / / // /_/ // // / / /
                        /_/ /_/ /_/ \__,_//_//_/ /_/

   ========================================================================== */


#if ENABLE_STANDALONE
int main
#else
int u3_rev_main
#endif
(
    int          argc,       /* number of arguments in argv */
    char        *argv[]      /* program arguments */
)
{
    const char  *file_path;  /* path to file to process */
    size_t       line_size;  /* size of the line buffer */
    size_t       line_pos;   /* data position in line buffer */
    size_t       line_len;   /* length of string in line buffer */
    FILE        *fin;        /* input file data will be read from */
    int          ret;        /* return code from the program */
    size_t       i;          /* iterator */

#if ENABLE_MALLOC
    char        *line;       /* malloced buffer to hold single line */
#else
    char         line[U3_REV_LINE_MAX + 2]; /* buffer to hold single line */
#endif
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    /* assume everything went fine
     */

    ret = 0;
    file_path = NULL;
    line_pos = 0;

    if (argc == 2)
    {
        /* exactly one argument has been passed
         */

        if (argv[1][0] == '-')
        {
            switch (argv[1][1])
            {
            case 'v':
                fprintf(stderr, "rev " U3_REV_VERSION "\n"
                    "u3 " U3_VERSION "\n");
                break;

            case 'h':
                print_help();
                break;

            default:
                fprintf(stderr, "invalid option -%c\n", argv[1][1]);
                print_help();
                ret = U3_EXIT_FAILURE;
                errno = EINVAL;
            }

            return ret;
        }

        /* argument does not start from '-' assuming it's file
         */

        file_path = argv[1];
    }
    else if (argc > 2)
    {
        print_help();
        errno = EINVAL;
        return U3_EXIT_FAILURE;
    }

    line_size = U3_REV_LINE_MAX + 2;

#if ENABLE_MALLOC

    line = malloc(line_size);

    if (line == NULL)
    {
        perror("e/malloc()");
        return U3_EXIT_FAILURE;
    }

#endif

    /* if file has been passed in argument use that as a source of data,
     * otherwise use stdin which may be actual stdin or pipe
     */

    fin = file_path ? fopen(file_path, "r") : stdin;

    if (fin == NULL)
    {
        perror("e/fopen()");
        ret = U3_EXIT_FAILURE;
        goto fopen_error;
    }

    for (;;)
    {
        /* set last byte of line buffer to something other than '\0'to know
         * whether fgets overwrite it or not
         */

        line[line_size - 1] = 0x7f;

        /* try to read whole line into the buffer
         */

        if (fgets(line + line_pos, line_size - line_pos, fin) == NULL)
        {
            if (feof(fin))
            {
                /* end of file reached and no data has been read, we're done
                 */

                break;
            }

            perror("e/error reading input file");
            ret = U3_EXIT_FAILURE;
            goto error;
        }

        if (line[line_size - 1] == '\0' && line[line_size - 2] != '\n')
        {
            /* fgets did overwrite last byte with '\0' which means it filled
             * whole line buffer
             *
             * and
             *
             * last character in string is not new * line
             */

#if ENABLE_MALLOC

            char *new_line;
            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


            /* but nothing is lost yet, malloc is enabled to we can alocate
             * more memory to satisfy that long line. In previous malloc
             * we added +2 to line_size for '\n' and '\0' characters and
             * now we want to double the line_size buffer, so we subtract
             * 2, then double the size and at the end add 2 bytes again.
             */

            line_size -= 2;
            line_size *= 2;
            line_size += 2;
            new_line = realloc(line, line_size);

            if (new_line == NULL)
            {
                perror("e/realloc()");
                goto error;
            }

            /* reallocation successful, update line with new pointer and
             * set line_pos to point to next free byte in line.
             */

            line = new_line;
            line_pos = line_size / 2;
            continue;

#else /* ENABLE_MALLOC */

            /* malloc is not used so we cannot increase buffer, that means
             * error
             */

            fprintf(stderr, "e/line is longer than %ld, aborting\n",
                (long)(line_size - 2));
            errno = ENOBUFS;
            ret = U3_EXIT_FAILURE;
            goto error;

#endif /* ENABLE_MALLOC */

        }

        /* now we have full line in line buffer, do the reverse
        */

        line_len = strlen(line);

        if (line[line_len - 1] == '\n')
        {
            /* last character of the line is a newline, we don't include it
             * in reversing, so decrement line_len by one.
             */

            --line_len;
        }

        if (line_len == 0)
        {
            /* it appears current line contains only new line character,
             * nothing to swap/
             */

            fputs("\n", stdout);
            continue;
        }

        for (i = 0, --line_len; i < line_len; ++i, --line_len)
        {
            /* standard swap with the help of XOR magic
            */

            line[i] ^= line[line_len];
            line[line_len] ^= line[i];
            line[i] ^= line[line_len];
        }

        /* print line to stdout, if newline was in line, it will be printed
         * as last character, since we didn't swap it
         */

        fputs(line, stdout);
        line_pos = 0;
    }

    fflush(stdout);

error:
    if (file_path)
    {
        /* close only if fin is not stdin,  we don't want to close stdin
         * in case standalone is not enabled - we could cause next read
         * of stdin to fail
         */

        fclose(fin);
    }

fopen_error:

#if ENABLE_MALLOC
    free(line);
#endif

    return ret;
}
