//
// Created by Jean Pourroy on 19/12/2018.
//

#ifndef PERFORMANCE_MODELISATION_LINE_OPROFILE_H
#define PERFORMANCE_MODELISATION_LINE_OPROFILE_H


#include "Line.h"
#include "InputFile.h"
#include <string>
#include <unordered_map>

class Line_Oprofile : public Line {

    std::string m_symbole_name;
    std::string m_application_name;
    int m_function_line;

public:
    Line_Oprofile(const string &m_original_line);

    static int LINE_COUNTER;
    static int LAST_FCT_CTR;

    virtual void analyse_current_line();

    void get_address_and_ctrs();

    static InputFile<Line_Oprofile> *FILE_OPR;

    // map address of instructions of OBJECT FILE to line inside oprofile_file
    // used for branch instruction to recover the line of the branch
    static std::unordered_map<uint64_t, int> oprofile_address;

    void print(std::ostream &sortie) const override;

};


#endif //PERFORMANCE_MODELISATION_LINE_OPROFILE_H
