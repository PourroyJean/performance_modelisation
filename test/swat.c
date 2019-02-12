/*********************************************************************/
/* BENCHMARK                                                         */
/*-------------------------------------------------------------------*/
/* Parameters :                                                      */
/*  - NROW  : 100,000,000                                            */
/*  - NCOL  : 128                                                    */
/*  - NITER : 860                                                    */
/*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <assert.h>

#include <sys/times.h>
#include "ktypes.h"
#include "crc32.h"

#include <sys/time.h>

double
dml_micros() {
    static struct timezone tz;
    static struct timeval tv;
    gettimeofday(&tv, &tz);
    return ((tv.tv_sec * 1000000.0) + tv.tv_usec);
}


#if USEMPI == 1
#include <mpi.h>
#endif

#ifdef TEST
#define NROW    32             /* number of rows (table)    */
#define NCOL    16             /* number of columns (table) */
#define NITER   10             /* number of iterations      */
#define CHECKSUM 0x27c0086e    /* Checksum for test         */
#else
#define NROW    100000000     /* number of rows (table)    */
#define NCOL    128            /* number of columns (table) */
#define NITER   860            /* number of iterations      */
#endif

#ifndef CLK_TCK
#define CLK_TCK 60
#endif

#define SEED    536870911ULL
#define MYRANDMAX 0xFFFFFFFBULL
#define MYRAND_A  134217727ULL
#define MYRAND_B  32767ULL
ui64 next;

/*********************************************************************/
/* get_times gives the elapsed time                                  */
/*********************************************************************/

#include <sys/time.h>

static double get_times(void) {
    //struct tms tbuf;
    //times(&tbuf);
    //return (1.0 * tbuf.tms_utime/CLK_TCK);
    struct timezone tz;
    struct timeval tv;
    gettimeofday(&tv, &tz);
    return (((tv.tv_sec * 1000000.0) + tv.tv_usec) / 1000000.0);
}


/*********************************************************************/
/* Pseudo-Random Number Generator                                    */
/*-------------------------------------------------------------------*/
/* myrand_init seeds the generator                                   */
/* myrand outputs a pseudo-random 32-bit number                      */
/*********************************************************************/

static void myrand_init(ui64 seed) {
    next = seed;
    return;
}

static ui32 myrand(void) {
    next = (next * MYRAND_A + MYRAND_B) % MYRANDMAX;
    return (ui32) next;
}

/*********************************************************************/
/* Table : Allocation, Initialisation and free                       */
/*-------------------------------------------------------------------*/
/* A table is a 2-dimensions array of integers in [0,NROW] of nrow   */
/* rows and ncol columns.                                            */
/* The table is initialized with a pseudo-random generator seeded    */
/* by the line number.                                               */
/*********************************************************************/

static void table_alloc(ui32 ***table, int nrow, int ncol) {
    int irow;

    *table = (ui32 **) malloc(nrow * sizeof(ui32 * ));
    for (irow = 0; irow < nrow; irow++)
        (*table)[irow] = (ui32 *) malloc(ncol * sizeof(ui32));
    return;
}

static void table_init(ui32 **table, int nrow, int ncol, int irowbeg) {
    int irow, icol;

    for (irow = 0; irow < nrow; irow++) {
        myrand_init((ui64)(irowbeg + irow));
        for (icol = 0; icol < ncol; icol++)
            table[irow][icol] = (ui32)(myrand() & 0x00000000ffffffffULL) % NROW;
    }

    return;
}

static void table_free(ui32 ***table, int nrow) {
    int irow;

    for (irow = 0; irow < nrow; irow++)
        free((*table)[irow]);
    free(*table);

    return;
}


/*********************************************************************/
/* Data : Allocation, Initialisation, Copy and free                  */
/*-------------------------------------------------------------------*/
/* Data is a 1-dimension array of 64-bit integers of nrow elements   */
/* Data is initialized with a pseudo-random generator                */
/*********************************************************************/

int nrowslice;  /* num. of rows in a slice, current pe    */
int nrowslice1; /* num. of rows in a slice, pe = 0..npe-2 */
int nrowslice2; /* num. of rows in a slice, pe = npe-1    */

#define ADDR (0x0UL)
int myrank;
int mysize;
int shmid = -1;

static void free_largepages(void *data) {
    shmdt(data);
}

static void data_alloc_spe(ui64 **data, int nrow) {
    void *p;
    size_t len = (ui64) nrow * sizeof(ui64);
    len >>= 24;
    len++;
    len <<= 24; //align 16MB
    shmid = shmget(IPC_PRIVATE, len, SHM_HUGETLB | IPC_CREAT | SHM_R | SHM_W);
    assert(shmid >= 0);
    p = (ui64 *) shmat(shmid, (void *) ADDR, SHM_RND);
    *data = (ui64 *) p;
    assert(*data != (void *) (-1ULL));
}

static void data_alloc(ui64 **data, int nrow) {
    *data = (ui64 *) malloc(nrow * sizeof(ui64));
    return;
}

static void data_init(ui64 *data, int nrow) {
    int irow;

    myrand_init(SEED);
    for (irow = 0; irow < nrow; irow++)
        data[irow] = ((ui64) myrand() << 32) ^ (ui64) myrand();

    return;
}

static void data_copy(ui64 *data, ui64 *newdata, int nrow) {
    memcpy(data, newdata, nrow * sizeof(ui64));
    return;
}

static void data_free(ui64 **data) {
    free(*data);
    return;
}

static void data_free_spe(ui64 **data) {
    shmdt(*data);
}


/*********************************************************************/
/* Print Table and Data [DEBUG 2 ONLY]                               */
/*********************************************************************/

#ifdef TEST
static void table_print(ui32 **table, int nrow, int ncol, int label) {
  int irow, icol;

  for (irow = 0; irow < nrow; irow ++) {
    fprintf(stderr,"[T][%2d] ",label);
    for (icol = 0; icol < ncol; icol ++)
      fprintf(stderr, "%2d ", table[irow][icol]);
    fprintf(stderr, "\n");
  }

  return;
}

static void data_print(ui64 *data, int nrow, int label) {
  int irow;

  for (irow = 0; irow < nrow; irow ++)
    fprintf(stderr, "[D][%2d] %016llx\n", label,data[irow]);

  return;
}
#endif /*TEST*/


/*********************************************************************/
/* process_data [OPTIMIZE IT !!!]                                    */
/*-------------------------------------------------------------------*/
/* From table and data, this function creates newdata                */
/* Each element of newdata is the xor of NCOL elements of data       */
/* The choice of the NCOL elements is given by the table             */
/*********************************************************************/

static void process_data(ui64 *newdata,
                         ui64 *data,
                         ui32 **table,
                         int nrow,
                         int ncol) {
    int irow;

#pragma omp parallel for default(shared) private(irow)
    for (irow = 0; irow < nrow; irow++) {
        ui64 t1 = 0;
        ui64 t2 = 0;
        int icol;
        for (icol = 0; icol < ncol; icol += 2) {
            t1 ^= data[table[irow][icol]];
            t2 ^= data[table[irow][icol + 1]];
        }
        t1 ^= t2;
        newdata[irow] = t1;
    }
}

/*********************************************************************/
/* inter_pe_copy [MPI VERSION ONLY][OPTIMIZE IT !!!]                 */
/*-------------------------------------------------------------------*/
/* Each processor holds a slice of newdata. At the end of this       */
/* function, every processor must hold every slices to rebuild       */
/* newdata.                                                          */
/* Here, it is easily (and inefficiently) implemented with           */
/* broadcasts; optimization depends on the topology of the inter     */
/* processor network                                                 */
/*********************************************************************/
#if USEMPI == 1
static void inter_pe_copy(ui64 *newdata_slice,
              ui64 * data,
              int irowbeg,
              int nrowslice,
              int nrowslice1,
              int nrowslice2,
              int npe,
              MPI_Comm comm) {
  
  int ipe;
  /* copy my slice in data at its right place */
  data_copy(data+irowbeg, newdata_slice, nrowslice);
  
  /* do the broadcast for slices 0..npe-2 */  
  for (ipe = 0; ipe < npe - 1; ipe ++)
    MPI_Bcast(data + ipe * nrowslice1, 2*nrowslice1, MPI_INT, ipe, comm);
  
  /* do the broadcast for possibly larger slice npe-1 */
  MPI_Bcast(data + ipe * nrowslice1, 2*nrowslice2, MPI_INT, ipe, comm);
  
  MPI_Barrier(comm);
  
  return;
}
#endif /* USEMPI */



/*********************************************************************/
/* main : MONO-CPU VERSION ONLY                                      */
/*-------------------------------------------------------------------*/
/* Process the data NITER times                                      */
/* The output data is used as input for the next iteration           */
/*********************************************************************/

#if USEMPI == 0

int main(int argc, char *argv[]) {

    ui64 *data;
    ui64 *newdata;
    ui32 **table;
    int iiter;
    unsigned long crc;

    double start_chrono_bench;
    double stop_chrono_bench;
    double start_chrono;
    double stop_chrono;
    double time_process = 0.0;
    double time_copy = 0.0;
    double time_bench;


    table_alloc(&table, NROW, NCOL);
    data_alloc(&data, NROW);
    data_alloc(&newdata, NROW);

    table_init(table, NROW, NCOL, 0);
    data_init(data, NROW);

#ifdef TEST
    table_print(table,NROW,NCOL,0);
    data_print(data,NROW,0);
    fprintf(stderr,"-- BEGIN --\n");
#endif /*TEST*/

    start_chrono_bench = get_times(); /* BENCH STARTS HERE */
    for (iiter = 0; iiter < NITER; iiter++) {
        /* process_data */
        start_chrono = get_times();
        process_data(newdata, data, table, NROW, NCOL);
        stop_chrono = get_times();
        time_process += stop_chrono - start_chrono;

        /* data_copy */
        start_chrono = get_times();
        data_copy(data, newdata, NROW);
        stop_chrono = get_times();
        time_copy += stop_chrono - start_chrono;
    }
    stop_chrono_bench = get_times();  /* BENCH STOPS HERE */
    time_bench = stop_chrono_bench - start_chrono_bench;

#ifdef TEST
    fprintf(stderr,"--- END ---\n");
    data_print(data,NROW,0);
#endif /*TEST*/


    crc = crc32(0, data, NROW);
    fprintf(stdout, "crc = %08lx\n", crc);
    fprintf(stdout, "bench time   : %lf\n", time_bench);
    fprintf(stdout, "process time : %lf\n", time_process);
    fprintf(stdout, "copy time    : %lf\n", time_copy);

    table_free(&table, NROW);
    data_free_spe(&data);
    data_free(&newdata);

#ifdef TEST
    if (crc ^ CHECKSUM) {
      printf("Error: invalid checksum !!!\n");
      return -1;
    }
    printf("OK: valid checksum\n");
#endif /*TEST*/

    return (0);
}

#endif /*USEMPI*/


/*********************************************************************/
/* main : MPI VERSION ONLY                                           */
/*-------------------------------------------------------------------*/
/* Each processor holds a slice of the table and the whole data      */
/* So each processor is able to compute a slice of newdata           */
/* The parallelized program works as follows :                       */
/*    - each processor generates the pseudo-random data              */
/*    - each processor generates a slice of the matrix               */
/*    - for each iteration :                                         */
/*        - each processor computes a slice of new data              */
/*        - they broadcast their slices so that every processor can  */
/*          rebuild newdata                                          */
/*********************************************************************/

#if USEMPI == 1
int main(int argc, char * argv[]) {

  ui64 *data;
  ui64 *newdata_slice;
  ui32 **table_slice;
  int iiter;
  unsigned long crc;

  double start_chrono_bench;
  double stop_chrono_bench;
  double start_chrono;
  double stop_chrono;
  double time_process = 0.0;
  double time_copy = 0.0;
  double time_bench;

  MPI_Comm World_comm;
  int mype,npe;
  int nrowslice;  /* num. of rows in a slice, current pe    */
  int nrowslice1; /* num. of rows in a slice, pe = 0..npe-2 */
  int nrowslice2; /* num. of rows in a slice, pe = npe-1    */
  int irowbeg;    /* num. of the 1st row of the slice */

  MPI_Init((int *)(void *)&argc, &argv);
  World_comm = MPI_COMM_WORLD;
  MPI_Comm_size(World_comm, (int *)(void *)&npe);
  MPI_Comm_rank(World_comm, (int *)(void *)&mype);
 
  // we need nrowslice1 >= nrowslice2  for gather proper behavior
  if((NROW%npe)==0){
      nrowslice1 = NROW / npe;
      nrowslice2 = NROW - (npe - 1) * nrowslice1;
  }else{
      nrowslice1 = (NROW / npe)+1;
      nrowslice2 = NROW - (npe - 1) * nrowslice1;
  }

  if (mype != npe - 1)
    nrowslice = nrowslice1;
  else
    nrowslice = nrowslice2;

  irowbeg = mype * nrowslice1;

  table_alloc(&table_slice, nrowslice1, NCOL);
  data_alloc_spe(&data, nrowslice1*npe);
  data_alloc(&newdata_slice, nrowslice1);

  table_init(table_slice, nrowslice, NCOL, irowbeg);
  data_init(data, NROW);

#ifdef TEST
  table_print(table_slice,nrowslice,NCOL,mype);
  data_print(data,NROW,mype);
  if (mype==0) fprintf(stderr,"-- BEGIN --\n");
#endif /*TEST*/

  MPI_Barrier(World_comm);
  start_chrono_bench = get_times(); /* BENCH STARTS HERE */

  for(iiter = 0; iiter < NITER; iiter++) {
double dml1,dml2,dml3,dml4,dml5;
dml1=dml_micros();

    /* process_data */
    start_chrono = get_times();
    process_data(newdata_slice,data,table_slice,nrowslice,NCOL);
    stop_chrono = get_times();
    time_process+=stop_chrono-start_chrono;
dml2=dml_micros();
    MPI_Barrier(World_comm);
dml3=dml_micros();
    /* data_copy */
    start_chrono = get_times();
    MPI_Allgather(newdata_slice,nrowslice1*2,MPI_INT,data,nrowslice1*2,MPI_INT,World_comm);
dml4=dml_micros();
    MPI_Barrier(World_comm);

    stop_chrono = get_times();
    time_copy+=stop_chrono-start_chrono;
    unsigned long ii0=0;
    //unsigned long ii0=crc32(0, data, NROW);
dml5=dml_micros();
printf("_1_ %6d %4d %4d %12.0lf %12.0lf %12.0lf %12.0lf %12.0lf\n",iiter,mype,npe,
       dml5-dml1,dml2-dml1,dml3-dml2,dml4-dml3,dml5-dml4);

#ifdef TEST
  if (mype==0) 
    fprintf(stderr,"iteration %3d, t = %.3lf\n", iiter, stop_chrono);
#endif /*TEST*/
  }
  stop_chrono_bench = get_times();  /* BENCH STOPS HERE */
  time_bench = stop_chrono_bench-start_chrono_bench;  

#ifdef TEST
  if (mype==0) fprintf(stderr,"--- END ---\n");
  data_print(data,NROW,mype);
#endif /*TEST*/

  MPI_Finalize();
  if (mype==0) {
    crc = crc32(0, data, NROW);
    fprintf(stdout,"crc = %08lX\n", crc);
    fprintf(stdout,"bench time   : %.3lf\n", time_bench);
    fprintf(stdout,"process time : %.3lf\n", time_process);
    fprintf(stdout,"copy time    : %.3lf\n", time_copy);
  }


  table_free(&table_slice,nrowslice);
  free_largepages(data);
  data_free(&newdata_slice);

#ifdef TEST
  if (mype==0) {
    if (checksum ^ CHECKSUM) {
      printf("Error: invalid checksum !!!\n");
      return -1;
    } 
  }
  printf("OK: valid checksum\n");
#endif /*TEST*/

  return(0);
}
#endif /*USEMPI*/
