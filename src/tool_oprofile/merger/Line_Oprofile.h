//
// Created by Jean Pourroy on 19/12/2018.
//

#ifndef PERFORMANCE_MODELISATION_LINE_OPROFILE_H
#define PERFORMANCE_MODELISATION_LINE_OPROFILE_H


#include "Line.h"

class Line_Oprofile : public Line {
    string m_symbole_name;
    string m_application_name;

public:
    Line_Oprofile(const string &m_original_line);

    virtual void analyse_current_line();
};


#endif //PERFORMANCE_MODELISATION_LINE_OPROFILE_H
