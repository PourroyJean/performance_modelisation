#include <stdio.h>
#include <stdint.h>


main() {
    int i = 0;
    int *pi = 0, *pf = 0;

    pi = &i;
    printf("From C,        &i=%lx  pi=%d\n", pi, *pi);
    __asm__ volatile (
//    "sub $0x4,%%rax;" : "=a" (pf) : "a" (pi)
//    "VBROADCASTSD %%eax, %%ymm1;"
    ".section .data  \n"
    "1: .asciz \"Hello\" \n"
    ".section .text"
    "vaddsd %%xmm0, %%xmm0,%%xmm1;"
    : "=a" (pf) : "a" (pi)
    );

    printf("After C/ASM/C, &i=%lx  pf=%d\n", pf + 1, *(pf +1)    );
}
