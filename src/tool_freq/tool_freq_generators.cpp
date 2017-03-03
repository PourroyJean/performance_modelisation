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



//Write to Cpp or Header file
#define WC(s) fprintf(P_FPC, s "\n");
#define WH(s) fprintf(P_FPH, s "\n");


void Tool_freq_generators::generate_assembly() {
    string strNbIteration = to_string(mParameters->P_LOOP_SIZE);
    //Generate the assembly loop executed BENCH_NB_ITERATION
    WC("__asm__ (\"myBench: \" ");
    for (auto instruction: *mInstructions_set) {
        const char *s = instruction.c_str();
        fprintf(P_FPC, "\t\t\"");
        fprintf(P_FPC, instruction.c_str());
        fprintf(P_FPC, "\"\n");
    }
    fprintf(P_FPC, "\"  sub    $0x1, %%%%eax;\"\n");
    string jumpLine = "\"  jnz    myBench\" : : \"a\" (" + strNbIteration + ")";
    fprintf(P_FPC, jumpLine.c_str());
    WC(");");
}


void Tool_freq_generators::generate_source() {

    //Generate include needed by the generated program
    WC("#include <string.h>");
    WC("#include <stdio.h>");
    WC("#include <iostream>");
    WC("#include <unistd.h>");
    WC("#include <stdint.h>");
    WC("#include <iomanip>");
    WC("using namespace std;");


    //Function rdtsc to monitor the cycle count
    WC("uint64_t rdtsc() {");
    WC("uint32_t lo, hi;");
    WC("__asm__ __volatile__ (\"rdtsc\" : \"=a\" (lo), \"=d\" (hi));");
    WC("return (uint64_t) hi << 32 | lo;}");

    //Gettimeofday homemade
    WC("#include <sys/time.h>");
    WC("double mygettime() {");
    WC("    struct timeval tp;");
    WC("    struct timezone tzp;");
    WC("    int i;");
    WC("    i = gettimeofday(&tp, &tzp);");
    WC("    return ((double) tp.tv_sec + (double) tp.tv_usec * 1.e-6);");
    WC("}");

    WC("int main(int argc, char **argv) {");
    WC("unsigned int nbCycleIn;");
    WC("uint64_t cycleInStart, cycleInEnd;");
    WC("double timeStart, timeEnd, timeSpent;")
    WC("double ipc;");

    //Boucle de warming
    WC("for (int i = 0; i < 1000000 ; i++) {");

    //TODO needed ? Init register mm0 and mm1 etc...
//    WC("__asm__ ( ");
//    WC(" \"mov     $1, %%%%rax;\"");        //addition
//    WC(" \"movq    %%%%rax, %%%%xmm0;\"");  //xmm0 = utilisé pour l'addition
//    WC(" \"mov     $1, %%%%rax;\"");        //addition
//    WC(" \"movq    %%%%rax, %%%%xmm1;\"");  //xmm1 = utilisé pour l'addition
//    WC("::);");

    // --------------- ASSEMBLY GENERATION OF THE CODE ------------
    WC("timeStart   = mygettime();");
    WC("cycleInStart = rdtsc();");
    generate_assembly();
    WC("cycleInEnd = rdtsc();");
    WC("timeEnd     = mygettime();");
    WC("}")


    //We extract the cycle count and the elapsed time to calculate the IPC and the frequency
    WC("nbCycleIn = cycleInEnd - cycleInStart;");
    WC("timeSpent = timeEnd - timeStart;");
    WC("unsigned int freq = nbCycleIn / (1000000 * (timeEnd - timeStart));");
    WC("cout << nbCycleIn << \' \' << fixed << setprecision(3) << freq << endl;");
    WC("return 0;");
    WC("}");

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

    fflush(P_FPC);
    fflush(P_FPH);


    return "";
}

Tool_freq_generators::Tool_freq_generators(Tool_freq_parameters *param) {
    P_FPC = fopen("assembly_generated.cpp", "w+");
    P_FPH = fopen("assembly_generated.h", "w+");

    mParameters = param;
    mRegister_name = "xmm";
    mPrevious_target_register = 1;
    mPrefix = "v";
    mOperations_set = new vector<string>();
    mSuffix = "s";
    mPrecision = "d";
}

void Tool_freq_generators::ExecuteAssembly(){
    DEBUG_PRINT("-- Execution the generated assembly file\n");

    //We let the kernel bind the process himself if no binding are set
    Cpu_binding();

    FILE *lsofFile_p = popen("./" ASM_FILE_exe, "r");

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
    fclose(P_FPC);
    fclose(P_FPH);
}


