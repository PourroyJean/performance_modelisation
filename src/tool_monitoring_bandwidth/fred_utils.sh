
#ifndef HPE_TOOLS

#define HPE_TOOLS
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <cpuid.h>
#include <dirent.h>
#include <numa.h>
#include <sys/resource.h>

// CONSTANTS ++++
#define MAXSTRLENGTH 256
#define MAXBUFLENGTH 65536 /* mainly used to parse /proc/iomem */
enum system_types {
  UNKNOWN,
  SOC,
  SDB, /* = IVB */
  HSW, /* = BDW */
  KNL,
  SKL,
  NB_TYPES
};
// CONSTANTS ----

// MACROS ++++
#define GET_ADDR(bus,dev,fn,off) \
        ( (uint64_t)0x80000000 + (bus<<16) + (dev<<11) + (fn<<8) + off )
#define GET_EXT_ADDR(bus,dev,fn,off) \
        ( (uint64_t)mymapping_pciext + (bus<<20) + (dev<<15) + (fn<<12) + off )
#define PCIDATA32(bus,dev,fn,off) \
        ( *(uint32_t * )( GET_EXT_ADDR(bus,dev,fn,off) ) )
#define PCIDATA16(bus,dev,fn,off) \
        ( *(uint16_t * )( GET_EXT_ADDR(bus,dev,fn,off) ) )
#define PCIDATA8(bus,dev,fn,off) \
        ( *(uint8_t * )( GET_EXT_ADDR(bus,dev,fn,off) ) )
#define MSYNC __sync_synchronize()
#define RMSR(msr,buf64) \
        pread(fdmsr,&buf64,8,msr);
#define WMSR(msr,buf64) \
        pwrite(fdmsr,&buf64,8,msr);
#define DEBUG(...) if(debug>0){printf("## HPE TOOLS ## ");printf(__VA_ARGS__);fflush(stdout);};
// MACROS ----

// GLOBAL VARIABLES ++++
int fdmsr=-1;
int bus0=-1,bus1=-1,bus2=-1,bus3=-1,bus4=-1,bus5=-1;
void *mymapping_pciext=0UL;
void *mymapping_mchbar=0UL;
int systype=UNKNOWN;
int debug=0;
int ratedfreq=0;
// GLOBAL VARIABLES ----

// FUNCTIONS ++++
void global_init();
int get_mycpu();
void get_busses();
void map_devmsr();
void map_pci_mmconfig_addr(int fdmem);
void map_mchbar(int fdmem);
double mytimestamp();
uint64_t rdtsc();
uint64_t getbits(uint64_t val, int start, int end);
int getsystype();
// FUNCTIONS ----

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* Returns cpu where caller is running                             */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
int get_mycpu() {
  int mypid,mycpu;
  char *mypath,*mystr,*token;
  char *work,*ptrptr;
  int myfd;
  int i;
  mypath=malloc(MAXSTRLENGTH);
  mystr=malloc(MAXSTRLENGTH);
  mypid=(int) getpid();
  sprintf(mypath,"/proc/%d/stat",mypid);
  myfd=open(mypath,O_RDONLY);
  (void)read(myfd,mystr,MAXSTRLENGTH);
  token=strtok_r(mystr," ",&ptrptr);
  for(i=2;i<40;i++) {
    token=strtok_r(NULL," ",&ptrptr);
  }
  mycpu=atoi(token);
  close(myfd);
  free(mystr);free(mypath);
  return mycpu;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* retrieve PCI bus(ses) 0 to 3 corresponding to my socket, if any */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void get_busses()
{
  uint64_t val64;
  RMSR(0x300,val64);
  bus0= 0xff&(val64);
  bus1= 0xff&(val64>>8);
  bus2= 0xff&(val64>>16);
  bus3= 0xff&(val64>>24);
  bus4= 0xff&(val64>>32);
  bus5= 0xff&(val64>>40);
  DEBUG("My busses 0, 1, 2, 3, 4, 5 are: %x, %x, %x, %x, %x, %x\n",bus0,bus1,bus2,bus3,bus4,bus5);
  return;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* Map MCH BAR                                                     */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void map_mchbar(fdmem)
{
  // should leverage the mapping of the PCIE config space...
  uint32_t val32=0;
  uint64_t val64=0;
  val32= 0xff00ffff & PCIDATA32(0,0,0,0);
  DEBUG("Got signature %x for Bus0 D0 F0 0\n",val32);
  if( /* looking for an Intel device that would correspond to E3-1200v3/4/5 platforms... */
     (val32==0x0c008086) // HSW
   ||(val32==0x0d008086) // HSW
   ||(val32==0x16008086) // BDW
   ||(val32==0x19008086) // SKL
  ){
    val64= ( (uint64_t)PCIDATA32(0,0,0,0x4c) )<<32;
    val64|= PCIDATA32(0,0,0,0x48);
    val64&= 0x7FFFFF8000; /* this selects only bits 15 to 38 as it should */
    DEBUG("Discovered a valid Intel memory controller with MCH BAR = %lx; enforcing systype=SOC\n",val64);
    systype=SOC;
    // just need to map a range that let us access MCHBAR+0x5050 and MCHBAR+0x5054
    mymapping_mchbar=mmap(0,0x10000,PROT_READ|PROT_WRITE,MAP_SHARED,fdmem,val64);
    DEBUG("MCHBAR mapped at %lx\n",mymapping_mchbar);
  } else {
    DEBUG("No MCH BAR for that platform\n");
  }

  return;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* Map PCI MMCONFIG ADDR                                           */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void map_pci_mmconfig_addr(fdmem)
{
  char *mybuf, *mystr;
  int i=0;
  FILE *mystream;
  uint64_t myval=0;

  if ( mystream=fopen("/proc/iomem","r") ) {
    mybuf=malloc(MAXBUFLENGTH);
    if (fread(mybuf,1,MAXBUFLENGTH,mystream) >0) {
      mystr=strtok(mybuf,"\n");
      while (mystr) {
       if (strstr(mystr,"PCI MMCONFIG 0000")) {
         myval=strtol(mystr, NULL, 16);
         break;
       }
       mystr=strtok(NULL,"\n");
       }
    }
    free(mybuf);
    fclose(mystream);
    mymapping_pciext=mmap(0,0x10000000,PROT_READ|PROT_WRITE,MAP_SHARED,fdmem,myval);
    DEBUG("PCI MMCONFIG 0000 address is %lx\n",myval);
    DEBUG("PCI MMCONFIG 0000 mapped at %lx\n",mymapping_pciext);
  }

  return;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* returns the time */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
double mytimestamp()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double ret=1.*(double)tv.tv_sec+(double)tv.tv_usec/1000000.;
  return ret;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* read TSC                                                        */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
uint64_t rdtsc()
{
  uint32_t lo, hi;
  /* We cannot use "=A", since this would use %rax on x86_64 */
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return (uint64_t)hi << 32 | lo;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* extract selected bits from 64-bit counter                       */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
uint64_t getbits(uint64_t val, int start, int end)
{
  uint64_t ret=val;
  assert(end<64);
  assert(start>=0);
  if (end<63) ret=( ( ((uint64_t)1)<<(end+1) ) - (uint64_t)1 )&val;
  return ( ret>>start );
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* find out platform                                               */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
int getsystype()
{
  unsigned int eax,ebx,ecx,edx;
  __get_cpuid(0,&eax,&ebx,&ecx,&edx);
  DEBUG("Processor identification, step #0: %x %x %x %x\n",eax,ebx,ecx,edx);
  if((ebx!=0x756e6547)||(ecx!=0x6c65746e)||(edx!=0x49656e69)){
    // Not GenuineIntel
    DEBUG("Processor is not GenuineIntel -- limited support\n");
    systype=UNKNOWN;
  }else{
    DEBUG("Processor is GenuineIntel\n");
    __get_cpuid(1,&eax,&ebx,&ecx,&edx);
    DEBUG("Processor identification, step #1: %x\n",eax);
    if( (eax&0xf00) != 0x600 ){
      DEBUG("Processor is not Family 6; considering platform as UNKNOWN for now...\n");
      return UNKNOWN;
    }
    eax= (eax&0xf00) + ((eax>>12)&0xf0)+((eax>>4)&0xf);
    DEBUG("Family/Model is %x\n",eax);
    switch(eax){
      case 0x062A :
      case 0x063A :
      case 0x063E :
      case 0x063C :
      case 0x063D :
      case 0x0645 :
      case 0x0646 :
      case 0x0647 :
      case 0x064E :
      case 0x0656 :
      case 0x065E :
      case 0x068E :
      case 0x069E :
        DEBUG("Considering platform as SOC for now...\n");
        return SOC;
      case 0x062D :
      case 0x062F :
        DEBUG("Considering platform as SDB/IVB for now...\n");
        return SDB;
      case 0x063F :
      case 0x064F :
        DEBUG("Considering platform as HSW/BDW for now...\n");
        return HSW;
      case 0x0655 :
        DEBUG("Considering platform as SKL for now...\n");
        return SKL;
      case 0x0657 :
      case 0x0685 :
        DEBUG("Considering platform as KNL for now...\n");
        return KNL;
      default : // too old or too new, or just never tested / unsupported
        DEBUG("Considering platform as UNKNOWN...\n");
        return UNKNOWN;
    }
  }
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* initializes all relevant stuff                                  */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void global_init()
{
    int myprio;
    int fdmem;
    char *msr_file_name;
    uint64_t val64=0;

    if(getenv("HPETOOLS_DEBUG"))debug=1;

    ioperm(0, 0x400, 0x1);
    iopl(0x3);
    ioperm(0x40, 0x4, 0);
    ioperm(0x60, 0x4, 0);

    myprio=getpriority(PRIO_PROCESS,0);
    DEBUG("My current priority is %d; will try to set it to -20 anyway...\n",myprio);
    myprio=setpriority(PRIO_PROCESS,0,-20);
    myprio=getpriority(PRIO_PROCESS,0);
    DEBUG("My new priority is %d.\n",myprio);

    msr_file_name=malloc(MAXSTRLENGTH);
    sprintf(msr_file_name, "/dev/cpu/%d/msr", get_mycpu());
    fdmsr = open(msr_file_name, O_RDWR|O_SYNC);
    free(msr_file_name);

    RMSR(0xCE,val64);
    ratedfreq = 0xff&(val64>>8);
    DEBUG("Rated freq=%d x 100MHz (MSR 0xCE = %x)\n",ratedfreq,val64);

    get_busses();

    systype=getsystype();
    DEBUG("Note: initial platform detection may be overridden by individual tools.\n");

    fdmem=open("/dev/mem", O_RDWR|O_SYNC);
    map_pci_mmconfig_addr(fdmem);
    map_mchbar(fdmem);
    close(fdmem);
}

#endif

