#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <iostream>
#include <iomanip>
#include "kg_parameters.h"
#include "kg_generators.h"
#include "kg_executor.h"
#include "kg_misc.h"
#




int main(int argc, char **argv) {

    //----------- ARGUMENT PARSING --------------
    KG_parameters * kg_parameters = new KG_parameters();
    kg_parameters->parse_arguments(argc, argv);

    //------------ CODE GENERATION  -------------
    KG_generators * generator = new KG_generators (kg_parameters);
    generator->Generate_code();

    //------------ ASSEMBLY COMPILATION ---------
    std::string stmp = "bash -c \"g++ --std=c++11  -o " + FILE_ASM_EXE + " " +  FILE_ASM_SOURCE_GENERATED +  "\"";
    system(stmp.c_str());

    //----------- EXECUTING --------------------
    KG_executor  * executor = new KG_executor (generator);
    executor->Execute_assembly();

    //----------- ANALYZING -------------------
    executor->Monitor_Execution();

    return 0;
}
