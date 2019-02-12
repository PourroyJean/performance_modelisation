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

protected:
    std::string m_application_name;             // Related application name of the line
    int m_function_line;                        // Index of the begginning of the function whom it belongs

    static int LINE_COUNTER;                    // Count the current number of line read
    static int LINE_LAST_FCT;                   // Save the index of the last function found

    static InputFile<Line_Oprofile> *FILE_OPR;  // Save all the lines in a vectore implemented with the InputFile class

public:

    Line_Oprofile(const std::string &m_original_line);

    void analyse_current_line() override;

    //Exact and convert the hexa address from the line read to decimal
    void extract_address();

    //Extract and update the counters variable from the line read
    void extract_counters();

    friend std::ostream &operator<<(std::ostream &sortie, Line_Oprofile &l);

    // map address of instructions of OBJECT FILE to line inside oprofile_file
    // used for branch instruction to recover the line of the branch
    static std::unordered_map<uint64_t, int> oprofile_address;

    //When the analysis of the file is done, a resum of the biggest function can be print
    static void print_resume();

    // ------ GETTERS  AND  SETTERS ----

    static InputFile<Line_Oprofile> *getFILE_OPR();

    static void setFILE_OPR(InputFile<Line_Oprofile> *FILE_OPR);

    const std::string &get_application_name() const;

    void set_application_name(const std::string &m_application_name);
};

#endif //PERFORMANCE_MODELISATION_LINE_OPROFILE_H
