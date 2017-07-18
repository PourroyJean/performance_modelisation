#include <getopt.h>
#include <cstdlib>
#include "kg_parameters.h"
#include <string.h>
#include <sstream>      // std::istringstream
#include <iostream>
#include "kg_misc.h"
#include <unistd.h>


std::string BIN_DIR;
std::string HOME_DIR;

using namespace std;

KG_parameters::KG_parameters() {
    //By default use the parameters located in the tool_feq_misch.h file
    P_WIDTH = PARAM_WIDTH;
    P_OPERATIONS = PARAM_OPERATIONS;
    P_BIND = PARAM_BIND;
    P_DEPENDENCY = PARAM_DEPENDENCY;
    P_PRECISION = PARAM_PRECISION;
    P_LOOP_SIZE = PARAM_LOOP_SIZE;
    P_GRAPH = PARAM_GRAPH;
    P_SAMPLES = PARAM_SAMPLES;
    P_VERBOSE = PARAM_VERBOSE;
    P_HELP = false;
};


void KG_parameters::parse_arguments(int argc, char **argv) {

    //Parse the binary directory absolute path
    string s1(argv[0]);
    BIN_DIR = s1.substr(0, s1.find_last_of("\\/"));

    //Get the current directory
    char buff[999];
    getcwd(buff, 999);
    string cwd(buff);
    HOME_DIR = cwd;

    const char *const short_opts = "W:O:B:D:P:L:S:vhG";
    const option long_opts[] = {
            {"width",      required_argument, nullptr, 'W'},
            {"operations", required_argument, nullptr, 'O'},
            {"bind",       required_argument, nullptr, 'B'},
            {"dependency", required_argument, nullptr, 'D'},
            {"precision",  required_argument, nullptr, 'P'},
            {"loopsize",   required_argument, nullptr, 'L'},
            {"graphic",    no_argument,       nullptr, 'G'},
            {"samples",    required_argument, nullptr, 'S'},
            {"verbose",    no_argument,       nullptr, 'v'},
            {"help",       no_argument,       nullptr, 'h'},
            {nullptr, 0,                      nullptr, 0}
    };

    char option;
    string tmp_str;
    while ((option = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
        int ioptarg;
        switch (option) {
            case 'W':
                ioptarg = atoi(optarg);
                if (ioptarg == 64 || ioptarg == 128 || ioptarg == 256 || ioptarg == 512) {
                    this->P_WIDTH = ioptarg;
                } else {
                    printf("/!\\ WRONG WIDTH OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'O':
                P_OPERATIONS = optarg;
                break;
            case 'B':
                if ((atoi(optarg) >= 0) && (4096 >= atoi(optarg))) {
                    this->P_BIND = atoi(optarg);
                } else {
                    printf("/!\\ WRONG BINDING OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'D':
                tmp_str = optarg;
                if (!tmp_str.compare("true") || !tmp_str.compare("false")) {
                    bool b;
                    istringstream(optarg) >> std::boolalpha >> b;
                    P_DEPENDENCY = b;
                } else {
                    printf("/!\\ WRONG DEPENDENCY OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }

                break;
            case 'P':
                tmp_str = optarg;
                if (!tmp_str.compare("single") || !tmp_str.compare("double")) {
                    this->P_PRECISION = optarg;
                } else {
                    printf("/!\\ WRONG PRECISION OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'L':
                ioptarg = atoi(optarg);
                if (ioptarg > 0 && ioptarg < 1000000000) {
                    this->P_LOOP_SIZE = ioptarg;
                } else {
                    printf("/!\\ WRONG LOOP SIZE OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'G':
                this->P_GRAPH = true;
                break;
            case 'S':
                ioptarg = atoi(optarg);
                if (ioptarg > 0 && ioptarg < 1000000000) {
                    this->P_SAMPLES = ioptarg;
                } else {
                    printf("/!\\ WRONG SAMPLES OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'v':
                this->P_VERBOSE = true;
                break;
            case 'h':
                this->P_HELP = true;
                break;
            default:
                usage();
                exit(EXIT_FAILURE);
        }
    }

    check_arguments();
}


void KG_parameters::check_operations() {
    for (char &c : P_OPERATIONS) {
        if (c != 'a' && c != 'A' && c != 'f' && c != 'F' && c != 'm' && c != 'M') {
            cout << "/!\\ WRONG OPERATIONS " << c << endl;
            exit(EXIT_FAILURE);
        }
    }
}


void KG_parameters::parameter_summary() {
    cout << "\t -W (width)            " << this->P_WIDTH << endl;
    cout << "\t -O <operationsl list> " << this->P_OPERATIONS << endl;
    cout << "\t -B (core binding)     " << this->P_BIND << endl;
    cout << "\t -D (op dependency)    " << std::boolalpha << this->P_DEPENDENCY << endl;
    cout << "\t -P (op precision)     " << this->P_PRECISION << endl;
    cout << "\t -L (loop size)        " << this->P_LOOP_SIZE << endl;
    cout << "\t -G (graphic)          " << this->P_GRAPH << endl;
    cout << "\t -S (samples)          " << this->P_SAMPLES << endl;

}


void KG_parameters::help() {
    cout << endl;
    cout << "This tool should be launched with the following parameters ([] = default):\n";
    cout << "\t -W, --width            ["<< PARAM_WIDTH<< "] / 64 128 256 512" << endl;
    cout << "\t -O, --operations-list  ["<< PARAM_OPERATIONS"] / ADD=a MUL=m FMA=f" << endl;
    cout << "\t -B,--binding           [no binding] / 0 1 2 ... NbCore\n";
    cout << "\t -D,--dependency        ["<< boolalpha <<  PARAM_DEPENDENCY<< "] / true false\n";
    cout << "\t -P,--precision         [" << PARAM_PRECISION <<"] / single double\n";
    cout << "\t -L,--loopsize          [" << PARAM_LOOP_SIZE << "]\n";
    cout << "\t -G,--graphic            grapical output (python required)\n";
    cout << "\t -S,--samples           [" << PARAM_SAMPLES << "]\n";
    cout << "\t -v,--verbose\n";
    cout << "\t -h,--help\n";
}

void KG_parameters::check_arguments() {
    if (this->P_HELP) {
        this->help();
        exit(0);
    }
    if (this->P_VERBOSE) {
        printf("The frequency tool was launched with:\n");
        this->parameter_summary();
    };

    check_operations();
}