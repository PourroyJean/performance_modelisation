//
// Created by Jean Pourroy on 20/02/2017.
//

#include <iostream>
#include "tool_freq_generators.h"
#include "tool_freq_misc.h"



//Write to Cpp or Header file
#define WC(s) fprintf(P_FPC, s "\n");
#define WH(s) fprintf(P_FPH, s "\n");


void Tool_freq_generators::generate_assembly() {
    WC("__asm__ (\"myBench: \" ");
    for (auto instruction: * mInstructions_set){
        const char * s = instruction.c_str();
        fprintf(P_FPC, "\"");
        fprintf(P_FPC, instruction.c_str());
        fprintf(P_FPC, "\"\n");

    }
//    WC("\"       sub    $0x1, %%eax;\"");
    fprintf(P_FPC, "\"  sub    $0x1, %%%%eax;\"\n");
    WC("\"  jnz    myBench\" : : \"a\" (10000)");
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

void Tool_freq_generators::generate_instructions() {
    DEBUG_PRINT("-- Generating instructions vector\n");

    mInstructions_set->clear();

    for (auto operation : *mOperations_set) {
        //v add p d
        string instruction = mPrefix + operation + mSuffix + mPrecision + " ";

        instruction += "%%%%"  + mRegister_name + "0, ";
        instruction += "%%%%"  + mRegister_name + "1, ";
        instruction += "%%%%"  + mRegister_name + "2; ";



        mInstructions_set->push_back(instruction);
    }

}



void Tool_freq_generators::Init_Generator(Tool_freq_parameters *p) {
    DEBUG_PRINT("-- Init Generator register, prefix, suffix, and precision \n");
    if (p->P_WIDTH == 64 || p->P_WIDTH == 128){
        mRegister_name = "xmm";
    }
    if (p->P_WIDTH == 256){
        mRegister_name = "ymm";
    }
    if (p->P_WIDTH == 512){
        mRegister_name = "zmm";
    }
    //only v instructions supported
    mPrefix = "v";
    if ( p->P_WIDTH == 64){
        mSuffix = "s";
    }
    else {
        mSuffix = "p";
    }
    if (!p->P_PRECISION.compare("single")){
        mPrecision = "s";
    } else {
        mPrecision = "d";
    }

    for (auto op: p->P_OPERATIONS) {
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


string Tool_freq_generators::Generate_code(Tool_freq_parameters *param) {
    DEBUG_PRINT("Generating assembly...\n");


    //----- INITIALISATION -----------
    Init_Generator(param);

    //Generate each final instruction
    generate_instructions();

//    for (auto i: *mInstructions_set) {
//        cout << i << endl;
//    }

    generate_source();
    generate_header();

    fflush(P_FPC);
    fflush(P_FPH);


    return "";
}

Tool_freq_generators::Tool_freq_generators() {
    P_FPC = fopen("assembly_generated.cpp", "w+");
    P_FPH = fopen("assembly_generated.h", "w+");

    mRegister_name = "xmm";
    mNext_register= 0;
    mPrefix = "v";
    mOperations_set = new vector<string>();
    mSuffix = "s";
    mPrecision = "d";
}

Tool_freq_generators::~Tool_freq_generators() {
    fclose(P_FPC);
    fclose(P_FPH);

}
