//
// Created by Jean Pourroy on 19/12/2018.
//

#ifndef PERFORMANCE_MODELISATION_INPUTFILE_H
#define PERFORMANCE_MODELISATION_INPUTFILE_H

#include <string>
#include <vector>

template<class TYPE>
class InputFile {

    const std::string &m_file_path;
    int m_line_counter; //The current line in the file

public:

    std::vector<TYPE *> m_lines;

    InputFile(const std::string &m_file_path);

    const vector<TYPE *> &getM_lines() const {
        return m_lines;
    }


//    friend std::ostream &operator<<(std::ostream &sortie, const InputFile<TYPE> &n);//

};

template<class TYPE>
std::ostream &operator<<(std::ostream &sortie, const InputFile<TYPE> &n);//


#include "InputFile.cpp"

#endif //PERFORMANCE_MODELISATION_INPUTFILE_H
