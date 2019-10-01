//
// Created by Jean Pourroy on 20/02/2017.
//

#include <cstdio>
#include "kg_misc.h"


#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/time.h>


#include <unistd.h>



using namespace std;

void usage() {
    fprintf(stderr,
            "Usage: ./benchmark_kernel_generator [-I INSTRUCTIONS ] [-W WIDTH] [-O operations] [-B BINDING] [-vh]\n");
}

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}


double mygettime() {
    struct timeval tp;
    struct timezone tzp;
    int i;
    i = gettimeofday(&tp, &tzp);
    return ((double) tp.tv_sec + (double) tp.tv_usec * 1.e-6);
}

uint64_t rdtsc() {
    uint32_t lo, hi;
    /* We cannot use "=A", since this would use %rax on x86_64 */
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t) hi << 32 | lo;
}


void native_frequency() {
    unsigned int time;
    uint64_t rtcstart, rtcend;
    double tstart, tend;
    unsigned int freq;


    tstart = mygettime();
    rtcstart = rdtsc();
    usleep(1000000);
    rtcend = rdtsc();
    tend = mygettime();
    time = rtcend - rtcstart;
    freq = time / (1000000 * (tend - tstart));
    printf("NATIVE_FREQUENCY %u MHz\n", freq);

}


void leftRotatebyOne(std::vector<int> * arr , int n)
{
    int i_1 = arr->at(n-1);
    for (int i = 0; i < n ; i++){
        int tmp = arr->at(i);
        arr->at(i) = i_1;
        i_1 = tmp;
    }
}

/*Function to left rotate arr[] of size n by d*/
void leftRotate(std::vector<int> * arr, int d, int n)
{
    for (int i = 0; i < d; i++)
        leftRotatebyOne(arr, n);
}
