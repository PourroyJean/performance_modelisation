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

//These files are two template used to generate the code. _start and _end containing the beginning and the ending of the final file
#define FILE_TEMPLATE_START         (BIN_DIR  + "/tool_freq_template_start.cpp")
#define FILE_TEMPLATE_END           (BIN_DIR  + "/tool_freq_template_end.cpp")
//This is the generated cpp file and the program compiled with. Generated file are created in the user's current directory
#define FILE_ASM_SOURCE_GENERATED   (HOME_DIR  + "/assembly_generated.cpp")
#define FILE_ASM_EXE                (HOME_DIR  + "/assembly")
//During the execution we use a temporal file to store the value (cycle and time)
#define FILE_MONTORING_TMP          (HOME_DIR + "/tmp_tool_freq_output")


//--------------
//- PARAMETERS -
//--------------
//Below are the default value of the generator.
//Do not use them direclty on the code because they could be overwritten by the user  at the execution (parameters flag)
#define PARAM_SIMD          SCALAR
#define PARAM_WIDTH         64
#define PARAM_OPERATIONS    "aaaaa"
#define PARAM_BIND          -1
#define PARAM_DEPENDENCY    false
#define PARAM_PRECISION     "double"
#define PARAM_VERBOSE       false
#define PARAM_LOOP_SIZE     3000000

//We do  sevarel measure to calculate the IPC perfromance of the program
#define PARAM_NB_LOOP       100

//These both string are used to locate where the binary is, and from where the user is launching it.
extern std::string BIN_DIR ;
extern std::string HOME_DIR ;


void usage();

std::vector<std::string> split(const std::string &s, char delim);

double mygettime();

uint64_t rdtsc();

void native_frequency();

void init_tool_freq ();

#endif //PERFORMANCE_MODELISATION_TOOL_FREQ_MISC_H
