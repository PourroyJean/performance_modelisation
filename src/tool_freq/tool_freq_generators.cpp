//
// Created by Jean Pourroy on 20/02/2017.
//
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "tool_freq_generators.h"
#include "tool_freq_misc.h"



//Write to Cpp or Header file
#define WC(s) fprintf(P_FPC, s "\n");
#define WH(s) fprintf(P_FPH, s "\n");


void Tool_freq_generators::generate_assembly() {
    stringstream tmpSS;
    string strNbIteration;
    tmpSS << BENCH_NB_ITERATION;
    tmpSS >> strNbIteration;

    WC("__asm__ (\"myBench: \" ");
    for (auto instruction: *mInstructions_set) {
        const char *s = instruction.c_str();
        fprintf(P_FPC, "\t\t\"");
        fprintf(P_FPC, instruction.c_str());
        fprintf(P_FPC, "\"\n");

    }
//    WC("\"       sub    $0x1, %%eax;\"");
    fprintf(P_FPC, "\"  sub    $0x1, %%%%eax;\"\n");
    string jumpLine = "\"  jnz    myBench\" : : \"a\" (" + strNbIteration + ")";
    fprintf(P_FPC, jumpLine.c_str());
    WC(");");
}


void Tool_freq_generators::generate_source() {
//    WC("#include \"assembly_generated.h\"");
    WC("#include <string.h>");
    WC("#include <stdio.h>");
    WC("#include <iostream>");
    WC("#include <unistd.h>");
    WC("using namespace std;");
    WC("int main(int argc, char **argv) {");
    WC("cout << \"coucou\";");

    generate_assembly();


    WC("}")
    WC("")

}

void Tool_freq_generators::generate_header() {
    WH("#ifndef ASSEMBLY_GENERATED_H")
    WH("#define ASSEMBLY_GENERATED_H")
    WH("void assembly_function ();")
    WH("#endif //ASSEMBLY_GENERATED_H")


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
        //v add p d
        string instruction = mPrefix + operation + mSuffix + mPrecision + " ";
        instruction += "%%%%" + mRegister_name + "0, ";
        instruction += "%%%%" + mRegister_name + to_string(Get_register_source()) + ", ";
        instruction += "%%%%" + mRegister_name + to_string(Get_register_cible()) + "; ";
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
            mOperations_set->push_back("fma");
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
    generate_header();

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

Tool_freq_generators::~Tool_freq_generators() {
    fclose(P_FPC);
    fclose(P_FPH);

}
