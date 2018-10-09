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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#if HAVE_MMAP
#   include <sys/mman.h>
#endif


/* ==========================================================================
               ____                     __   _
              / __/__  __ ____   _____ / /_ (_)____   ____   _____
             / /_ / / / // __ \ / ___// __// // __ \ / __ \ / ___/
            / __// /_/ // / / // /__ / /_ / // /_/ // / / /(__  )
           /_/   \__,_//_/ /_/ \___/ \__//_/ \____//_/ /_//____/

   ========================================================================== */


/* ==========================================================================
    Function reads from 'fd' to 'buf' until end of file occurs or 'buflen'
    bytes have been stored to 'buf'.
   ========================================================================== */


ssize_t read_all
(
    int     fd,     /* file descriptor to read data from */
    void   *buf,    /* location where to store data read from fd */
    size_t  buflen  /* length of the buf */
)
{
    size_t  tr;     /* total bytes read */
    ssize_t r;      /* return value from read() */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    tr = 0;
    for (;;)
    {
        r = read(fd, (unsigned char *)buf + tr, buflen - tr);
        tr += r;

        if (r == -1)
        {
            perror("read()");
            return -1;
        }

        if (r == 0 || tr == buflen)
        {
            /* we got it call or we cannot fit anything else
             */

            return tr;
        }

        if (tr > buflen)
        {
            /* overflow! abandon ship!
             */

            fprintf(stderr, "read_all() buffer overflow!\n");
            exit (1);
        }
    }
}


/* ==========================================================================
    Checks if file f1 and f2 are equal or not

    return
            0       files are NOT equal
            1       files are equal
           -1       error checking for equality
   ========================================================================== */


int file_equal
(
    const char  *f1,   /* path to file 1 */
    const char  *f2    /* path to file 2 */
)
{
    int          fd1;  /* file 1 descriptor */
    int          fd2;  /* file 2 descriptor */
    size_t       pos;  /* current file buffer position */
    ssize_t      fs1;  /* file or buffer 1 size in bytes */
    ssize_t      fs2;  /* file or buffer 2 size in bytes */

#if HAVE_MMAP

    char        *fb1;  /* buffer holding file 1 data */
    char        *fb2;  /* buffer holding fule 2 data */
    struct stat  st;   /* temporary structure to get file info */

#else

    char         fb1[4096];
    char         fb2[4096];

#endif
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    fd1 = open(f1, O_RDONLY);
    if (fd1 < 0)
    {
        perror("open() f1");
        return -1;
    }

    fd2 = open(f2, O_RDONLY);
    if (fd2 < 0)
    {
        perror("open() f2");
        close(fd1);
        return -1;
    }


#if HAVE_MMAP
    /* mmap is available, use this to speed things up
     */

    if (fstat(fd1, &st) == -1)
    {
        perror("fstat() fd1");
        close(fd1);
        close(fd2);
        return -1;
    }

    fs1 = st.st_size;

    if (fstat(fd2, &st) == -1)
    {
        perror("fstat() fd2");
        close(fd1);
        close(fd2);
        return -1;
    }

    fs2 = st.st_size;

    if (fs1 != fs2)
    {
        /* files size are not equal - no way they are equal
         */

        close(fd1);
        close(fd2);
        return 0;
    }

    if (fs1 == 0)
    {
        /* both files are empty and thus equal
         */

        close(fd1);
        close(fd2);
        return 1;
    }

    fb1 = mmap(NULL, fs1, PROT_READ, MAP_SHARED, fd1, 0);
    if (fb1 == MAP_FAILED)
    {
        perror("mmap() fd1");
        close(fd1);
        close(fd2);
        return -1;
    }

    fb2 = mmap(NULL, fs2, PROT_READ, MAP_SHARED, fd2, 0);
    if (fb2 == MAP_FAILED)
    {
        perror("mmap() fd2");
        munmap(fb1, fs1);
        close(fd1);
        close(fd2);
        return -1;
    }

    for (pos = 0; pos != (size_t)fs1; ++pos)
    {
        if (fb1[pos] != fb2[pos])
        {
            /* files are not equal
             */

            munmap(fb1, fs1);
            munmap(fb2, fs2);
            close(fd1);
            close(fd2);
            return 0;
        }
    }

    /* no different characters were found, files equal, at last!
     */

    munmap(fb1, fs1);
    munmap(fb2, fs2);
    close(fd1);
    close(fd2);
    return 1;

#else /* HAVE_MMAP */
    /* now mmap, we have to do it the old fasioned way
     */

    for (;;)
    {
        fs1 = read_all(fd1, fb1, sizeof(fb1));
        fs2 = read_all(fd2, fb2, sizeof(fb2));

        if (fs1 == -1 || fs2 == -1)
        {
            return -1;
        }

        if (fs1 != fs2)
        {
            /* read different number of bytes, one file is
             * probably shorter then the other, marking not
             * equal
             */


            close(fd1);
            close(fd2);
            return 0;
        }

        for (pos = 0; pos != (size_t)fs1; ++pos)
        {
            if (fb1[pos] != fb2[pos])
            {
                /* files are not equal
                 */

                close(fd1);
                close(fd2);
                return 0;
            }
        }

        if (fs1 != sizeof(fb1))
        {
            /* last read_all() read less than full buffer,
             * means we hit end of file, not error until now
             * then files are equal
             */

            close(fd1);
            close(fd2);
            return 1;
        }

        /* file has not been fully read, keep going
         */
    }
#endif
}
