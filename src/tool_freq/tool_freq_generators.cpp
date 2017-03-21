//
// Created by Jean Pourroy on 20/02/2017.
//
#include <stdio.h>
#include <iostream>
#include "tool_freq_generators.h"
#include "tool_freq_misc.h"


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <fstream>      // std::fstream
#include <cmath>


void Tool_freq_generators::generate_assembly() {

    mFile_assembly_src << "__asm__ (\"myBench: \" " << endl;
    for (auto instruction: *mInstructions_set) {
        mFile_assembly_src << "\t\t\"" << instruction << "\"\n";
    }
    mFile_assembly_src << "\"  sub    $0x1, %%eax;\"\n";
    mFile_assembly_src << "\"  jnz    myBench\" : : \"a\" (" << mParameters->P_LOOP_SIZE << ")";
    mFile_assembly_src << ");";
}


void Tool_freq_generators::generate_source() {
    mFile_assembly_src << "#define TMP_FILE_monitoring \"" + FILE_MONTORING_TMP + "\"\n";

    //Template_START + LOOP_ASSEMBLY + Template_END
    mFile_assembly_src << mFile_template_start.rdbuf();
    generate_assembly();
    mFile_assembly_src << mFile_template_end.rdbuf();

    mFile_template_start.close();
    mFile_template_end.close();
    mFile_assembly_src.close();
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
        string saveCible = to_string(Get_register_cible());
        //v add p d
        string instruction = mPrefix + operation + mSuffix + mPrecision + " ";
        instruction += "%%" + mRegister_name + "0, ";
        instruction += "%%" + mRegister_name + saveSource + ", ";
        instruction += "%%" + mRegister_name + saveCible + "; ";
        //tmp pour verifier dependency
//        instruction += "%%" + mRegister_name + to_string(Get_register_cible()) + ", ";
//        instruction += "%%" + mRegister_name + to_string(Get_register_cible()) + ", ";
//        instruction += "%%" + mRegister_name + to_string(Get_register_cible()) + "; ";

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


void Tool_freq_generators::Generate_code() {
    DEBUG_PRINT("Generating assembly...\n");

    //----- INITIALISATION -----------
    Init_Generator();

    //Generate each final instruction
    generate_instructions();

    for (auto i: *mInstructions_set) {
        cout << i << endl;
    }

    generate_source();

    return;
}

Tool_freq_generators::Tool_freq_generators(Tool_freq_parameters *param) {
    mParameters = param;
    mRegister_name = "xmm";
    mPrevious_target_register = 1;
    mPrefix = "v";
    mOperations_set = new vector<string>();
    mSuffix = "s";
    mPrecision = "d";

    mFile_template_start.open(FILE_TEMPLATE_START, std::ios_base::binary);
    mFile_template_end.open(FILE_TEMPLATE_END, std::ios_base::binary);
    mFile_assembly_src.open(FILE_ASM_SOURCE_GENERATED, std::ios_base::binary);


    if (!(mFile_assembly_src.is_open() && mFile_template_end.is_open() && mFile_template_start.is_open())) {
        cerr << "Error opening one of these files: \n";
        cerr << FILE_TEMPLATE_START << endl;
        cerr << FILE_TEMPLATE_END << endl;
        cerr << FILE_ASM_SOURCE_GENERATED << endl;
        exit(0);
    }
}

void Tool_freq_generators::ExecuteAssembly() {
    DEBUG_PRINT("-- Execution the generated assembly file\n");

    //We let the kernel bind the process himself if no binding are set
    Cpu_binding();

    string stmp(FILE_ASM_EXE);
    system(stmp.c_str());


//    const char * path = "/nfs/pourroy/code/THESE/performance_modelisation/build/bin/assembly";
//    const char * path = "./bin/assembly";
//    FILE *lsofFile_p = popen(path, "r");
    return;

}


void Tool_freq_generators::Monitor_Execution() {
    ifstream res_file(FILE_MONTORING_TMP);
    int tab_cycle[PARAM_NB_LOOP];
    double tab_time[PARAM_NB_LOOP];

    long long int total_cycle = 0;
    double total_time = 0;
    long long int total_var_cycle = 0;
    double total_var_time = 0;
    for (int i = 0; i < PARAM_NB_LOOP; ++i) {
        res_file >> tab_cycle[i] >> tab_time[i];
        total_cycle += tab_cycle[i];
        total_time += tab_time[i];

        //Variance
        total_var_cycle += tab_cycle[i] * tab_cycle[i];
        total_var_time += tab_time[i] * tab_time[i];
    }

    long long int mean_cycle = total_cycle / PARAM_NB_LOOP;
    double mean_time = total_time / PARAM_NB_LOOP;

    long double var_cycle = total_cycle / PARAM_NB_LOOP - mean_cycle * mean_cycle;
    long double var_time = total_time / PARAM_NB_LOOP - mean_time * mean_time;

    double et_cycle = sqrt(var_cycle);
    double et_time = sqrt(var_time);

    cout << "Mean cycle         " << mean_cycle << endl;
    cout << "Mean time          " << mean_time << endl;
    cout << "var cycle          " << var_cycle << endl;
    cout << "var time           " << var_time << endl;
    cout << "Ecart type cycle   " << et_cycle << endl;
    cout << "Ecart type time    " << et_time << endl;

    int NbInstruction = mParameters->P_LOOP_SIZE * mParameters->P_OPERATIONS.size();
    float IPC = (float) NbInstruction / (float) mean_cycle;
    cout << "IPC                " << IPC << endl;


}

void Tool_freq_generators::Cpu_binding() {

    //We only bind the process if the user enterer -B parameter
    if (mParameters->P_BIND < 0) {
        return;
    }
//
//    int i = 0;
//    cpu_set_t mycpumask;
//
//    CPU_ZERO(&mycpumask); //Clears set, so that it contains no CPUs.
//    if (mParameters->P_BIND >= 0) {
//        CPU_SET(mParameters->P_BIND, &mycpumask); //Add CPU cpu to set
//        sched_setaffinity(0, sizeof(cpu_set_t), &mycpumask);
//    };
//    /* double-check */
//    sched_getaffinity(0, sizeof(cpu_set_t), &mycpumask);
//    for (i = 0; i < sysconf(_SC_NPROCESSORS_CONF); i++) {
//        if (CPU_ISSET(i, &mycpumask)) printf("+ Running on CPU #%d\n", i);
//    };
    return;
}


Tool_freq_generators::~Tool_freq_generators() {
//    fclose(P_FPC);
//    fclose(P_FPH);
}


