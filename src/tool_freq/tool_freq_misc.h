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

void usage ();

std::vector<std::string> split(const std::string &s, char delim);

#endif //PERFORMANCE_MODELISATION_TOOL_FREQ_MISC_H
