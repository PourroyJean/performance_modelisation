//
// Created by Jean Pourroy on 20/02/2017.
//
#include <stdio.h>
#include <iostream>
#include "kg_generators.h"
#include <misc.h>


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


using namespace std;


void KG_generators::generate_assembly() {


    mFile_assembly_src << "\t\t__asm__ (\"\" " << endl;

    string smallestRegisterName = "xmm";

    //TODO : remetre l'initialisation des registre xmm
    //-- KERNEL INIT
    mFile_assembly_src << "\t\t //Initialisation opérandes à 1\n";
//    mFile_assembly_src << "\t\t \"mov     $1,    %%rbx; \"\n";
//    mFile_assembly_src << "\t\t \"movq    %%rbx, %%" << smallestRegisterName << "0;\"" << "  //operand 1;\n";
//    mFile_assembly_src << "\t\t \"movq    %%rbx, %%" << smallestRegisterName << "1;\"" << "  //operand 2;\n\n";

    if (mParameters->P_COUNT) {
        mFile_assembly_src << "\t\t //Initialisation compteur\n";
        mFile_assembly_src << "\t\t \"mov     $1,    %%rbx; \"\n";
        for (int i = 2; i <= 15; ++i) {
            mFile_assembly_src << "\t\t \"movq    %%rbx, %%" << smallestRegisterName << "" << i << ";\"\n";
        }
        mFile_assembly_src << endl;
    }


    //-- KERNEL GENERATION: loop generation
    mFile_assembly_src << "\t\t \"myBench: \" " << endl;
    for (int i = 0; i < mParameters->P_UNROLLING; ++i) {
        for (auto instruction: *mInstructions_set) {
            mkernel_assembly_src << "\t\t\t\t\"" << instruction << "\"\n";
        }
    }
    mFile_assembly_src << mkernel_assembly_src.str();

    mFile_assembly_src << "\t\t\"sub  $0x1, %%eax;\"\n";
    mFile_assembly_src << "\t\t\"jnz  myBench;\"";

    //-- ONLY IF WE SELF CHECK
    if (mParameters->P_COUNT) {
        mFile_assembly_src << "\n\n\t\t//Réduction: dommer le nombre d'addition total dans xmm0\n";
        mFile_assembly_src << "\t\t \"mov     $0,    %%rbx; \"\n";
        mFile_assembly_src << "\t\t \"movq    %%rbx, %%xmm0;\"\n";
        for (int i = 2; i <= mRegister_max; ++i) {
            mFile_assembly_src << "\t\t \"vaddpd  %%" << smallestRegisterName << "0, %%" << smallestRegisterName << i
                               << ", %%xmm0;\"\n";
        }
        mFile_assembly_src << "\t\t \"movd  %%xmm0, %0;  //Final result \"\n";
        mFile_assembly_src << endl;
    }

    mFile_assembly_src
            << "\t\t: \"=r\" (instructions_executed) "
            << ": \"a\" (NB_lOOP_IN));" << endl;



}


void KG_generators::generate_source() {
    mFile_assembly_src << "#define TMP_FILE_monitoring \"" + FILE_MONTORING_TMP + "\"\n";
    mFile_assembly_src << "int NB_lOOP = " << mParameters->P_SAMPLES << ";\n";
    mFile_assembly_src << "int NB_lOOP_IN = " << mParameters->P_LOOP_SIZE << ";\n";
    mFile_assembly_src << "int NB_INST = " << mParameters->P_OPERATIONS.length() << ";\n";
    mFile_assembly_src << "int P_COUNT = " << mParameters->P_COUNT << ";\n";
    mFile_assembly_src << "int P_UNROLLING = " << mParameters->P_UNROLLING << ";\n";
    mFile_assembly_src << "int CPU_BIND = " << mParameters->P_BIND << ";\n";
    mFile_assembly_src << "int FLOP_SP_PER_LOOP = " << mFLOP_SP << ";\n";
    mFile_assembly_src << "int FLOP_DP_PER_LOOP = " << mFLOP_DP << ";\n";
    mFile_assembly_src << "bool is_check_freq = " << std::boolalpha << mParameters->P_FREQUENCY << ";\n";


    //Template_START + LOOP_ASSEMBLY + Template_END
    mFile_assembly_src << mFile_template_start.rdbuf();
    generate_assembly();
    mFile_assembly_src << mFile_template_end.rdbuf();
    mFile_assembly_src << mFile_template_freq.rdbuf();


    mFile_template_start.close();
    mFile_template_end.close();
    mFile_template_freq.close();
    mFile_assembly_src.close();
}


int KG_generators::Get_register_source() {
    if (mParameters->P_DEPENDENCY) {
        return mPrevious_target_register;
    } else {
        return 1;
    }
}

int KG_generators::Get_register_cible() {
    if (mPrevious_target_register == mMAX_REGISTER - 1) { //max is 32 registers
        mPrevious_target_register = 2;
    } else {
        mPrevious_target_register++;
    }

    return mPrevious_target_register;
}


void KG_generators::KG_generate_table_registers() {

    //generates three register buffers for constructing instructions.
    //The first table contains only register 0,
    //The second source depends on whether or not the dependency option is used.
    int size_register_vector = mOperations_set->size() < mMAX_REGISTER - 1 ? mOperations_set->size() : mMAX_REGISTER -2;

    mTableRegisterSource1 = new std::vector<int>(size_register_vector);
    mTableRegisterSource2 = new std::vector<int>(size_register_vector);
    mTableRegisterCible = new std::vector<int>(size_register_vector);


    //generates the three lists of source and destination registers for instructions
    for (int i = 0; i < size_register_vector; ++i) {
        mTableRegisterSource1->at(i) = 0;

        if (mParameters->P_DEPENDENCY) {
            mTableRegisterSource2->at(i) = i + 2;
        } else {
            mTableRegisterSource2->at(i) = 1;
        }

        mTableRegisterCible->at(i) = i + 2;
    }

    //the rotation of the source register table allows to generate N chain of dependencies
    leftRotate(mTableRegisterSource2, mParameters->P_NB_DEPENDENCY, size_register_vector);


    DEBUG << mTableRegisterSource1->size() << " ok ok \n";
    DEBUG << " Source 1 - [";
    for (auto a : *mTableRegisterSource1) { DEBUG << a << ", "; }
    DEBUG << "]\n  Source 2 - [";
    for (auto a : *mTableRegisterSource2) { DEBUG << a << ", "; }
    DEBUG << "]\n Cible   -  [";
    for (auto a : *mTableRegisterCible) { DEBUG << a << ", "; }
    DEBUG << "]\n";
}


void KG_generators::generate_instructions() {
    DEBUG << "-- Generating instructions vector\n";


    mInstructions_set->clear();
    mPrevious_target_register = 1;

    int instruction_index_register = 0;

    KG_generate_table_registers();

    DEBUG << "mOperations_set " << mOperations_set->size() << endl;

    for (auto &operation : *mOperations_set) {
        string saveSource = to_string(Get_register_source());
        string saveCible = to_string(Get_register_cible());
        instruction_index_register = (instruction_index_register) % (mMAX_REGISTER - 2);

        //v add p d
        string instruction = mPrefix + operation + mSuffix + mPrecision + " ";
        //instruction = "vdivpd ";    #generate division todo

        // 3 registers
        instruction += "%%" + mRegister_name + to_string(mTableRegisterSource1->at(instruction_index_register)) + ", ";
        instruction += "%%" + mRegister_name + to_string(mTableRegisterSource2->at(instruction_index_register)) + ", ";
        instruction += "%%" + mRegister_name + to_string(mTableRegisterCible->at(instruction_index_register)) + "; ";


        mInstructions_set->push_back(instruction);
        instruction_index_register++;

        DEBUG << instruction << endl;
    }

}


void KG_generators::parse_and_label_instructions() {
    DEBUG << "-- Init Generator register, prefix, suffix, and precision \n";

    //only [v]addpd instructions supported
    mPrefix = "v";

    int flop_per_inst = 0;
    int size_register = 0;

    //vaddp[s,d]
    if (!mParameters->P_PRECISION.compare("single")) {
        mPrecision = "s";
        size_register = 32;
    } else {
        mPrecision = "d";
        size_register = 64;
    }

    if (mParameters->P_WIDTH == 64) {
        //Scalar
        mSuffix = "s";
        flop_per_inst = 1;
    } else {
        mSuffix = "p";
        flop_per_inst = mParameters->P_WIDTH / size_register;
    }

    //Regster used involved the type of instruction used
    if (mParameters->P_WIDTH == 64 || mParameters->P_WIDTH == 128) {
        mRegister_name = "xmm";
    }
    if (mParameters->P_WIDTH == 256) {
        mRegister_name = "ymm";
    }
    if (mParameters->P_WIDTH == 512) {
        mRegister_name = "zmm";
    }


    int flop = 0;
    //generate the instructions vector
    for (auto op: mParameters->P_OPERATIONS) {
        if (op == 'a') {
            mOperations_set->push_back("add");
            flop += flop_per_inst;
        } else if (op == 'm') {
            mOperations_set->push_back("mul");
            flop += flop_per_inst;
        } else if (op == 'f') {
            mOperations_set->push_back("fmadd231");
            flop += flop_per_inst * 2;
        } else {
            cout << "ERROR PARSING INSTRUCTION";
            exit(1);
        }
    }

    //If dependy is set, calculate the last register used to loop with the first instruction
    if (mParameters->P_DEPENDENCY) {
        if (mParameters->P_OPERATIONS.length() < mMAX_REGISTER - 1) {
            mLast_register = (mParameters->P_OPERATIONS.length() + 1) % mMAX_REGISTER;
        } else if ((mParameters->P_OPERATIONS.length()) % (mMAX_REGISTER - 2) == 0) {
            mLast_register = mMAX_REGISTER - 1;
        } else {
            mLast_register = ((mParameters->P_OPERATIONS.length()) % (mMAX_REGISTER - 2)) + 1;
        }
    }

    flop *= mParameters->P_UNROLLING;

    if (!mParameters->P_PRECISION.compare("single")) {
        mFLOP_SP = flop;
    } else {
        mFLOP_DP = flop;
    }

}


void KG_generators::parse_and_label_instructions_custom() {
    DEBUG << "-- Init Generator register, prefix, suffix, and precision for custom width \n";

    //only [v]addpd instructions supported
    mPrefix = "v";

    if (!mParameters->P_PRECISION.compare("single")) {
        mPrecision = "s";
    } else {
        mPrecision = "d";
    }

    for (int i = 0; i < mParameters->P_OPERATIONS.size(); ++i) {

        char op = mParameters->P_OPERATIONS[i];
        int width = (*mParameters->P_WIDTH_CUSTOM)[i];
        DEBUG << "_op_ " << op << " " << width << endl;


        if (op == 'a') {
            mOperations_set->push_back("add");
        } else if (op == 'm') {
            mOperations_set->push_back("mul");
        } else if (op == 'f') {
            mOperations_set->push_back("fmadd231");
        } else {
            cout << "ERROR PARSING INSTRUCTION";
            exit(1);
        }


        if (width == 64 || width == 128 || width == 1 || width == 2) {
            mRegister_list.push_back("xmm");
        }
        if (width == 256 || width == 3) {
            mRegister_list.push_back("ymm");
        }
        if (width == 512 || width == 4) {
            mRegister_list.push_back("zmm");
        }
    }


    if (mParameters->P_WIDTH == 64) {
        //Scalar
        mSuffix = "s";
    } else {
        mSuffix = "p";
    }


    int flop = 0;

    flop *= mParameters->P_UNROLLING;

    if (!mParameters->P_PRECISION.compare("single")) {
        mFLOP_SP = flop;
    } else {
        mFLOP_DP = flop;
    }


    DEBUG << "Operation list: \n";
    for (auto o: *mOperations_set) {
        DEBUG << o << endl;
    }


    DEBUG << "Register list: \n";
    for (auto r: mRegister_list) {
        DEBUG << r << endl;
    }

}


void KG_generators::Generate_code() {
    DEBUG << "-- Generating assembly...\n";


    (mParameters->P_WIDTH_CUSTOM) ?
    parse_and_label_instructions_custom() :
    parse_and_label_instructions();

    generate_instructions();

    generate_source();


    return;
}

void KG_generators::print_assembly_kernel () const{
    cout << mkernel_assembly_src.str();
    return;
}

KG_generators::KG_generators(KG_parameters *param) :
        mParameters(param),
        mRegister_name("xmm"),
        mPrevious_target_register(1),
        mPrefix("v"),
        mSuffix("s"),
        mPrecision("d"),
        mFLOP_SP(0),
        mFLOP_DP(0) {

    mOperations_set = new vector<string>();
    mFile_template_start.open(FILE_TEMPLATE_START, std::ios_base::binary);
    mFile_template_end.open(FILE_TEMPLATE_END, std::ios_base::binary);
    mFile_template_freq.open(FILE_TEMPLATE_FREQ, std::ios_base::binary);
    mFile_assembly_src.open(FILE_ASM_SOURCE_GENERATED, std::ios_base::binary);


    if (!(mFile_assembly_src.is_open() && mFile_template_end.is_open() && mFile_template_start.is_open() &&
          mFile_template_freq.is_open())) {
        cerr << "Error opening one of these files: \n";
        cerr << FILE_TEMPLATE_START << endl;
        cerr << FILE_TEMPLATE_END << endl;
        cerr << FILE_TEMPLATE_FREQ << endl;
        cerr << FILE_ASM_SOURCE_GENERATED << endl;
        exit(0);
    }


}


KG_generators::~KG_generators() {
}


