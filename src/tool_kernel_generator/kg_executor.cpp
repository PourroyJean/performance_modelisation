#include <iostream>
#include "kg_parameters.h"
#include "kg_misc.h"
#include "kg_generators.h"
#include "kg_executor.h"
#include <cmath>


using namespace std;

KG_executor::KG_executor (KG_generators * generator){
    mGenerator = generator;
    mParameters = generator->mParameters;
}


void KG_executor::Execute_assembly() {
    DEBUG_PRINT("-- Execution the generated assembly file\n");

    //We let the kernel bind the process himself if no binding are set
    Cpu_binding();

    string stmp(FILE_ASM_EXE);
    system(stmp.c_str());
    return;

}


void KG_executor::Monitor_Execution() {
    ifstream res_file(FILE_MONTORING_TMP);
    int tab_cycle[mParameters->P_SAMPLES];
    double tab_time[mParameters->P_SAMPLES];

    long long int total_cycle = 0;
    double total_time = 0;
    long long int total_var_cycle = 0;
    double total_var_time = 0;
    for (int i = 0; i < mParameters->P_SAMPLES; ++i) {
        res_file >> tab_cycle[i] >> tab_time[i];
        total_cycle += tab_cycle[i];
        total_time += tab_time[i];

        //Variance
        total_var_cycle += tab_cycle[i] * tab_cycle[i];
        total_var_time += tab_time[i] * tab_time[i];
    }

    long long int mean_cycle = total_cycle / mParameters->P_SAMPLES;
//    long double var_cycle = total_cycle / mParameters->P_SAMPLES - mean_cycle * mean_cycle;

    double mean_time = total_time / mParameters->P_SAMPLES;
//    long double var_time = total_time / mParameters->P_SAMPLES - mean_time * mean_time;

    double ecart_time  = 0;
    double ecart_cycle = 0;
    for (int i = 0; i < mParameters->P_SAMPLES; ++i) {
        ecart_cycle += abs (tab_cycle[i] - mean_cycle);
        ecart_time  += abs (tab_time[i]  - mean_time);

    }
    ecart_cycle /= mParameters->P_SAMPLES;
    ecart_time  /= mParameters->P_SAMPLES;
//    double et_cycle = sqrt(var_cycle);
//    double et_time = sqrt(var_time);

    int NbInstruction = mParameters->P_LOOP_SIZE * mParameters->P_OPERATIONS.size();
    float IPC = (float) NbInstruction / (float) mean_cycle;

    double frequency = (mean_cycle / mean_time )/1000000000;



    cout << "Mean cycle         " << mean_cycle << endl;
    cout << "Ecart cycle        " << ecart_cycle << endl;
    cout << "Mean time          " <<fixed << mean_time << endl;
    cout << "Ecart time         " << ecart_time << endl;
    cout << "----------------------------------------" << endl;
    cout << "Frequency          " << frequency << endl;
    cout << "IPC                " << IPC << endl;
    cout << "----------------------------------------" << endl;

    if(mParameters->P_GRAPH){
        Graphical_Output();
    }


}

void KG_executor::Graphical_Output() {
    string stmp(FILE_SCRIPT_REPARTITION);
    int nbRect = PARAM_SAMPLES / 10;
    string exe = stmp + " " + FILE_MONTORING_TMP + " " +  to_string(nbRect);
    system(exe.c_str());
}

void KG_executor::Cpu_binding() {

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
