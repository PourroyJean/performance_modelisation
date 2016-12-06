//#include"includeStudent.h"
#include <inttypes.h>
#include <stdio.h>
#include "misc.h"


/**
 *  This function is used to calculate the number of time the loop has to be repeated to last 1 second
 * @return number of iteration
 */
int find_second() {
    int k = 100000000;
    int res, i, a, b, c, d, e = 1;
    TIME_ELAPSED = 0;
    while (TIME_ELAPSED < 1000000) {
        k += 100000000;
        TIC
        for (i = 0; i < k; ++i) {
            a = b++ + c++;
            d = a + e++;
            res += d;
        }
        TOC
    }
    return k;
}

/**
 *  This tool is used to calculate the processor frequency
 * @param argc not needed
 * @param argv not needed
 * @return no return
 */
int main(int argc, char *argv[]) {
    int k = find_second();
    TIC
    ui64 deb = dml_cycles();
    int res, i, a, b, c, d, e = 1;
    for (i = 0; i < k; ++i) {
        a = b++ + c++;
        d = a + e++;
        res += d;
    }
    ui64 fin = dml_cycles();
    (void) res; //be sure result is used to counter compiler optimization
    TOC
    //print_times("time elapsed: ");
    printf("Frequency: %f\n", (fin - deb) / TIME_ELAPSED / 1000);
    return 0;
}