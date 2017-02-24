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
//    WC("#include <string.h>");
//    WC("#include <stdio.h>");
//    WC("#include <iostream>");
//    WC("#include <unistd.h>");
//    WC("using namespace std;");
//
//
//    WC("int main(int argc, char **argv) {");
//    WC("cout << \"coucou\\n\";");
//
//    generate_assembly();
//
//    WC("}");
//
//
//    /*

    stringstream tmpSS;
    string strNbIteration;
    tmpSS << BENCH_NB_ITERATION;
    tmpSS >> strNbIteration;
    string strTmp;

    WC("#include <string.h>");
    WC("#include <stdio.h>");
    WC("#include <iostream>");
    WC("#include <unistd.h>");
    WC("#include <stdint.h>");
    WC("using namespace std;");

    WC("uint64_t rdtsc() {");
    WC("uint32_t lo, hi;");
    WC("__asm__ __volatile__ (\"rdtsc\" : \"=a\" (lo), \"=d\" (hi));");
    WC("return (uint64_t) hi << 32 | lo;}");


    WC("int main(int argc, char **argv) {");
    WC("unsigned int time;");
    WC("uint64_t rtcstart, rtcend;");
    WC("int i;");
    WC("double ipc;");
    //On espere 10 cycles
//    strTmp = "for (i = 0; i < " +  strNbIteration  + " ; i++) {\n";
    strTmp = "for (i = 0; i < 10000000 ; i++) {\n";
    fprintf(P_FPC, strTmp.c_str());
    //Init register mm0 and mm1
    WC("__asm__ ( ");
    WC(" \"mov     $1, %%%%rax;\"");      //addition
    WC(" \"movq    %%%%rax, %%%%xmm1;\"");  //xmm1 = utilisé pour l'addition
    WC(" \"mov     $1, %%%%rax;\"");      //addition
    WC(" \"movq    %%%%rax, %%%%xmm0;\"");  //xmm1 = utilisé pour l'addition
    WC("::);");

    WC("rtcstart = rdtsc();");
    generate_assembly();
    WC("rtcend = rdtsc();");

    WC("}")
    WC("time = rtcend - rtcstart;");
    WC("cout << time << endl;");
    WC("return time;");
    WC("}");

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
        string saveSource = to_string(Get_register_source());
        string saveCible  = to_string(Get_register_cible());
        //v add p d
        string instruction = mPrefix + operation + mSuffix + mPrecision + " ";
        instruction += "%%%%" + mRegister_name + "0, ";
        instruction += "%%%%" + mRegister_name + saveSource + ", ";
        instruction += "%%%%" + mRegister_name + saveCible + "; ";
//        instruction += "%%%%" + mRegister_name + "2" + "; ";
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

int Tool_freq_generators::ExecuteAssembly(){
    DEBUG_PRINT("-- Execution the generated assembly file\n");

    FILE *lsofFile_p = popen("./" ASM_FILE_exe, "r");

    if (!lsofFile_p)
    {
        return -1;
    }

    char buffer[1024];
    char *line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
    pclose(lsofFile_p);
    return stoi (line_p);

}


Tool_freq_generators::~Tool_freq_generators() {
    fclose(P_FPC);
    fclose(P_FPH);

}
