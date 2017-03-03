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
#include "tool_freq_parameters.h"
#include "tool_freq_generators.h"
#include "tool_freq_misc.h"
#



std::string BIN_DIR ;

int main(int argc, char **argv) {
    init_tool_freq ();


    //----------- ARGUMENT PARSING --------------
    Tool_freq_parameters * tool_freq_parameters = new Tool_freq_parameters();
    tool_freq_parameters->parse_arguments(argc, argv);
    tool_freq_parameters->check_arguments();


    //------------ CODE GENERATION  -------------
    Tool_freq_generators * generator = new Tool_freq_generators (tool_freq_parameters);
    generator->Generate_code();

    //------------ ASSEMBLY COMPILATION ---------

    string stmp = "bash -c \"g++ --std=c++11  -o " + FILE_ASM_EXE + " " +  FILE_ASM_SOURCE_GENERATED +  "\"";
    cout << " END : " << stmp << endl;
    system(stmp.c_str());

    //----------- EXECUTING --------------------
    generator->ExecuteAssembly();

    //----------- CONCLUDING -------------------
    int NbCycle = generator->mExecutionCycle;
    int NbInstruction = tool_freq_parameters->P_LOOP_SIZE * tool_freq_parameters->P_OPERATIONS.size();
    float IPC = (float) NbInstruction/ (float) NbCycle;

    cout << " Nb Instruction    " << NbInstruction << endl
         << " Nb Cycle          " << NbCycle << endl
         << " IPC               " << fixed << setprecision(10) <<  IPC << endl;



    return 0;


    native_frequency();


    return 0;
}
