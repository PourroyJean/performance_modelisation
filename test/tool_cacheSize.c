//
// Created by Jean Pourroy on 06/12/2016.
//

//
// Created by Jean Pourroy on 06/12/2016.
//

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <strings.h>
#include "misc.h"

#define KILO 1024UL
#define MEGA 1048576UL
#define GIGA KILO*MEGA


#define STEP 1024 * 1024  * 128

#define KILO 1024UL
#define MEGA 1048576UL
#define GIGA KILO*MEGA

int main(int argc, char *argv[]) {
    int i;
    int stride = 8;

    //Allocation de 1 GB
    int64_t *array = (int64_t *) malloc(1 * GIGA);
    int nb_elem = GIGA / sizeof(int64_t) - 1;

    printf("Size of one element: %lu byte\n", sizeof(int64_t));

    printf("Array size  Cycle per element\n");

    int steps = 64 * 1024 * 1024; // Arbitrary number of steps

    for (int array_size = 16; array_size < 1 * GIGA / sizeof (int64_t); array_size = array_size * 2) {
        int res = 0;
        ui64 deb = dml_cycles();
        for (int i = 0; i < STEP; i++) {
            array[(i * stride) % (array_size - 1)] = 1; // (x & lengthMod) is equal to (x % arr.Leng
            res++;
        }
        ui64 fin = dml_cycles();
        printf("%10lu %18"PRIu64"\n", array_size * sizeof(int64_t), (fin - deb) / res);
    }
    free (array);

}

