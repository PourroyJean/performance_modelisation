//
// Created by Jean Pourroy on 12/01/2018.
//

#include "code_annotation.h"
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>





#include <string.h>
using namespace std;
using namespace std::chrono;

int yamb_annotate_set_event (string event_name, string color){

    //Sanity check
    std::replace(event_name.begin(), event_name.end(), ' ', '_');
    std::replace(color.begin(), color.end(), ' ', '_');


    m_LOG_FILE.open(YAMB_ANNOTATE_LOG_FILE, std::ofstream::out | std::ofstream::app);
    if (!(m_LOG_FILE.is_open())) {
        cerr << "Error opening one of these files: \n";
        return(-1);
    }

    struct timeval tp;
    gettimeofday(&tp, NULL);
    long long mslong = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000; //get current timestamp in milliseconds
    m_LOG_FILE << mslong << " " << event_name << " " << color << std::endl;

    m_LOG_FILE.close();

}




int main (){

    sleep (.1);
    yamb_annotate_set_event("Flag1second", "red");
    sleep (0.1);
    yamb_annotate_set_event("Flag2second", "blue");
    sleep (0.1);
    yamb_annotate_set_event("Flag3second", "orange");
    sleep (0.1);

}

