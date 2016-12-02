#include "misc.h"
#include <inttypes.h>
#include <stdio.h>


void print_times(char *s) {
    int total_usecs;
    total_usecs = (end_time.tv_sec - start_time.tv_sec) * 1000000 +
                  (end_time.tv_usec - start_time.tv_usec);
    printf("%s%.2f\n", s, ((float) total_usecs) / 1000.0);
}


/**
 * This assembly function return the number of the current cycle
 * @return  cycle number
 */
ui64 dml_cycles() {
    ui64 eax, edx;
    __asm__ volatile ("rdtsc":"=a" (eax), "=d" (edx));
    return ((edx << 32) | eax);
}
