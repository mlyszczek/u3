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


#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/* ==========================================================================
                                   _         __     __
              _   __ ____ _ _____ (_)____ _ / /_   / /___   _____
             | | / // __ `// ___// // __ `// __ \ / // _ \ / ___/
             | |/ // /_/ // /   / // /_/ // /_/ // //  __/(__  )
             |___/ \__,_//_/   /_/ \__,_//_.___//_/ \___//____/

   ========================================================================== */


static FILE *stdout_save;
static FILE *stdout_ro;
static int   fd_stdin = -1;
static int   fd_stdout = -1;
static int   fd_stderr = -1;
static int   fd_stdin_file = -1;
static int   fd_stdout_file = -1;
static int   fd_stderr_file = -1;


/* ==========================================================================
               ____                     __   _
              / __/__  __ ____   _____ / /_ (_)____   ____   _____
             / /_ / / / // __ \ / ___// __// // __ \ / __ \ / ___/
            / __// /_/ // / / // /__ / /_ / // /_/ // / / /(__  )
           /_/   \__,_//_/ /_/ \___/ \__//_/ \____//_/ /_//____/

   ==========================================================================
                                   _                __
                     ____   _____ (_)_   __ ____ _ / /_ ___
                    / __ \ / ___// /| | / // __ `// __// _ \
                   / /_/ // /   / / | |/ // /_/ // /_ /  __/
                  / .___//_/   /_/  |___/ \__,_/ \__/ \___/
                 /_/
   ========================================================================== */


/* ==========================================================================
    Redirects 'fd' to file 'fd_file'
   ========================================================================== */


static int fd_to_file
(
    int   fd_file,  /* where to store newly created file descriptor */
    int   fd,       /* what descriptor should be redirected to file */
    int  *sfd       /* location where to save fd for future restore */
)
{
    /* save 'fd' file descriptor so we can later restore it
     */

    *sfd = dup(fd);
    if (*sfd < 0)
    {
        perror("dup()");
        return -1;
    }

    /* redirect file to provided fd - writing to fd * will actually
     * write to fd_file
     */

    if (dup2(fd_file, fd) < 0)
    {
        perror("dup2()");
        dup2(fd, *sfd);
        *sfd = -1;
        return -1;
    }

    return 0;
}


/* ==========================================================================
    Restores previous behaviour for 'fd'.
   ========================================================================== */


static int restore_fd
(
    int fd,   /* file descriptor number to restore */
    int sfd   /* previously saved 'fd' */
)
{
    if (sfd == -1)
    {
        errno = EINVAL;
        return -1;
    }

    if (dup2(sfd, fd) < 0)
    {
        perror("dup2()");
        return -1;
    }

    return 0;
}


/* ==========================================================================
                                        __     __ _
                         ____   __  __ / /_   / /(_)_____
                        / __ \ / / / // __ \ / // // ___/
                       / /_/ // /_/ // /_/ // // // /__
                      / .___/ \__,_//_.___//_//_/ \___/
                     /_/
   ========================================================================== */


/* ==========================================================================
    Redirects stdout to file
   ========================================================================== */


int stdout_to_file
(
    const char *file  /* file where stdout should be redirected */
)
{
    /* flush stdout in case there is some cached data in there - it is
     * possible fflush occurs after our redirect and thus data that
     * should have been put into real stdout, will be put into our file,
     * not an ideal situation
     */

    fflush(stdout);

    fd_stdout_file = open(file, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd_stdout_file < 0)
    {
        perror("open()");
        return -1;
    }

    if (fd_to_file(fd_stdout_file, STDOUT_FILENO, &fd_stdout) != 0)
    {
        perror("fd_to_file()");
        close(fd_stdout_file);
        fd_stdout_file = -1;
        return -1;
    }

    return 0;
}


/* ==========================================================================
    Sabotages stdout FILE pointer so writing to it will result in error
   ========================================================================== */


int stdout_sabotage
(
    const char *file
)
{
    /* first we need to create file or else we won't be able to
     * open it in read only mode
     */

    stdout_ro = fopen(file, "w");
    fclose(stdout_ro);

    /* now we open file in read only mode, so writint to it will
     * result in error
     */

    stdout_ro = fopen(file, "r");

    /* now save stdout so we can restore it later and plant our
     * bad FILE.
     */

    stdout_save = stdout;
    stdout = stdout_ro;

    return 0;
}


/* ==========================================================================
    Recover stdout from sabotage
   ========================================================================== */


void stdout_recover(void)
{
    stdout = stdout_save;
    fclose(stdout_ro);
}


/* ==========================================================================
    Redirects stderr to file
   ========================================================================== */


int stderr_to_file
(
    const char *file  /* file where stderr should be redirected */
)
{
    fd_stderr_file = open(file, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd_stderr_file < 0)
    {
        perror("open()");
        return -1;
    }

    if (fd_to_file(fd_stderr_file, STDERR_FILENO, &fd_stderr) != 0)
    {
        perror("fd_to_file()");
        close(fd_stderr_file);
        fd_stderr_file = -1;
        return -1;
    }

    return 0;
}


/* ==========================================================================
    Redirects stdin to file, so reading from stdin is actual read from file
   ========================================================================== */


int stdin_from_file
(
    const char *file  /* file where stdin should be redirected */
)
{

    fd_stdin_file = open(file, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd_stdin_file < 0)
    {
        perror("open()");
        return -1;
    }

    if (fd_to_file(fd_stdin_file, STDIN_FILENO, &fd_stdin) != 0)
    {
        perror("fd_to_file()");
        close(fd_stdin_file);
        fd_stdin_file = -1;
        return -1;
    }

    return 0;
}


/* ==========================================================================
    Restores stdout to act normally, also closes file.
   ========================================================================== */


int restore_stdout(void)
{
    if (restore_fd(STDOUT_FILENO, fd_stdout) != 0)
    {
        perror("restore_fd()");
    }

    close(fd_stdout_file);
    close(fd_stdout);
    fd_stdout_file = -1;
    fd_stdout = -1;
    return 0;
}


/* ==========================================================================
    Restores stderr to act normally, also closes file.
   ========================================================================== */


int restore_stderr(void)
{
    if (restore_fd(STDERR_FILENO, fd_stderr) != 0)
    {
        perror("restore_fd()");
    }

    close(fd_stderr_file);
    close(fd_stderr);
    fd_stderr_file = -1;
    fd_stderr = -1;
    return 0;
}


/* ==========================================================================
    Restores stdin to act normally, also closes file.
   ========================================================================== */


int restore_stdin(void)
{
    if (restore_fd(STDIN_FILENO, fd_stdin) != 0)
    {
        perror("restore_fd()");
    }

    close(fd_stdin_file);
    close(fd_stdin);
    fd_stdin_file = -1;
    fd_stdin = -1;
    return 0;
}


/* ==========================================================================
    Rewinds stdout file, so read() can start from the beginning
   ========================================================================== */


void rewind_stdout_file(void)
{
    lseek(fd_stdout_file, 0, SEEK_SET);
}


/* ==========================================================================
    Rewinds stderr file, so read() can start from the beginning
   ========================================================================== */


void rewind_stderr_file(void)
{
    lseek(fd_stderr_file, 0, SEEK_SET);
}


/* ==========================================================================
    Rewinds stdin file, so read() can start from the begining
   ========================================================================== */


void rewind_stdin_file(void)
{
    /* some systems (like freebsd) use read pointer that
     * is inside stdin, when others (like linux) will be
     * using read pointer from file descriptor stdin points
     * to. To make both type of system happy we also seek
     * stdin to the beginning
     */

    fseek(stdin, 0, SEEK_SET);
    lseek(fd_stdin_file, 0, SEEK_SET);
}


/* ==========================================================================
    Reads from file what was  sent  to  stdout  -  should  be  called  after
    fd_to_file() function
   ========================================================================== */


ssize_t read_stdout_file
(
    void   *buf,
    size_t  count
)
{
    return read(fd_stdout_file, buf, count);
}


/* ==========================================================================
    Reads from file what was  sent  to  stderr  -  should  be  called  after
    fd_to_file() function
   ========================================================================== */


ssize_t read_stderr_file
(
    void   *buf,
    size_t  count
)
{
    return read(fd_stderr_file, buf, count);
}


/* ==========================================================================
    Writes data to stdint file, should be called after fd_to_file() function
   ========================================================================== */


ssize_t write_stdin_file
(
    void         *buf,
    const size_t  count
)
{
    return write(fd_stdin_file, buf, count);
}
