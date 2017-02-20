#ifndef PERFORMANCE_MODELISATION_TOOL_FREQ_MISC_H
#define PERFORMANCE_MODELISATION_TOOL_FREQ_MISC_H




#define DEBUG 1
#ifdef DEBUG
# define DEBUG_PRINT(x) printf (x)
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif



void usage ();


#endif //PERFORMANCE_MODELISATION_TOOL_FREQ_MISC_H
