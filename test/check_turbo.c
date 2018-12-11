
#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <sched.h>
#include <unistd.h>

#define NITE 20
#define NLOOPS 400000000UL

void aloop(unsigned int n);
uint64_t rdtsc();
double mygettime();

main(int argc,char **argv)
{
  int i, mycpu=0;
  cpu_set_t mycpumask;
  unsigned int freq;
  unsigned int res;
  uint64_t rtcstart,rtcend,time;
  double tstart,tend;
  float ipc;

  printf("+ \n");

  /* Retrieve which CPU to bind to */
  if(argv[1]) mycpu=atoi(argv[1]);
  CPU_ZERO(&mycpumask);
  if(mycpu>=0)
  {
    CPU_SET(mycpu,&mycpumask);
    sched_setaffinity(0,sizeof(cpu_set_t),&mycpumask);
  };
  /* double-check */
  sched_getaffinity(0,sizeof(cpu_set_t),&mycpumask);
  for (i=0; i < sysconf(_SC_NPROCESSORS_CONF); i++)
  {
    if(CPU_ISSET(i,&mycpumask)) printf("+ Running on CPU #%d\n",i);
  };

  /* Compute native frequency */
  tstart=mygettime();
  rtcstart=rdtsc();
  usleep(1000000);
  rtcend=rdtsc();
  tend=mygettime();
  time=rtcend-rtcstart;
  freq=time/(1000000*(tend-tstart));
  printf("+ Estimating 'native' frequency to    %u MHz\n",freq);

  /* Make the CPU execute a given number of basic instructions,
     expected to be executed in 8 cycles */
  rtcstart=rdtsc();
  for(i=0;i<NITE;i++){
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
  }
  rtcend=rdtsc();
  time=rtcend-rtcstart;
  ipc=(double)NITE*(double)NLOOPS/(double)time;
  printf("+ Found my IPC to be %4.2f (would expect 1.0)\n",ipc);
  printf("+ Estimating 'effective' frequency to %u MHz\n",(unsigned int)((double)freq*ipc));
  if(ipc>1.0001)
    printf("+ Turbo ON !\n+ \n");
  else
    printf("+ Turbo mode does not seem to be accessible to me...\n+ \n");
  
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

