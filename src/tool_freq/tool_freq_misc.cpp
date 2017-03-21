//
// Created by Jean Pourroy on 20/02/2017.
//

#include <cstdio>
#include "tool_freq_misc.h"


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
            "Usage: ./tool_freq [-I INSTRUCTIONS ] [-W WIDTH] [-O operations] [-B BINDING] [-vh]\n");
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
