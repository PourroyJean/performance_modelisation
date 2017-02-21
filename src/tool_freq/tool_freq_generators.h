//
// Created by Jean Pourroy on 20/02/2017.
//

#ifndef PERFORMANCE_MODELISATION_TOOL_FREQ_GENERATORS_H
#define PERFORMANCE_MODELISATION_TOOL_FREQ_GENERATORS_H

#include <vector>
#import "tool_freq_parameters.h"

class Tool_freq_generators {

private:
    void init_files();

    void generate_source();

    void generate_header();

    void generate_assembly();

    void generate_instructions();

    void Init_Operation_set(Tool_freq_parameters *t);

    void Init_Generator(Tool_freq_parameters *t);

    FILE *P_FPC;
    FILE *P_FPH;

public:
    string mRegister_name;
    unsigned mNext_register;
    string mPrefix;
    std::vector<std::string> *mOperations_set = new vector<string>();
    std::vector<std::string> *mInstructions_set = new vector<string>();

    string mSuffix;
    string mPrecision;


    string Generate_code(Tool_freq_parameters *t);

    Tool_freq_generators();

    virtual ~Tool_freq_generators();
};


#endif //PERFORMANCE_MODELISATION_TOOL_FREQ_GENERATORS_H
