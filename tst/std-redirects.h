/* ==========================================================================
    Licensed under BSD 2clause license See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */

#ifndef U3_TST_PIPE_H
#define U3_TST_PIPE_H 1

#include <sys/types.h>

int stdout_to_file(const char *file);
int stderr_to_file(const char *file);
int stdin_from_file(const char *file);
int restore_stdout(void);
int restore_stderr(void);
int restore_stdin(void);
void rewind_stdout_file(void);
void rewind_stderr_file(void);
void rewind_stdin_file(void);
ssize_t read_stdout_file(void *buf, size_t count);
ssize_t read_stderr_file(void *buf, size_t count);
ssize_t write_stdin_file(const void *buf, size_t count);

#endif /* U3_TST_PIPE_H */
