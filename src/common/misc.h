//
// Created by Jean Pourroy on 02/12/2016.
//

#ifndef __MISC_H__
#define __MISC_H__

#include <sys/time.h> //taking time
#include <time.h> //taking time
#include <inttypes.h>

extern float TIME_ELAPSED;
extern struct timeval start_time, end_time;

#define TIC gettimeofday(&start_time, NULL);
#define TOC gettimeofday(&end_time, NULL); TIME_ELAPSED = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec);

void print_times(char *s);

#define OP_DEBUG 0
#ifdef  OP_DEBUG
    #define DEBUG cout << "_DEBUG_  "
#else
    #define DEBUG 0 && cout
#endif

typedef uint64_t ui64;
typedef uint32_t ui32;
/**
 * This assembly function return the number of the current cycle
 * @return  cycle number
 */
ui64 dml_cycles();


#ifdef OP_DEBUG
# define DEBUG_PRINT(x) std::cout << ((x))
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif


#endif //__misc_H__
