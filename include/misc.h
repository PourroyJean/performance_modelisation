//
// Created by Jean Pourroy on 02/12/2016.
//

#ifndef __MISC_H__
#define __MISC_H__

#include <sys/time.h> //taking time
#include <time.h> //taking time
#include <inttypes.h>

#define TIC gettimeofday(&start_time, NULL);
#define TOC(s) gettimeofday(&end_time, NULL); print_times(s);
struct timeval start_time, end_time;
void print_times(char *s);



typedef uint64_t ui64;
/**
 * This assembly function return the number of the current cycle
 * @return  cycle number
 */
ui64 dml_cycles();


#endif //__misc_H__
