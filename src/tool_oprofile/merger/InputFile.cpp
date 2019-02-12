//
// Created by Jean Pourroy on 19/12/2018.
//


#ifdef PERFORMANCE_MODELISATION_INPUTFILE_H
#include "InputFile.h"
#include "vector"
#include <string>
#include <iostream>
#include <fstream>      // std::ifstream
#include <assert.h>

using namespace std;

template <class T>
InputFile<T>::InputFile(const string &m_file_path) :  m_file_path(m_file_path) {
}

template <class T>
ostream& operator<<(ostream& sortie, const InputFile<T> & n) {
    cout << "Print the file  " << n.get_file_path() << " with " << n.get_lines_vector().size() << " lines\n";
  for(auto one_line : n.get_lines_vector()){
    cout << "#" <<  *one_line << "#" << endl;
  }
  return sortie;
}

template <class T>
void InputFile<T>::analysis (){
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
#endif