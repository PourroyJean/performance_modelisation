//
// Created by Jean Pourroy on 20/02/2017.
//
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "tool_freq_generators.h"
#include "tool_freq_misc.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sched.h>
#include <unistd.h>
#include <fstream>      // std::fstream



//Write to Cpp or Header file
#define WC(s) fprintf(mFile_assembly_src, s "\n");
//#define WH(s) fprintf(P_FPH, s "\n");



void Tool_freq_generators::generate_assembly() {
    string strNbIteration = to_string(mParameters->P_LOOP_SIZE);

    WC("__asm__ (\"myBench: \" ");
    for (auto instruction: *mInstructions_set) {
        const char *s = instruction.c_str();
        fprintf(mFile_assembly_src, "\t\t\"");
        fprintf(mFile_assembly_src, instruction.c_str());
        fprintf(mFile_assembly_src, "\"\n");
    }
    fprintf(mFile_assembly_src, "\"  sub    $0x1, %%%%eax;\"\n");
    string jumpLine = "\"  jnz    myBench\" : : \"a\" (" + strNbIteration + ")";
    fprintf(mFile_assembly_src, jumpLine.c_str());
    WC(");");
}


void Tool_freq_generators::generate_source() {
    int n;
    char buffer[999];
    while ((n = fread(buffer, 1, 999, mFile_template_start)) > 0)
    {
        fwrite(buffer, 1, n, mFile_assembly_src);
    }

    generate_assembly();

    while ((n = fread(buffer, 1, 999, mFile_template_end)) > 0)
    {
        fwrite(buffer, 1, n, mFile_assembly_src);
    }



    fclose (mFile_template_start);
    fclose (mFile_template_end);
    fclose (mFile_assembly_src);

}


int Tool_freq_generators::Get_register_source() {
    if (mParameters->P_DEPENDENCY) {
        return mPrevious_target_register;
    } else {
        return 1;
    }
}

int Tool_freq_generators::Get_register_cible() {
    if (mPrevious_target_register == 15) {
        mPrevious_target_register = 1;
    }
    mPrevious_target_register++;
    return mPrevious_target_register;
}


void Tool_freq_generators::generate_instructions() {
    DEBUG_PRINT("-- Generating instructions vector\n");

    mInstructions_set->clear();
    mPrevious_target_register = 1;

    for (auto operation : *mOperations_set) {
        string saveSource = to_string(Get_register_source());
        string saveCible  = to_string(Get_register_cible());
        //v add p d
        string instruction = mPrefix + operation + mSuffix + mPrecision + " ";
        instruction += "%%%%" + mRegister_name + "0, ";
        instruction += "%%%%" + mRegister_name + saveSource + ", ";
        instruction += "%%%%" + mRegister_name + saveCible + "; ";
        //tmp pour verifier dependency
//        instruction += "%%%%" + mRegister_name + to_string(Get_register_cible()) + ", ";
//        instruction += "%%%%" + mRegister_name + to_string(Get_register_cible()) + ", ";
//        instruction += "%%%%" + mRegister_name + to_string(Get_register_cible()) + "; ";

        mInstructions_set->push_back(instruction);
    }

}


void Tool_freq_generators::Init_Generator() {
    DEBUG_PRINT("-- Init Generator register, prefix, suffix, and precision \n");
    if (mParameters->P_WIDTH == 64 || mParameters->P_WIDTH == 128) {
        mRegister_name = "xmm";
    }
    if (mParameters->P_WIDTH == 256) {
        mRegister_name = "ymm";
    }
    if (mParameters->P_WIDTH == 512) {
        mRegister_name = "zmm";
    }
    //only v instructions supported
    mPrefix = "v";
    if (mParameters->P_WIDTH == 64) {
        mSuffix = "s";
    } else {
        mSuffix = "p";
    }
    if (!mParameters->P_PRECISION.compare("single")) {
        mPrecision = "s";
    } else {
        mPrecision = "d";
    }

    for (auto op: mParameters->P_OPERATIONS) {
        if (op == 'a') {
            mOperations_set->push_back("add");
        }
        if (op == 'm') {
            mOperations_set->push_back("mul");
        }
        if (op == 'f') {
            mOperations_set->push_back("fmadd231");
        }
    }

}


string Tool_freq_generators::Generate_code() {
    DEBUG_PRINT("Generating assembly...\n");


    //----- INITIALISATION -----------
    Init_Generator();

    //Generate each final instruction
    generate_instructions();

    for (auto i: *mInstructions_set) {
        cout << i << endl;
    }

    generate_source();

//    fflush(P_FPC);
//    fflush(P_FPH);


    return "";
}

Tool_freq_generators::Tool_freq_generators(Tool_freq_parameters *param) {
//    P_FPC = fopen("assembly_generated.cpp", "w+");
//    P_FPH = fopen("assembly_generated.h", "w+");

    mParameters = param;
    mRegister_name = "xmm";
    mPrevious_target_register = 1;
    mPrefix = "v";
    mOperations_set = new vector<string>();
    mSuffix = "s";
    mPrecision = "d";

    string stmp = FILE_TEMPLATE_START;
    mFile_template_start  = fopen(stmp.c_str(), "rb");
    stmp = FILE_TEMPLATE_END;
    mFile_template_end    = fopen(stmp.c_str(), "rb");
    stmp = FILE_ASM_SOURCE_GENERATED;
    mFile_assembly_src    = fopen(stmp.c_str(), "wb");
}

void Tool_freq_generators::ExecuteAssembly(){
    DEBUG_PRINT("-- Execution the generated assembly file\n");

    //We let the kernel bind the process himself if no binding are set
    Cpu_binding();

    string stmp  (FILE_ASM_EXE);

        cout << "EXE" << stmp << endl;
    FILE *lsofFile_p = popen("/nfs/pourroy/code/THESE/performance_modelisation/build/bin/assembly", "r");

    if (!lsofFile_p)
    {
        return;
    }

    char buffer[1024];
    string line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
    std::vector<std::string> x = split(line_p, ' ');

    //First value returned is the cycle count followed by the frequency.
    mExecutionCycle =       stoi(x[0]);
    mExecutionFrequency =   stoi(x[1]);
    pclose(lsofFile_p);
    return;

}


void Tool_freq_generators::Cpu_binding() {

    //We only bind the process if the user enterer -B parameter
    if(mParameters->P_BIND < 0 ){
        return;
    }

    int i = 0;
    cpu_set_t mycpumask;

    CPU_ZERO(&mycpumask); //Clears set, so that it contains no CPUs.
    if (mParameters->P_BIND >= 0) {
        CPU_SET(mParameters->P_BIND, &mycpumask); //Add CPU cpu to set
        sched_setaffinity(0, sizeof(cpu_set_t), &mycpumask);
    };
    /* double-check */
    sched_getaffinity(0, sizeof(cpu_set_t), &mycpumask);
    for (i = 0; i < sysconf(_SC_NPROCESSORS_CONF); i++) {
        if (CPU_ISSET(i, &mycpumask)) printf("+ Running on CPU #%d\n", i);
    };
    return;
}


Tool_freq_generators::~Tool_freq_generators() {
//    fclose(P_FPC);
//    fclose(P_FPH);
}


