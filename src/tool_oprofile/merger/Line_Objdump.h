//
// Created by Jean Pourroy on 19/12/2018.
//

#ifndef PERFORMANCE_MODELISATION_PAP_LINE_OBJDUMP_H
#define PERFORMANCE_MODELISATION_PAP_LINE_OBJDUMP_H

#include <unordered_map>
#include "string"
#include "Line.h"
#include "InputFile.h"

using namespace std;

class Line_Objdump : public Line {
protected:
    string m_assembly_instruction;

public:
    Line_Objdump(string line);

    static InputFile<Line_Objdump> m_file;
    static int LINE_COUNTER;
    static int LAST_FCT_CTR;
    static string LAST_SYMB;    // Name of the last symbol fetched
//    friend std::ostream &operator<<(std::ostream &sortie, const Line_Objdump & l);//

    virtual void analyse_current_line() override;

    uint64_t get_address();


// map address of instructions of OBJECT FILE to line inside objdump_file
// used for branch instruction to recover the line of the branch
    static std::unordered_map<uint64_t, int> objdump_address;

    void print(std::ostream &sortie) const override;
};


#endif //PERFORMANCE_MODELISATION_PAP_LINE_OBJDUMP_H
