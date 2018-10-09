/* ==========================================================================
    Licensed under BSD 2clause license See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */

#ifndef U3_FPOS_H
#define U3_FPOS_H 1

ssize_t read_all(int fd, void *buf, size_t buflen);
int file_equal(const char *f1, const char *f2);

#endif /* U3_FPOS_H */
