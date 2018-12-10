#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>


//JEAN
#include <iostream>
using namespace std;


double
dml_micros()
{
        static struct timezone tz;
        static struct timeval  tv;
        gettimeofday(&tv,&tz);
        return((tv.tv_sec*1000000.0)+tv.tv_usec);
}

//#define __AFFINITY

#ifdef _DML_INT64
#ifdef __linux__
#define THETYPE unsigned long long
#else
#define THETYPE uint64_t
#endif
#else
#define THETYPE double
#endif

#ifdef __linux__
#define THEINT  unsigned long long
#else
#define THEINT  uint64_t
#endif

/*
	to run the program : execute
        ./test1 EXPERIMENT parms ....

	EXPERIMENT is a string that will be displayed on each line of measures to
		help remember the name of the experiment
        EXPERIMENT is mandatory and need to be argv[1]

        the parms are :
	enter the parms "parm_name=value"  in any order they all have default values

        MODE :
           MODE_READ    : classical algorythm , with UNROLL level from 1 to 8
           MODE_READSPE : unrolling from 2 to 16 with sum reduction
           MODE_READIND : unrolling from 2 to 16 with sum reduction with indexing
        UNROLL can be 1 2 4 8 16 but not for all MODEs look above
        MAT_SIZE is in megabytes the maximum length of THETYPEs used to make the test
        MIN_STRIDE is the smallest stride used
        MAX_STRIDE is the greatest stride used
            if  (MIN_STRIDE==MAX_STRIDE) you have a constant stride
        MIN_LOG10 is the log10 value of the smallest length of the buffer used "THETYPEs"
        MAX_LOG10 is the log10 value of the greatest length of the buffer used "THETYPEs"
	STEP_LOG10 is the step used to increase the length of the buffer
                   using very small values will increase  significantly the process
                   combined with (MIN_STRIDE==MAX_STRIDE) you can profile a
                   caracteristic behaviour;
        MAT_OFFSET : number of THETYPEs you want to offset , we are normally aligned on
                   megabyte boundaries then also on cache line boundary
        NUM_INDEX    number of indexes used with MODE_READIND

others: you should normally not use

        NTMAX_MEASURES is the number of  time we repeat the measure
		we can display the BEST teh AVERAGE of the WORST MEASURE or ALL
		BEST is a good indicator of peak but average will give better indication of reality
        MAX_OPS is the maximum number of operation per loop, the reason is to limit
           CPU for large buffer when there is no need to loop too much because all
           accesses are out of cache
        MAKE_UNAME if set to 0 we dont do uname command to help Aries or others tools.

        CPU_AFF is the CPU where we will run and measure
	MEM_AFF is the

remarks:

       The unrolled version expect multiply of the unroll factor , since this
       is a performance code we do not care loosing the last elements of the normal loop.

*/

#define DISP_BEST    	1
#define DISP_AVE     	2
#define DISP_TWO     	3
#define DISP_ALL     	4
#define DISP_NS      	0
#define DISP_GB      	1
#define DISP_CY      	2

int     mypid;
int     moderw;
int     GHZ=0;
int     VERBOSE=1;
int     DISP=DISP_AVE;           	/* mode de DISPLAY                                                */
int     NSGBCY=DISP_GB;	 	 	/* mode in ns or GB						   */
int     CACHE_LINE=64;          	/* length of cache line      					   */
THEINT  CPU_AFF=0;               	/* CPU ou le processeur doit tourner                              */
THEINT  MEM_AFF=0;               	/* MEM ou le processeur doit tourner                              */
THEINT  MAT_SIZE=100;             	/* size of matrice in Meg 					   */
THEINT  MAX_OPS =1;              	/* max number of operations per function call                     */
int     MIN_STRIDE=sizeof(THETYPE); 	/* smallest stride size used ( number of THETYPEs )                */
int     MAX_STRIDE=1024*8;       	/* greatest stride size used ( number of THETYPEs )                */
int     MAX_MEASURES=2;          	/* number of time the measure is taken we print only the best     */
THEINT  MAT_OFFSET=0;            	/* THETYPE offset added to mat ( which is forced to Meg boundary ) */
int     NUM_INDEX=1;             	/* number of indexes used with MODE_READIND                       */
int	MAKE_UNAME=1;            	/* flag to not do uname when using special tools like aries 0=NO  */
double   MIN_LOG10=3.0;          	/* log10 of minimal  vector size used				   */
double   MAX_LOG10=8.0;          	/* log10 of maximum  vector size used				   */
double   STEP_LOG10=0.1;         	/* log10 of STEP for vector size used				   */
#define MIN_OPS_PER_SCAN 128     	/* we do not measure if less MIN_OPS_PER SCAN operations per loop */
#define BIG_VAL 999999999.0
#define LOG10 2.30258509299404568401


#define MODE_READ    	1
#define MODE_READSPE 	2
#define MODE_READIND 	3
#define MODE_WRITE      4
#define MODE_WRITESPE   5
#define MODE_WRITEIND   6

#define MODER		1
#define MODEW		2

#define UNROLL1   	1
#define UNROLL2   	2
#define UNROLL4   	4
#define UNROLL8   	8

int	MODE=MODE_READ;
int     UNROLL=UNROLL4;

double  total_loops=0.0;
double  start_time=0.0;
double  end_time=0.0;

char message_info[1024];

THETYPE towrite;
THETYPE *mat,*mat_for_free;

void
help()
{
	printf("./test1 NAME_MACHINE parms\n");
	printf("options are MAT_SIZE MIN_STRIDE MAX_STRIDE MIN_LOG10 MAX_LOG10 STEP_LOG10\n");
	printf("            MAX_OPS MAT_OFFSET MAX_MEASURES NUM_INDEX MAKE_UNAME         \n");
	printf("            CPU_AFF[0-X] MEM_AFF[0-X] MODE                               \n");
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
char* strupr( char *_src ) {
  char *c = _src;
  while ( *_src++ )
    toupper( *_src );
  return c;
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//  ALGO for READ
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_read_unroll1(THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps;
	THETYPE sum=0;
//	printf ("\n_sum_read_unroll_ step           %d \n", step);
//    printf ("_sum_read_unroll_ xstep          %d \n", step);
//    printf ("_sum_read_unroll_ ops_per_scan   %d \n", ops_per_scan);
//    printf ("_sum_read_unroll_ repeat         %d \n", repeat);
	int rep;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		for(steps=0;steps<ops_per_scan;steps++){
			sum+=*p1;p1+=step;
		}
	}
	return(sum);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_read_unroll2(THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
    THEINT steps,xstep=step*2;
	int rep;
	THETYPE sum=0;
	ops_per_scan/=2;
//    printf ("\n_sum_read_unroll2_ step           %d \n", step);
//    printf ("_sum_read_unroll2_ xstep          %d \n", xstep);
//    printf ("_sum_read_unroll2_ ops_per_scan   %d \n", ops_per_scan);
//    printf ("_sum_read_unroll2_ repeat   %d \n", repeat);


    for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		THETYPE *p2=p1+step;
		for(steps=0;steps<ops_per_scan;steps++){
			sum+=*p1;p1+=xstep;
			sum+=*p2;p2+=xstep;
		}
	}
//    exit (-1);
    return(sum);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_read_unroll4(THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*4;
	THETYPE sum=0;
	int rep;
	ops_per_scan/=4;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		THETYPE *p2=p1+step;
		THETYPE *p3=p2+step;
		THETYPE *p4=p3+step;
		for(steps=0;steps<ops_per_scan;steps++){
			sum+=*p1;p1+=xstep;
			sum+=*p2;p2+=xstep;
			sum+=*p3;p3+=xstep;
			sum+=*p4;p4+=xstep;
		}
	}
	return(sum);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_read_unroll8(THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*8;
	THETYPE sum=0;
	int rep;
	ops_per_scan/=8;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		THETYPE *p2=p1+step;
		THETYPE *p3=p2+step;
		THETYPE *p4=p3+step;
		THETYPE *p5=p4+step;
		THETYPE *p6=p5+step;
		THETYPE *p7=p6+step;
		THETYPE *p8=p7+step;
		for(steps=0;steps<ops_per_scan;steps++){
			sum+=*p1;p1+=xstep;
			sum+=*p2;p2+=xstep;
			sum+=*p3;p3+=xstep;
			sum+=*p4;p4+=xstep;
			sum+=*p5;p5+=xstep;
			sum+=*p6;p6+=xstep;
			sum+=*p7;p7+=xstep;
			sum+=*p8;p8+=xstep;
		}
	}
	return(sum);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_readspe_unroll2 (THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*2;
	THETYPE sum1=0,sum2=0;
	int rep;
	ops_per_scan/=2;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		THETYPE *p2=p1+step;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1+=*p1;p1+=xstep;
			sum2+=*p2;p2+=xstep;
		}
	}
	sum1+=sum2;
	return(sum1);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_readspe_unroll4 (THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*4;
	THETYPE sum1=0,sum2=0,sum3=0,sum4=0;
	int rep;
	ops_per_scan/=4;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		THETYPE *p2=p1+step;
		THETYPE *p3=p2+step;
		THETYPE *p4=p3+step;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1+=*p1;p1+=xstep;
			sum2+=*p2;p2+=xstep;
			sum3+=*p3;p3+=xstep;
			sum4+=*p4;p4+=xstep;
		}
	}
	sum1+=sum2;
	sum3+=sum4;
	sum1+=sum3;
	return(sum1);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_readspe_unroll8 (THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*8;
	THETYPE sum1=0,sum2=0,sum3=0,sum4=0,sum5=0,sum6=0,sum7=0,sum8=0;
	int rep;
	ops_per_scan/=8;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		THETYPE *p2=p1+step;
		THETYPE *p3=p2+step;
		THETYPE *p4=p3+step;
		THETYPE *p5=p4+step;
		THETYPE *p6=p5+step;
		THETYPE *p7=p6+step;
		THETYPE *p8=p7+step;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1+=*p1;p1+=xstep;
			sum2+=*p2;p2+=xstep;
			sum3+=*p3;p3+=xstep;
			sum4+=*p4;p4+=xstep;
			sum5+=*p5;p5+=xstep;
			sum6+=*p6;p6+=xstep;
			sum7+=*p7;p7+=xstep;
			sum8+=*p8;p8+=xstep;
		}
	}
	sum1+=sum2;
	sum3+=sum4;
	sum5+=sum6;
	sum7+=sum8;
	sum1+=sum3;
	sum5+=sum7;
	sum1+=sum5;
	return(sum1);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_readind_unroll2 (THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*2;
	THETYPE sum1=0,sum2=0;
	int rep;
	ops_per_scan/=2;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1+=p1[0];
			sum2+=p1[step];
			      p1+=xstep;
		}
	}
	sum1+=sum2;
	return(sum1);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_readind_unroll4 (THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*4;
	int rep;
	int step2=step +step;
	int step3=step2+step;
	int step4=step3+step;
	THETYPE sum1=0,sum2=0,sum3=0,sum4=0;
	ops_per_scan/=4;
	for(rep=0;rep<repeat;rep++){
	    if(NUM_INDEX==1){
		THETYPE *p1=m;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1+=p1[0    ];
			sum2+=p1[step ];
			sum3+=p1[step2];
			sum4+=p1[step3];
			      p1+=xstep;
		}
	    }
	    if(NUM_INDEX==2){
		THETYPE *p1=m;
		THETYPE *p2=m+step2;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1+=p1[0   ];
			sum2+=p1[step];
			      p2+=xstep;
			sum3+=p2[0   ];
			sum4+=p2[step];
			      p2+=xstep;
		}
	    }
	}
	sum1+=sum2;
	sum3+=sum4;
	sum1+=sum3;
	return(sum1);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_readind_unroll8 (THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*8;
	THETYPE sum1=0,sum2=0,sum3=0,sum4=0,sum5=0,sum6=0,sum7=0,sum8=0;
	int rep;
	int step2=step +step;
	int step3=step2+step;
	int step4=step3+step;
	int step5=step4+step;
	int step6=step5+step;
	int step7=step6+step;
	ops_per_scan/=8;
	for(rep=0;rep<repeat;rep++){
	    if(NUM_INDEX==1){
		THETYPE *p1=m;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1 +=p1[0    ];
			sum2 +=p1[step ];
			sum3 +=p1[step2];
			sum4 +=p1[step3];
			sum5 +=p1[step4];
			sum6 +=p1[step5];
			sum7 +=p1[step6];
			sum8 +=p1[step7];
			       p1+=xstep;
		}
	    }
	    if(NUM_INDEX==2){
		THETYPE *p1=m;
		THETYPE *p2=m+step4;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1 +=p1[0    ];
			sum2 +=p1[step ];
			sum3 +=p1[step2];
			sum4 +=p1[step3];
			       p1+=xstep;
			sum5 +=p2[0    ];
			sum6 +=p2[step ];
			sum7 +=p2[step2];
			sum8 +=p2[step3];
			       p2+=xstep;
		}
	    }
	    if(NUM_INDEX==4){
		THETYPE *p1=m;
		THETYPE *p2=p1+step*2;
		THETYPE *p3=p2+step*2;
		THETYPE *p4=p3+step*2;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1 +=p1[0   ];
			sum2 +=p1[step];
			       p1+=xstep;
			sum3 +=p2[0   ];
			sum4 +=p2[step];
			       p2+=xstep;
			sum5 +=p3[0   ];
			sum6 +=p3[step];
			       p3+=xstep;
			sum7 +=p4[0   ];
			sum8 +=p4[step];
			       p4+=xstep;
		}
	    }
	}
	sum1+=sum2;
	sum3+=sum4;
	sum5+=sum6;
	sum7+=sum8;
	sum1+=sum3;
	sum5+=sum7;
	sum1+=sum5;
	return(sum1);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//  ALGO for WRITE
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


	THETYPE
	sum_write_unroll1(THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps;
	THETYPE sum=0;
	THETYPE wr=towrite;
	int rep;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		for(steps=0;steps<ops_per_scan;steps++){
			sum+=*p1;*p1+=wr;p1+=step;
		}
	}
	return(sum);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_write_unroll2(THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*2;
	THETYPE sum=0;
	int rep;
	ops_per_scan/=2;
	THETYPE wr=towrite;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		THETYPE *p2=p1+step;
		for(steps=0;steps<ops_per_scan;steps++){
			sum+=*p1;*p1+=wr;p1+=xstep;
			sum+=*p2;*p2+=wr;p2+=xstep;
		}
	}
	return(sum);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_write_unroll4(THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*4;
	THETYPE sum=0;
	int rep;
	ops_per_scan/=4;
	THETYPE wr=towrite;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		THETYPE *p2=p1+step;
		THETYPE *p3=p2+step;
		THETYPE *p4=p3+step;
		for(steps=0;steps<ops_per_scan;steps++){
			sum+=*p1;*p1+=wr;p1+=xstep;
			sum+=*p2;*p2+=wr;p2+=xstep;
			sum+=*p3;*p3+=wr;p3+=xstep;
			sum+=*p4;*p4+=wr;p4+=xstep;
		}
	}
	return(sum);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_write_unroll8(THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*8;
	THETYPE sum=0;
	int rep;
	ops_per_scan/=8;
	THETYPE wr=towrite;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		THETYPE *p2=p1+step;
		THETYPE *p3=p2+step;
		THETYPE *p4=p3+step;
		THETYPE *p5=p4+step;
		THETYPE *p6=p5+step;
		THETYPE *p7=p6+step;
		THETYPE *p8=p7+step;
		for(steps=0;steps<ops_per_scan;steps++){
			sum+=*p1;*p1+=wr;p1+=xstep;
			sum+=*p2;*p2+=wr;p2+=xstep;
			sum+=*p3;*p3+=wr;p3+=xstep;
			sum+=*p4;*p4+=wr;p4+=xstep;
			sum+=*p5;*p5+=wr;p5+=xstep;
			sum+=*p6;*p6+=wr;p6+=xstep;
			sum+=*p7;*p7+=wr;p7+=xstep;
			sum+=*p8;*p8+=wr;p8+=xstep;
		}
	}
	return(sum);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_writespe_unroll2 (THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*2;
	THETYPE sum1=0,sum2=0;
	int rep;
	ops_per_scan/=2;
	THETYPE wr=towrite;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		THETYPE *p2=p1+step;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1+=*p1;*p1+=wr;p1+=xstep;
			sum2+=*p2;*p2+=wr;p2+=xstep;
		}
	}
	sum1+=sum2;
	return(sum1);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_writespe_unroll4 (THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*4;
	THETYPE sum1=0,sum2=0,sum3=0,sum4=0;
	int rep;
	ops_per_scan/=4;
	THETYPE wr=towrite;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		THETYPE *p2=p1+step;
		THETYPE *p3=p2+step;
		THETYPE *p4=p3+step;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1+=*p1;*p1+=wr;p1+=xstep;
			sum2+=*p2;*p2+=wr;p2+=xstep;
			sum3+=*p3;*p3+=wr;p3+=xstep;
			sum4+=*p4;*p4+=wr;p4+=xstep;
		}
	}
	sum1+=sum2;
	sum3+=sum4;
	sum1+=sum3;
	return(sum1);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_writespe_unroll8 (THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*8;
	THETYPE sum1=0,sum2=0,sum3=0,sum4=0,sum5=0,sum6=0,sum7=0,sum8=0;
	int rep;
	ops_per_scan/=8;
	THETYPE wr=towrite;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		THETYPE *p2=p1+step;
		THETYPE *p3=p2+step;
		THETYPE *p4=p3+step;
		THETYPE *p5=p4+step;
		THETYPE *p6=p5+step;
		THETYPE *p7=p6+step;
		THETYPE *p8=p7+step;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1+=*p1;*p1+=wr;p1+=xstep;
			sum2+=*p2;*p2+=wr;p2+=xstep;
			sum3+=*p3;*p3+=wr;p3+=xstep;
			sum4+=*p4;*p4+=wr;p4+=xstep;
			sum5+=*p5;*p5+=wr;p5+=xstep;
			sum6+=*p6;*p6+=wr;p6+=xstep;
			sum7+=*p7;*p7+=wr;p7+=xstep;
			sum8+=*p8;*p8+=wr;p8+=xstep;
		}
	}
	sum1+=sum2;
	sum3+=sum4;
	sum5+=sum6;
	sum7+=sum8;
	sum1+=sum3;
	sum5+=sum7;
	sum1+=sum5;
	return(sum1);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_writeind_unroll2 (THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*2;
	THETYPE sum1=0,sum2=0;
	int rep;
	ops_per_scan/=2;
	THETYPE wr=towrite;
	for(rep=0;rep<repeat;rep++){
		THETYPE *p1=m;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1+=p1[0];p1[0]+=wr;
			sum2+=p1[step];p1[step]+=wr;
			      p1+=xstep;
		}
	}
	sum1+=sum2;
	return(sum1);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_writeind_unroll4 (THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*4;
	int rep;
	int step2=step +step;
	int step3=step2+step;
	int step4=step3+step;
	THETYPE sum1=0,sum2=0,sum3=0,sum4=0;
	ops_per_scan/=4;
	THETYPE wr=towrite;
	for(rep=0;rep<repeat;rep++){
	    if(NUM_INDEX==1){
		THETYPE *p1=m;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1+=p1[0    ];p1[0    ]+=wr;
			sum2+=p1[step ];p1[step ]+=wr;
			sum3+=p1[step2];p1[step2]+=wr;
			sum4+=p1[step3];p1[step3]+=wr;
			      p1+=xstep;
		}
	    }
	    if(NUM_INDEX==2){
		THETYPE *p1=m;
		THETYPE *p2=m+step2;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1+=p1[0   ];p1[0   ]+=wr;
			sum2+=p1[step];p1[step]+=wr;
			      p2+=xstep;
			sum3+=p2[0   ];p2[0   ]+=wr;
			sum4+=p2[step];p2[step]+=wr;
			      p2+=xstep;
		}
	    }
	}
	sum1+=sum2;
	sum3+=sum4;
	sum1+=sum3;
	return(sum1);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	sum_writeind_unroll8 (THETYPE *m,THEINT max_index,int step, int repeat,THEINT ops_per_scan)
{
	THEINT steps,xstep=step*8;
	THETYPE sum1=0,sum2=0,sum3=0,sum4=0,sum5=0,sum6=0,sum7=0,sum8=0;
	int rep;
	int step2=step +step;
	int step3=step2+step;
	int step4=step3+step;
	int step5=step4+step;
	int step6=step5+step;
	int step7=step6+step;
	ops_per_scan/=8;
	THETYPE wr=towrite;
	for(rep=0;rep<repeat;rep++){
	    if(NUM_INDEX==1){
		THETYPE *p1=m;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1 +=p1[0    ];p1[0    ]+=wr;
			sum2 +=p1[step ];p1[step ]+=wr;
			sum3 +=p1[step2];p1[step2]+=wr;
			sum4 +=p1[step3];p1[step3]+=wr;
			sum5 +=p1[step4];p1[step4]+=wr;
			sum6 +=p1[step5];p1[step5]+=wr;
			sum7 +=p1[step6];p1[step6]+=wr;
			sum8 +=p1[step7];p1[step7]+=wr;
			       p1+=xstep;
		}
	    }
	    if(NUM_INDEX==2){
		THETYPE *p1=m;
		THETYPE *p2=m+step4;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1 +=p1[0    ];p1[0    ]+=wr;
			sum2 +=p1[step ];p1[step ]+=wr;
			sum3 +=p1[step2];p1[step2]+=wr;
			sum4 +=p1[step3];p1[step3]+=wr;
			       p1+=xstep;
			sum5 +=p2[0    ];p2[0    ]+=wr;
			sum6 +=p2[step ];p2[step ]+=wr;
			sum7 +=p2[step2];p2[step2]+=wr;
			sum8 +=p2[step3];p2[step3]+=wr;
			       p2+=xstep;
		}
	    }
	    if(NUM_INDEX==4){
		THETYPE *p1=m;
		THETYPE *p2=p1+step*2;
		THETYPE *p3=p2+step*2;
		THETYPE *p4=p3+step*2;
		for(steps=0;steps<ops_per_scan;steps++){
			sum1 +=p1[0   ];p1[0   ]+=wr;
			sum2 +=p1[step];p1[step]+=wr;
			       p1+=xstep;
			sum3 +=p2[0   ];p2[0   ]+=wr;
			sum4 +=p2[step];p2[step]+=wr;
			       p2+=xstep;
			sum5 +=p3[0   ];p3[0   ]+=wr;
			sum6 +=p3[step];p3[step]+=wr;
			       p3+=xstep;
			sum7 +=p4[0   ];p4[0   ]+=wr;
			sum8 +=p4[step];p4[step]+=wr;
			       p4+=xstep;
		}
	    }
	}
	sum1+=sum2;
	sum3+=sum4;
	sum5+=sum6;
	sum7+=sum8;
	sum1+=sum3;
	sum5+=sum7;
	sum1+=sum5;
	return(sum1);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//   Code to manage mem locality if feature supported by OS
//   usage : we hope the OS allocate in priority the memory located near the CPU where we are at the
//           moment of the large malloc
//   proved to work on hpux+CLM+Itanium
//   proved to work on DL585+linux 2.6.7 gave stream 13.8
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#ifdef __AFFINITY
extern "C"{
int sched_setaffinity(int,int,void*);
int sched_getaffinity(int,int,void*);
}
void set_affinity(THEINT mask)
{
	int ret;
	ret = sched_setaffinity(0,  8, &mask);
	mask=-1;
	ret = sched_getaffinity(0,  8, &mask);
	printf("__1__ sched_getaffinity = %d, mask = %08lx\n", ret, mask);
}
#else
void set_affinity(THEINT mask)
{
}
#endif

#ifdef __hpux
#include <sys/mpctl.h>
#include <unistd.h>
void set_affinity(int processor)
{
	int newproc,process;
	process=getpid();
        printf ("__1__ try to attach process %d on processor\n", process, processor);
	newproc = mpctl(MPC_SETPROCESS_FORCE, processor, process);
        if (newproc == -1) {
            perror("__1__ mpctl call failed");
        } else {
            printf ("__1__ process %d is now on processor %d\n", process, newproc);
        }
}
#endif

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//   we use the content of the data to count the number of memory operations
//   then for read count 1 per operation
//   then for write count 2 per operation    read+write
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void
init_mat()
{
        int lp;unsigned int m2;
        THEINT si=MAT_SIZE;
        set_affinity(MEM_AFF); //JEAN
	printf("_____  %lld \n",si);
        mat_for_free=(THETYPE*)malloc(si);
        assert(mat_for_free!=NULL);
        MAT_SIZE/=(sizeof(THETYPE));
        mat=mat_for_free;
        mat += (MAT_OFFSET/sizeof(THETYPE));
        if(VERBOSE!=0)
        printf("__ mat address is %p %lld\n",mat_for_free,si);
        if(moderw==MODER){
                if(VERBOSE!=0)
                for(lp=0;lp<MAT_SIZE;lp++)mat[lp]=1.0; /* store 1 to count the read accesses */
        }
        if(moderw==MODEW){
                if(VERBOSE!=0)
                for(lp=0;lp<MAT_SIZE;lp++)mat[lp]=2.0; /* store 1 to count the write accesses */
        }
	// need to set CPU affinity after having writen in memory
	// tested on hpux+linux(ia64+opteron)
        set_affinity(CPU_AFF); //JEAN
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	THETYPE
	work_part2(double *mat,int mode,int unroll,THEINT max_index,int step)
{
	 int repeat;
	THEINT ops_per_scan=max_index/step;
	if (ops_per_scan<MIN_OPS_PER_SCAN)return(BIG_VAL);
	repeat=MAX_OPS/ops_per_scan;
	if(repeat==0)repeat=1;
	towrite=0.0;

//    printf ("\n_work_part2_ maxindex     %d \n", max_index);
//    printf ("_work_part2_ step           %d \n", step);
//    printf ("_work_part2_ ops_per_scan   %d \n", ops_per_scan);
//    printf ("_work_part2_ Repeat         %d \n", ops_per_scan);
//    exit (-1);


    if(mode==MODE_READ){
		if(unroll==UNROLL1){
			return(sum_read_unroll1(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL2){
			return(sum_read_unroll2(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL4){
//            if (step == 64 )
//                printf  ( "\nmax_index %d step %d repeat %d ops_per_scan %d \n ", max_index, step, repeat, ops_per_scan);
            return(sum_read_unroll4(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL8){
			return(sum_read_unroll8(mat,max_index,step,repeat,ops_per_scan));
		}
	}else
	if(mode==MODE_READSPE){
		if(unroll==UNROLL2){
			return(sum_readspe_unroll2(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL4){
			return(sum_readspe_unroll4(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL8){
			return(sum_readspe_unroll8(mat,max_index,step,repeat,ops_per_scan));
		}
	}
	if(mode==MODE_READIND){
		if(unroll==UNROLL2){
			return(sum_readind_unroll2(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL4){
			return(sum_readind_unroll4(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL8){
			return(sum_readind_unroll8(mat,max_index,step,repeat,ops_per_scan));
		}
	}else
	if(mode==MODE_WRITE){
		if(unroll==UNROLL1){
			return(sum_write_unroll1(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL2){
			return(sum_write_unroll2(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL4){
			return(sum_write_unroll4(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL8){
			return(sum_write_unroll8(mat,max_index,step,repeat,ops_per_scan));
		}
	}else
	if(mode==MODE_WRITESPE){
		if(unroll==UNROLL2){
			return(sum_writespe_unroll2(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL4){
			return(sum_writespe_unroll4(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL8){
			return(sum_writespe_unroll8(mat,max_index,step,repeat,ops_per_scan));
		}
	}
	if(mode==MODE_WRITEIND){
		if(unroll==UNROLL2){
			return(sum_writeind_unroll2(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL4){
			return(sum_writeind_unroll4(mat,max_index,step,repeat,ops_per_scan));
		}
		if(unroll==UNROLL8){
			return(sum_writeind_unroll8(mat,max_index,step,repeat,ops_per_scan));
		}
	}
	return(0.0);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	void
	work(char **argv,int mode,int unroll,char *mes)
{
	double log_max_index,time1,time2,ns_per_op,num_ops,best_measure,worst_measure,sum_measures;
	THEINT max_index;
	int step,measure;
        printf("_ %s  maxKmem ",message_info);
        for(step=MIN_STRIDE;step<=MAX_STRIDE;step*=2){
                if (DISP==DISP_AVE )printf(" AV=%4d  ",step*8);
                if (DISP==DISP_BEST)printf(" BE=%4d  ",step*8);
                if (DISP==DISP_TWO )printf(" BE=%4d AV=%4d  ",step*8,step*8);
                if (DISP==DISP_ALL )printf(" BE=%4d WO=%4d AV=%4d  ",step*8,step*8,step*8);
        }
	printf("\n");
	for(log_max_index=MIN_LOG10;log_max_index<MAX_LOG10+.0000001;log_max_index+=STEP_LOG10){
                max_index=(THEINT)(double)(exp(log_max_index*LOG10)+0.5);
                THEINT istride=max_index*sizeof(THETYPE)/1024;
                if(max_index>MAT_SIZE)break;
                printf("_ %s %8lld ",message_info,istride);





                for(step=MIN_STRIDE;step<=MAX_STRIDE;step*=2){
			double gb;
                        best_measure=BIG_VAL;
                        worst_measure=0;
                        sum_measures=0.0;
                        for(measure=0;measure<MAX_MEASURES;measure++){
                                double t;

//                                printf("\n");
//                                printf ("_work_ MIN_LOG10   %f \n", MIN_LOG10);
//                                printf ("_work_ MAX_LOG10   %f \n", MAX_LOG10);
//                                printf ("_work_ STEP_LOG10  %f \n", STEP_LOG10);
//                                printf ("_work_ max_index   %d \n", max_index);
//                                printf ("_work_ step        %d \n", step);
//                                printf ("_work_ MIN_STRIDE   %d \n", MIN_STRIDE);
//                                exit (-1);

                                time1=dml_micros();
                                num_ops=work_part2(mat,mode,unroll,max_index,step);
                                time2=dml_micros();
                                t=(time2-time1)*1000.0;
                                if(num_ops!=0)total_loops+=num_ops;
                                if(t<best_measure)best_measure=t;
                                if(t>worst_measure)worst_measure=t;
                                sum_measures+=t;
                        }

                        ns_per_op=best_measure/num_ops;
			gb=CACHE_LINE;gb/=ns_per_op;
			if(NSGBCY==DISP_GB)ns_per_op=gb;
			if(NSGBCY==DISP_CY)ns_per_op*=GHZ;
			if((DISP==DISP_ALL)||(DISP==DISP_BEST)||(DISP==DISP_TWO)){
                        	printf(" %7.3f",ns_per_op);
			}

                        ns_per_op=worst_measure/num_ops;
			gb=CACHE_LINE;gb/=ns_per_op;
			if(NSGBCY==DISP_GB)ns_per_op=gb;
			if(NSGBCY==DISP_CY)ns_per_op*=GHZ;
			if(DISP==DISP_ALL){
                        	printf(" %7.3f",ns_per_op);
			}

            ns_per_op=sum_measures/num_ops/MAX_MEASURES;

            gb=CACHE_LINE;gb/=ns_per_op;

                    if(NSGBCY==DISP_GB){
                        ns_per_op=gb;
                    }
			if(NSGBCY==DISP_CY)ns_per_op*=GHZ;

//            cout << "\n JEAN : " << ns_per_op << " " << gb << " real : " ;

			if((DISP==DISP_ALL)||(DISP_TWO)||(DISP_AVE)){
                        	printf(" %7.3f  ",ns_per_op);
			}
                }
        printf("\n");

                fflush(stdout);
	}
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	void
	init_int_parm(int argc,char **argv,int *the_int,const char *name)
{
        int parm;
        for(parm=1;parm<argc;parm++){
		char *where=strstr(argv[parm],name);
		if(where!=NULL)
		if(where[strlen(name)]=='=')
		{
			*the_int=atoi(where+strlen(name)+1);
			if(VERBOSE!=0)
			printf("____ %s = %d\n",name,*the_int);
		}
	}
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	void
	init_uint64_parm(int argc,char **argv,THEINT *the_int,const char *name)
{
        int parm;
        for(parm=1;parm<argc;parm++){
		char *where=strstr(argv[parm],name);
		if(where!=NULL)
		if(where[strlen(name)]=='=')
		{
			*the_int=strtoull(where+strlen(name)+1,0,10);
			if(VERBOSE!=0)
			printf("____ %s = %lld\n",name,*the_int);
		}
	}
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	void
	init_double_parm(int argc,char **argv,double *the_double,const char *name)
{
        int parm;
        for(parm=1;parm<argc;parm++){
		char *where=strstr(argv[parm],name);
		if(where!=NULL)
		if(where[strlen(name)]=='=')
		{
			sscanf(where+strlen(name)+1,"%lf",the_double);
			if(VERBOSE!=0)
			printf("____ %s = %1.5f\n",name,*the_double);
		}
	}
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	void
	init_int_with_str_parm(int argc,char **argv,int *the_int,const char *name,const char *what,int real)
{
        int parm;
        for(parm=1;parm<argc;parm++){
		char *where=strstr(argv[parm],name);
		if(where!=NULL)
		if(where[strlen(name)]=='=')
		{
			if(strcmp(where+strlen(name)+1,what)==0){
				*the_int=real;
				if(VERBOSE!=0)
				printf("____ %s = %d\n",name,*the_int);
			}
		}
	}
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	void
	init_parms(int argc,char **argv)
{
	int parm;
  	mypid=getpid();
        for(parm=2;parm<argc;parm++){
		strupr(argv[parm]);
	}
	init_uint64_parm(argc,argv,&MAT_SIZE,"MAT_SIZE");
	init_uint64_parm(argc,argv,&MAT_OFFSET,"MAT_OFFSET");
	init_uint64_parm(argc,argv,&CPU_AFF,"CPU_AFF");
	init_uint64_parm(argc,argv,&MEM_AFF,"MEM_AFF");
	init_uint64_parm(argc,argv,&MAX_OPS ,"MAX_OPS");
	init_int_parm(argc,argv,&VERBOSE,"VERBOSE");
	init_int_parm(argc,argv,&GHZ,"GHZ");
	init_int_parm(argc,argv,&MIN_STRIDE,"MIN_STRIDE");
	init_int_parm(argc,argv,&MAX_STRIDE,"MAX_STRIDE");
	init_int_parm(argc,argv,&MAX_MEASURES,"MAX_MEASURES");
	init_int_parm(argc,argv,&UNROLL,"UNROLL");
	init_int_parm(argc,argv,&NUM_INDEX,"NUM_INDEX");
	init_int_parm(argc,argv,&CACHE_LINE,"CACHE_LINE");
	init_double_parm(argc,argv,&MIN_LOG10,"MIN_LOG10");
	init_double_parm(argc,argv,&MAX_LOG10,"MAX_LOG10");
	init_double_parm(argc,argv,&STEP_LOG10   ,"STEP_LOG10");
	init_int_with_str_parm(argc,argv,&MODE,"MODE","READ",MODE_READ);
	if(MODE==MODE_READ)moderw=MODER;
	init_int_with_str_parm(argc,argv,&MODE,"MODE","READSPE",MODE_READSPE);
	if(MODE==MODE_READSPE)moderw=MODER;
	init_int_with_str_parm(argc,argv,&MODE,"MODE","READIND",MODE_READIND);
	if(MODE==MODE_READIND)moderw=MODER;
	init_int_with_str_parm(argc,argv,&MODE,"MODE","WRITE",MODE_WRITE);
	if(MODE==MODE_WRITE)moderw=MODEW;
	init_int_with_str_parm(argc,argv,&MODE,"MODE","WRITESPE",MODE_WRITESPE);
	if(MODE==MODE_WRITESPE)moderw=MODEW;
	init_int_with_str_parm(argc,argv,&MODE,"MODE","WRITEIND",MODE_WRITEIND);
	if(MODE==MODE_WRITEIND)moderw=MODEW;
	init_int_with_str_parm(argc,argv,&DISP,"DISP","BEST",DISP_BEST);
	init_int_with_str_parm(argc,argv,&DISP,"DISP","AVE",DISP_AVE);
	init_int_with_str_parm(argc,argv,&DISP,"DISP","TWO",DISP_TWO);
	init_int_with_str_parm(argc,argv,&DISP,"DISP","ALL",DISP_ALL);
	init_int_with_str_parm(argc,argv,&NSGBCY,"DISP","NS",DISP_NS);
	init_int_with_str_parm(argc,argv,&NSGBCY,"DISP","GB",DISP_GB);
	init_int_with_str_parm(argc,argv,&NSGBCY,"DISP","CY",DISP_CY);
	/* some checkings */
	assert((CACHE_LINE==64)||(CACHE_LINE==128));
	MAT_SIZE +=1;
	MAT_SIZE *=(1024*1024);
	assert(MAT_SIZE>=1024*1024);
	MAX_OPS  *=(1024*1024);
	assert(  (UNROLL==UNROLL1)||(UNROLL==UNROLL2)||(UNROLL==UNROLL4)||(UNROLL==UNROLL8));
	assert((MODE==MODE_READ)||(MODE==MODE_READSPE)||(MODE==MODE_READIND)||
	       (MODE==MODE_WRITE)||(MODE==MODE_WRITESPE)||(MODE==MODE_WRITEIND));
	assert((NUM_INDEX==1)||(NUM_INDEX==2)||(NUM_INDEX==4));
#ifdef __AFFINITY
	if(moderw==MODER)
	sprintf(message_info,"%s_RD_%02lld_%02lld_%05d_",argv[1],CPU_AFF,MEM_AFF,mypid);
	if(moderw==MODEW)
	sprintf(message_info,"%s_WR_%02lld_%02lld_%05d_",argv[1],CPU_AFF,MEM_AFF,mypid);
#else
	if(moderw==MODER)
	sprintf(message_info,"%s_RD_%05d_",argv[1],mypid);
	if(moderw==MODEW)
	sprintf(message_info,"%s_WR_%05d_",argv[1],mypid);
#endif
	if(NSGBCY==DISP_CY)
	assert(GHZ!=0);
        MIN_STRIDE/=sizeof(THETYPE);
        MAX_STRIDE/=sizeof(THETYPE);
#ifdef __linux__  // works only with 64 procs !!!!!
	CPU_AFF=(THEINT)1ULL<<CPU_AFF;
	MEM_AFF=(THEINT)1ULL<<MEM_AFF;
#endif
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	int
	main(int argc,char **argv)
{
	double tot,band;
	char mes[1024];
	if(argc==1){
		help();
		return(0);
	}
	strcpy(mes,argv[1]);
	init_parms(argc,argv);
	init_mat();
	start_time=dml_micros();
	work(argv,MODE,UNROLL,mes);
	end_time=dml_micros();
	tot=(end_time-start_time)*1000.0/total_loops;
	band=CACHE_LINE/tot;
	printf("_ %s  total micros = %1.0f total loops = %1.0f perfo = %1.2f ns/loop band= %1.3lf GB/s\n",
		message_info,end_time-start_time,total_loops,tot,band);
	return(0);
}




