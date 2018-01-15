//
// Created by Jean Pourroy on 12/01/2018.
//

#ifndef PERFORMANCE_MODELISATION_CODE_ANNOTATION_H
#define PERFORMANCE_MODELISATION_CODE_ANNOTATION_H

#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;





#define YAMB_ANNOTATE_LOG_FILE "/tmp/yamb_annotate_log_file"


ofstream m_LOG_FILE;

int yamb_annotate_set_event (string event_name, string color);





#endif //PERFORMANCE_MODELISATION_CODE_ANNOTATION_H
