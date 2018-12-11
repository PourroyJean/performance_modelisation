#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <sched.h>
#include <unistd.h>
#include <cstdlib>

//This code was developed by Frederic Ciesielski

#
#define NITE 100
#define NLOOPS 400000UL
#
void aloop(unsigned int n);
uint64_t rdtsc();
double mygettime();
#
int main(int argc,char **argv)
{
    int i, mycpu=0;
    unsigned int freq,time;
    unsigned int res;
    uint64_t rtcstart,rtcend;
    double tstart,tend;
    float ipc;

    /* Compute native frequency */
    tstart=mygettime();
    rtcstart=rdtsc();
    usleep(1000000);
    rtcend=rdtsc();
    tend=mygettime();
    time=rtcend-rtcstart;
    freq=time/(1000000*(tend-tstart));
//    printf("+ Estimating 'native' frequency to    %u MHz\n",freq);

    /* Make the CPU execute a given number of basic instructions,
       expected to be executed in 8 cycles */
    for (i=0; i<NITE; i++){
        rtcstart=rdtsc();
        __asm__ ("aloop: "
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       jnz    aloop" : : "a" (NLOOPS)
        );
        rtcend=rdtsc();
    }
    time=rtcend-rtcstart;
    ipc=(double)NLOOPS/(double)time;
//    printf("+ Found my IPC to be %4.2f (would expect 1.0)\n",ipc);
//    printf("+ Estimating 'effective' frequency to %u MHz\n",(unsigned int)((double)freq*ipc));
    if(ipc>1.0001)
        printf("YES");
    else
        printf("NO");

    return 0;
}

uint64_t rdtsc()
{
    uint32_t lo, hi;
    /* We cannot use "=A", since this would use %rax on x86_64 */
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t)hi << 32 | lo;
}

double mygettime()
{
    struct timeval tp;
    struct timezone tzp;
    int i;
    i = gettimeofday(&tp,&tzp);
    return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}
