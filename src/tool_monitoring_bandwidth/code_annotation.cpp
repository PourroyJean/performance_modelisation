#include <iostream>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <sys/time.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>

#include "code_annotation.h"


using namespace std;
using namespace std::chrono;

int yamb_annotate_set_event(char *e, char *c) {

    //C to C++ type
    std::string event_name = string(e);
    std::string color = string(c);
    ofstream m_LOG_FILE;

    //Sanity check
    std::replace(event_name.begin(), event_name.end(), ' ', '_');
    std::replace(color.begin(), color.end(), ' ', '_');


    m_LOG_FILE.open(YAMB_ANNOTATE_LOG_FILE, std::ofstream::out | std::ofstream::app);
    if (!(m_LOG_FILE.is_open())) {
        cerr << "Error opening one of these files: \n";
        return (-1);
    }

    struct timeval tp;
    gettimeofday(&tp, NULL);
    long long mslong = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000.0; //get current timestamp in milliseconds
    m_LOG_FILE << mslong << " " << event_name << " " << color << std::endl;


    m_LOG_FILE.close();

    return 0;
}




//int main (){
//
//    yamb_annotate_set_event("-------", "red");
//    sleep (1);
//    yamb_annotate_set_event("+1", "blue");
//    sleep (1);
//    yamb_annotate_set_event("+1", "orange");
//    usleep (500000);
//    yamb_annotate_set_event("+0.5", "blue");
//
//}

