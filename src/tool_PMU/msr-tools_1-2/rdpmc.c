#include <stdio.h>
#include <string.h>

// rdpmc_instructions uses a "fixed-function" performance counter to return the count of retired instructions on
//       the current core in the low-order 48 bits of an unsigned 64-bit integer.
unsigned long rdpmc_instructions()
{
    unsigned a, d, c;

    c = (1<<30);
    __asm__ volatile("rdpmc" : "=a" (a), "=d" (d) : "c" (c));

    return ((unsigned long)a) | (((unsigned long)d) << 32);;
}

// rdpmc_actual_cycles uses a "fixed-function" performance counter to return the count of actual CPU core cycles
//       executed by the current core.  Core cycles are not accumulated while the processor is in the "HALT" state,
//       which is used when the operating system has no task(s) to run on a processor core.
unsigned long rdpmc_actual_cycles()
{
    unsigned a, d, c;

    c = (1<<30)+1;
    __asm__ volatile("rdpmc" : "=a" (a), "=d" (d) : "c" (c));

    return ((unsigned long)a) | (((unsigned long)d) << 32);;
}

// rdpmc_reference_cycles uses a "fixed-function" performance counter to return the count of "reference" (or "nominal")
//       CPU core cycles executed by the current core.  This counts at the same rate as the TSC, but does not count
//       when the core is in the "HALT" state.  If a timed section of code shows a larger change in TSC than in
//       rdpmc_reference_cycles, the processor probably spent some time in a HALT state.
unsigned long rdpmc_reference_cycles()
{
    unsigned a, d, c;

    c = (1<<30)+2;
    __asm__ volatile("rdpmc" : "=a" (a), "=d" (d) : "c" (c));

    return ((unsigned long)a) | (((unsigned long)d) << 32);;
}

unsigned long rdpmc(unsigned n)
{
    unsigned a, d, c;

    c = n;
    __asm__ volatile("rdpmc" : "=a" (a), "=d" (d) : "c" (c));

    return ((unsigned long)a) | (((unsigned long)d) << 32);;
}

#define N 4

int multiplication (){

    int mat1[N][N] = { {1, 1, 1, 1},
                       {2, 2, 2, 2},
                       {3, 3, 3, 3},
                       {4, 4, 4, 4}};

    int mat2[N][N] = { {1, 1, 1, 1},
                       {2, 2, 2, 2},
                       {3, 3, 3, 3},
                       {4, 4, 4, 4}};

    int res[N][N];

    int i, j, k;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            res[i][j] = 0;
        }
    }

    int z;
    for (z  = 0; z < 1000000; ++z) {
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                for (k = 0; k < N; k++)
                    res[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                for (k = 0; k < N; k++)
                    mat1[i][j] += res[i][k] * mat2[k][j];
            }
        }
    }

    int r = 0;
    for (j = 0; j < N; j++)
    {
        for (k = 0; k < N; k++)
            r+=res[j][k];
    }
    return r;
}

int main (){
    printf ("Fixed-function 0 - rdpmc_instructions      : %lu \n", rdpmc_instructions());
    printf ("Fixed-function 1 - rdpmc_actual_cycles     : %lu \n", rdpmc_actual_cycles());
    printf ("Fixed-function 2 - rdpmc_reference_cycles  : %lu \n", rdpmc_reference_cycles());
    printf ("PMC 1                                      : %lu \n", rdpmc(0));
    printf ("PMC 2                                      : %lu \n", rdpmc(1));
    printf ("PMC 3                                      : %lu \n", rdpmc(2));
    printf ("PMC 4                                      : %lu \n", rdpmc(3));
    printf ("Register 0x300                             : %lu \n", rdpmc(0x300));
//
//    char command[50];
//    strcpy( command, "./assSingle" );
//    system(command);
//
//
//
//    printf ("Mult %d \n", multiplication());
//    printf ("Instruction     %ld \n", rdpmc_instructions());
//    printf ("Ref Cycle       %ld \n", rdpmc_reference_cycles);
//    printf ("Actual Cycles   %lu \n", rdpmc_actual_cycles);
//    printf ("Flops Cycles  0 %lu \n", rdpmc(0));
//    printf ("Flops Cycles  1 %lu \n", rdpmc(1));
//    printf ("Flops Cycles  2 %lu \n", rdpmc(2));
//    printf ("Flops Cycles  3 %lu \n", rdpmc(3));
}
