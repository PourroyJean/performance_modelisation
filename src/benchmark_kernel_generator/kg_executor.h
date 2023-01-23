//
// Created by Jean Pourroy on 21/03/2017.
//

#ifndef __kg_executor_H__
#define __kg_executor_H__


#include <vector>
#include <fstream>
#include "kg_parameters.h"
#include "kg_misc.h"
#include "kg_generators.h"


class KG_executor {

private:
    KG_generators *mGenerator;
    KG_parameters *mParameters;

    void Cpu_binding();


public:

    KG_executor(KG_generators *generator);

    void Execute_assembly();

    void Monitor_Execution();

    void Graphical_Output();


};


#endif //__kg_executor_H__
