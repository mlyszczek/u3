/* ==========================================================================
    Licensed under BSD 2clause license See LICENSE file for more information
    Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
   ========================================================================== */

#ifndef U3_U3_H
#define U3_U3_H 1

#define U3_VERSION "v1.0.0"

#if CONFIG_STANDALONE
#   define U3_EXIT_FAILURE 1
#else /* CONFIG_STANDALONE */
#   define U3_EXIT_FAILURE -1
#endif /* CONFIG_STANDALONE */

#endif /* U3_U3_H */
