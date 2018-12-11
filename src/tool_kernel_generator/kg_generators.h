//
// Created by Jean Pourroy on 20/02/2017.
//

#ifndef PERFORMANCE_MODELISATION_KG_GENERATORS_H
#define PERFORMANCE_MODELISATION_KG_GENERATORS_H

#include <vector>
#include <fstream>
#include "kg_parameters.h"
#include "kg_misc.h"

class KG_generators {

private:
    void init_files();

    void generate_source();

    void generate_assembly();

    void generate_instructions();

    void parse_and_label_instructions();

    void parse_and_label_instructions_custom();

    void calcul_flop();

    int Get_register_source();

    int Get_register_cible();

    std::ofstream mFile_assembly_src   ;
    std::ifstream mFile_template_start ;
    std::ifstream mFile_template_end   ;
    std::ifstream mFile_template_freq   ;



public:
    KG_parameters *mParameters;
    std::string mRegister_name;
    std::vector <string> mRegister_list;
    unsigned mPrevious_target_register;
    unsigned mRegister_max=0;
    std::string mPrefix;
    std::vector<std::string> *mOperations_set   = new std::vector<std::string>();
    std::vector<std::string> *mInstructions_set = new std::vector<std::string>();
    std::string mSuffix;
    std::string mPrecision;
    int mFLOP_SP;
    int mFLOP_DP;

    void Generate_code();

    KG_generators(KG_parameters *param);

    virtual ~KG_generators();
};


#endif //PERFORMANCE_MODELISATION_KG_GENERATORS_H
