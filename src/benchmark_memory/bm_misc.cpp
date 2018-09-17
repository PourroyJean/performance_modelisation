//
// Created by pourroy on 7/26/18.
//
#include "bm_misc.h"
#include <unistd.h>
#include <string>

using namespace std;

bool is_Launched_with_mpirun (){
    int pid = getppid ();
    char* n = (char*)calloc(1024,sizeof(char));
    if(n){
        sprintf(n, "/proc/%d/cmdline",pid);
        FILE* f = fopen(n,"r");
        if(f){
            size_t size;
            size = fread(n, sizeof(char), 1024, f);
            if(size>0){
                if('\n'==n[size-1])
                    n[size-1]='\0';
            }
            fclose(f);
        }
    }

    string name (n);
    string runmpi = "mpi"; //what we are looking to in the path; ex /opt/intel/linux/mpi/intel64/bin/pmi_proxy
    std::size_t found = name.find(runmpi);
    return (found!=std::string::npos) ?  true : false;
}
