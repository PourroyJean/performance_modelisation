//
// Created by Jean Pourroy on 06/12/2016.
//

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <strings.h>
#include "misc.h"


/**
 *
 * @param argc coucou
 * @param argv jannou
 * @return
 */


#define KILO 1024UL
#define MEGA 1048576UL
#define GIGA KILO*MEGA


#define ARRAY_SIZE 1024 * 1024 * 128


int main(int argc, char *argv[]) {
    int i;
    int stride = 1;
    int64_t * array = (int64_t *) malloc (ARRAY_SIZE * sizeof(int64_t));
    int nb_element = ARRAY_SIZE;
    printf ("Size of one element: %lu byte\n", sizeof(int64_t));
    printf ("Stride Cycle per elem\n");
    for (stride = 1; stride <= 10000; stride *= 2) {
        TIC
        ui64 deb = dml_cycles();
        int res = 0;
        int j;
        for (j = 0; j < 4; ++j) {
            for (int i = 0; i < ARRAY_SIZE; i += stride) {
                array[i] = 1;
                res += array[i];
            }
        }
        ui64 fin = dml_cycles();
        TOC
        //printf("stride = %3d : %10.1f ms,  %f per elem, %d nb elem,  %"PRIu64 " cycle per elem\n", stride, TIME_ELAPSED, TIME_ELAPSED / res, res, (fin-deb)/res);
        printf("%6d %14"PRIu64"\n", stride, (fin-deb)/res);
    }
}