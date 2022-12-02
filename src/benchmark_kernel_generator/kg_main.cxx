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



std::stringstream black_hole;


int main(int argc, char **argv) {

    //----------- ARGUMENT PARSING --------------
    KG_parameters * kg_parameters = new KG_parameters();
    kg_parameters->parse_arguments(argc, argv);

    //------------ CODE GENERATION  -------------
    KG_generators * generator = new KG_generators (kg_parameters);
    generator->Generate_code();

    if (kg_parameters->P_VERBOSE)
        generator->print_assembly_kernel();

    //------------ ASSEMBLY COMPILATION ---------
    std::string stmp = "bash -c \"" + kg_parameters->P_COMPILER + " -fopenmp " +  (kg_parameters->P_DEBUG ? "-g" : "")  +  " -o " + FILE_ASM_EXE + " " +  FILE_ASM_SOURCE_GENERATED +  "\"";
    if (kg_parameters->P_VERBOSE)
        cout << "+ Compilation command : " << stmp << endl;
    system(stmp.c_str());

    //----------- EXECUTING --------------------
    KG_executor  * executor = new KG_executor (generator);
    executor->Execute_assembly(kg_parameters->P_OPENMP);

    //----------- ANALYZING -------------------
    executor->Monitor_Execution();

    return 0;
}
