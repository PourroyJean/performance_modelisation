//
// Created by Jean Pourroy on 19/12/2018.
//

#ifndef PERFORMANCE_MODELISATION_INPUTFILE_H
#define PERFORMANCE_MODELISATION_INPUTFILE_H

#include <string>
#include <vector>


//Template used to represent a file of different type of Line

template<class TYPE>
class InputFile {

    const std::string &m_file_path;     //Path of the file use
    std::vector<TYPE *> m_lines;        //Vector that contains all the line of the read file

public:

    InputFile(const std::string &m_file_path);

    const std::vector<TYPE *> &get_lines_vector() const {
        return m_lines;
    }

    void analysis();

    // ------ GETTERS  AND  SETTERS ----
    const std::string &get_file_path() const {
        return m_file_path;
    }

    const std::vector<TYPE *> &get_lines() const {
        return m_lines;
    }

    void set_lines(const std::vector<TYPE *> &lines) {
        InputFile::m_lines = lines;
    }
};

template<class TYPE>
std::ostream &operator<<(std::ostream &sortie, const InputFile<TYPE> &n);//


#include "InputFile.cpp"

#endif //PERFORMANCE_MODELISATION_INPUTFILE_H
