#ifndef PERFORMANCE_MODELISATION_KG_MISC_H
#define PERFORMANCE_MODELISATION_KG_MISC_H


#include <string>


#include <vector>

//****************************
//**        FILES PATH      **
//****************************
//These files are two template used to generate the code. _start and _end containing the beginning and the ending of the final file
#define FILE_TEMPLATE_START         (BIN_DIR  + "/kg_template_start.cpp")
#define FILE_TEMPLATE_END           (BIN_DIR  + "/kg_template_end.cpp")
#define FILE_TEMPLATE_FREQ           (BIN_DIR  + "/kg_template_frequency.cpp")
//This is the generated cpp file and the program compiled with. Generated file are created in the user's current directory
#define FILE_ASM_SOURCE_GENERATED   (HOME_DIR  + "/assembly_generated.cpp")
#define FILE_ASM_EXE                (HOME_DIR  + "/assembly")
//During the execution we use a temporal file to store the value (cycle and time)
#define FILE_MONTORING_TMP          (HOME_DIR + "/tmp_kg_output")
//If the graphical option is set, we draw some graph with python scripts
#define FILE_SCRIPT_REPARTITION     (BIN_DIR  + "/kg_graphical.sh")


//--------------
//- PARAMETERS -
//--------------
//Below are the default value of the generator.
//Do not use them direclty on the code because they could be overwritten by the user  at the execution (parameters flag)
#define PARAM_WIDTH         64
#define PARAM_OPERATIONS    "aaaaa"
#define PARAM_BIND          0
#define PARAM_DEPENDENCY    false
#define PARAM_PRECISION     "double"
#define PARAM_VERBOSE       false
#define PARAM_LOOP_SIZE     200000
#define PARAM_UNROLLING     1
#define PARAM_FREQUENCY     true
#define PARAM_GRAPH         false
#define PARAM_DEBUG         false
#define PARAM_COUNT         false

//We do  several measures to calculate the IPC of the program
#define PARAM_SAMPLES       100

//These both string are used to locate where the binary is, and from where the user is launching it.
extern std::string BIN_DIR ;
extern std::string HOME_DIR ;


void usage();

std::vector<std::string> split(const std::string &s, char delim);

double mygettime();

uint64_t rdtsc();


#endif //PERFORMANCE_MODELISATION_KG_MISC_H
