//
// Created by Jean Pourroy on 21/03/2017.
//

#ifndef __tool_freq_executor_H__
#define __tool_freq_executor_H__


#include <vector>
#include <fstream>
#include "tool_freq_parameters.h"
#include "tool_freq_misc.h"
#include "tool_freq_generators.h"


class Tool_freq_executor {

private:
    Tool_freq_generators *mGenerator;
    Tool_freq_parameters *mParameters;

    void Cpu_binding();


public:

    Tool_freq_executor(Tool_freq_generators *generator);

    void Execute_assembly();

    void Monitor_Execution();

    void Graphical_Output();


};


#endif //__tool_freq_executor_H__
