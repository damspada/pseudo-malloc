#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H

#include <stdio.h>

/* 
 * Conditional printing macros
 * When DEBUG_PRINT is defined, prints are enabled
 * When DEBUG_PRINT is not defined, prints are disabled (no-op)
 */

#ifdef DEBUG_PRINT
    #define DEBUG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
    #define DEBUG_FPRINTF(stream, fmt, ...) fprintf(stream, fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINTF(fmt, ...) do {} while(0)
    #define DEBUG_FPRINTF(stream, fmt, ...) do {} while(0)
#endif

#endif /* DEBUG_PRINT_H */
