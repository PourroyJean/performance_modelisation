//
// Created by Jean Pourroy on 19/12/2018.
//


#ifdef PERFORMANCE_MODELISATION_INPUTFILE_H
#include "InputFile.h"
#include "vector"
#include <string>
#include <iostream>
#include <fstream>      // std::ifstream


template <class T>
InputFile<T>::InputFile(const string &m_file_path) :  m_file_path(m_file_path) {


    std::ifstream fobj(m_file_path);
    assert(fobj.is_open());
    while (1) {
        string line;
        getline(fobj, line);
        if (fobj.eof())
            break;
        assert(!fobj.fail() && !fobj.bad());
        m_lines.push_back(new T(line));
    }
    fobj.close();

}

template <class T>
ostream& operator<<(ostream& sortie, const InputFile<T> & n) {
  for(auto one_line : n.getM_lines()){
    cout <<"#" <<  *one_line << "#" << endl;
  }
  return sortie;
}



#endif