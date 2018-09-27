#include "misc.h"
#include <inttypes.h>
#include <stdio.h>


#include <string>

using namespace std;
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



//Sources:
//https://en.wikipedia.org/wiki/Mebibyte
//https://stackoverflow.com/questions/3898840/converting-a-number-of-bytes-into-a-file-size-in-c
string convert_size(size_t  size)
{
    static const char *SIZES[] = { "Byte", "KiB", "MiB", "GiB", "TiB" };
    size_t div = 0;
    size_t rem = 0;

    while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES)) {
        rem = (size % 1024);
        div++;
        size /= 1024;
    }

//    printf("%.1f %s\n", (float)size + (float)rem / 1024.0, SIZES[div]);
    char str [100];
    sprintf(str,"%.1f %s", (float)size + (float)rem / 1024.0, SIZES[div]);
    return string(str);
}