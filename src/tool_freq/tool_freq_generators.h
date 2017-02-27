//
// Created by Jean Pourroy on 20/02/2017.
//

#ifndef PERFORMANCE_MODELISATION_TOOL_FREQ_GENERATORS_H
#define PERFORMANCE_MODELISATION_TOOL_FREQ_GENERATORS_H

#include <vector>
#include "tool_freq_parameters.h"

class Tool_freq_generators {

private:
    void init_files();

    void generate_source();

    void generate_header();

    void generate_assembly();

    void generate_instructions();

    void Init_Generator();

    int Get_register_source();

    int Get_register_cible();


    FILE *P_FPC;
    FILE *P_FPH;

public:
    Tool_freq_parameters *mParameters;
    string mRegister_name;
    unsigned mPrevious_target_register;
    string mPrefix;
    std::vector<std::string> *mOperations_set = new vector<string>();
    std::vector<std::string> *mInstructions_set = new vector<string>();
    string mSuffix;
    string mPrecision;
    int mExecutionCycle;
    double mExecutionFrequency;
    string AssemblyFileName;

    void ExecuteAssembly();

    void Cpu_binding ();


    string Generate_code();

    Tool_freq_generators(Tool_freq_parameters *param);

    virtual ~Tool_freq_generators();
};


#endif //PERFORMANCE_MODELISATION_TOOL_FREQ_GENERATORS_H
