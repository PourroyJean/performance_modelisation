//
// Created by Jean Pourroy on 20/02/2017.
//

#ifndef PERFORMANCE_MODELISATION_TOOL_FREQ_GENERATORS_H
#define PERFORMANCE_MODELISATION_TOOL_FREQ_GENERATORS_H

#import "tool_freq_parameters.h"

class Tool_freq_generators {

private:
    void init_files();
    void generate_source();
    void generate_header();

public:
    string P_register_name;
    unsigned P_next_register = 0;

    FILE * P_FPC;
    FILE * P_FPH;




    string generate_assembly(Tool_freq_parameters * t);

    Tool_freq_generators();

    virtual ~Tool_freq_generators();
};


#endif //PERFORMANCE_MODELISATION_TOOL_FREQ_GENERATORS_H
