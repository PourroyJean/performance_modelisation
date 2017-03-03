#ifndef PERFORMANCE_MODELISATION_TOOL_FREQ_MISC_H
#define PERFORMANCE_MODELISATION_TOOL_FREQ_MISC_H


#include <string>

#define DEBUG 1
#ifdef DEBUG
# define DEBUG_PRINT(x) printf ((x))
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

#include <vector>

#define ASM_FILE_source "assembly_generated.cpp"
#define ASM_FILE_exe "assembly"


#define BENCH_NB_ITERATION 50000

//Below are the default value of the generator.
//Do not use them, because they could overwritten by the user (parameters flag)
#define PARAM_SIMD          SCALAR
#define PARAM_WIDTH         64
#define PARAM_OPERATIONS    "aaaaa"
#define PARAM_BIND          -1
#define PARAM_DEPENDENCY    false
#define PARAM_PRECISION     "double"
#define PARAM_VERBOSE       false
#define PARAM_LOOP_SIZE     200


void usage();

std::vector<std::string> split(const std::string &s, char delim);

double mygettime();

uint64_t rdtsc();

void native_frequency();


#endif //PERFORMANCE_MODELISATION_TOOL_FREQ_MISC_H
