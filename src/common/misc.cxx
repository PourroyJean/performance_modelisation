#include "misc.h"
#include <inttypes.h>
#include <stdio.h>




float TIME_ELAPSED;
struct timeval start_time, end_time;



void print_times(char *s) {
    int total_usecs;
    printf("%s%.2f\n", s, ((float) TIME_ELAPSED) / 1000.0);
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
