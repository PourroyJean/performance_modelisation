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
    //By default use the parameters located in the kg_misch.h file
    P_WIDTH = PARAM_WIDTH;
    P_WIDTH_CUSTOM = NULL;
    P_OPERATIONS = PARAM_OPERATIONS;
    P_COMPILER   = PARAM_COMPILER;
    P_BIND = PARAM_BIND;
    P_DEPENDENCY = PARAM_DEPENDENCY;
    P_NB_DEPENDENCY = PARAM_NB_DEPENDENCY;
    P_PRECISION = PARAM_PRECISION;
    P_LOOP_SIZE = PARAM_LOOP_SIZE;
    P_UNROLLING = PARAM_UNROLLING;
    P_FREQUENCY = PARAM_FREQUENCY;
    P_GRAPH = PARAM_GRAPH;
    P_SAMPLES = PARAM_SAMPLES;
    P_COUNT = PARAM_COUNT;
    P_VERBOSE = PARAM_VERBOSE;
    P_HELP = false;
    P_DEBUG = PARAM_DEBUG;
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

    const char *const short_opts = "W:O:K:B:D:P:L:U:F:S:A:C:vhG";
    const option long_opts[] = {
            {"width",      required_argument, nullptr, 'W'},
//            {"width_cust", required_argument, nullptr, 'w'},
            {"operations", required_argument, nullptr, 'O'},
            {"compiler",   required_argument, nullptr, 'K'},
            {"bind",       required_argument, nullptr, 'B'},
            {"dependency", required_argument, nullptr, 'D'},
            {"precision",  required_argument, nullptr, 'P'},
            {"loopsize",   required_argument, nullptr, 'L'},
            {"unrolling",  required_argument, nullptr, 'U'},
            {"frequency",  required_argument, nullptr, 'F'},
            {"graphic",    no_argument,       nullptr, 'G'},
            {"samples",    required_argument, nullptr, 'S'},
            {"count",      required_argument, nullptr, 'C'},
            {"verbose",    no_argument,       nullptr, 'v'},
            {"help",       no_argument,       nullptr, 'h'},
            {"debug",      required_argument, nullptr, 'A'},
            {nullptr, 0,                      nullptr, 0}
    };

    char option;
    string tmp_str;
    int tmp_int;
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
            case 'C':
                P_COMPILER   = optarg;
                break;
            case 'w':
                tmp_str = optarg;
                P_WIDTH_CUSTOM = new vector<int>();

                for (auto op: tmp_str) {

                    tmp_int = atoi(&op);
                    if (tmp_int >= 1 && tmp_int <= 4) {
                        P_WIDTH_CUSTOM->push_back(tmp_int);
                    } else {
                        printf("/!\\ WRONG CUSTOM WIDTH VALUE %s\n", optarg);
                        exit(EXIT_FAILURE);
                    }
                }
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
                ioptarg = atoi(optarg);
                if ((ioptarg >= 0) && (ioptarg < 32)) {
                    this->P_DEPENDENCY = true;
                    this->P_NB_DEPENDENCY = atoi(optarg);
                } else {
                    printf("/!\\ WRONG DEPENDENCY DEPTH OPTION: %s\n", optarg);
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
            case 'U':
                ioptarg = atoi(optarg);
                if (ioptarg >= 1 && ioptarg < 1000000000) {
                    this->P_UNROLLING = ioptarg;
                } else {
                    printf("/!\\ WRONG UNROLLING OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'F':
                tmp_str = optarg;
                if (!tmp_str.compare("true") || !tmp_str.compare("false")) {
                    bool b;
                    istringstream(optarg) >> std::boolalpha >> b;
                    P_FREQUENCY = b;
                } else {
                    printf("/!\\ WRONG FREQUENCY CHECK OPTION: %s\n", optarg);
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

            case 'R': //todo implement this option...
                tmp_str = optarg;
                if (!tmp_str.compare("true") || !tmp_str.compare("false")) {
                    bool b;
                    istringstream(optarg) >> std::boolalpha >> b;
                    P_COUNT = b;
                } else {
                    printf("/!\\ WRONG COUNT OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }


            case 'A':
                tmp_str = optarg;
                if (!tmp_str.compare("true") || !tmp_str.compare("false")) {
                    bool b;
                    istringstream(optarg) >> std::boolalpha >> b;
                    P_DEBUG = b;
                } else {
                    printf("/!\\ WRONG DEBUG OPTION: %s\n", optarg);
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
    cout << "\t -W (gobal width)      " << this->P_WIDTH << endl;
    cout << "\t -w (custom width)     " ; if(P_WIDTH_CUSTOM) for (int i : *this->P_WIDTH_CUSTOM) cout << i; cout << endl;
    cout << "\t -O <operationsl list> " << this->P_OPERATIONS << endl;
    cout << "\t -C <compiler command> " << this->P_COMPILER   << endl;
    cout << "\t -B (core binding)     " << this->P_BIND   << endl;
    cout << "\t -D (op dependency)    " << std::boolalpha << this->P_DEPENDENCY << " (" << P_NB_DEPENDENCY << ")" << endl;
    cout << "\t -P (op precision)     " << this->P_PRECISION << endl;
    cout << "\t -L (loop size)        " << this->P_LOOP_SIZE << endl;
    cout << "\t -U (unrolling)        " << this->P_UNROLLING << endl;
    cout << "\t -F (frequency)        " << std::boolalpha  << this->P_FREQUENCY << endl;
    cout << "\t -G (graphic)          " << this->P_GRAPH   << endl;
    cout << "\t -S (samples)          " << this->P_SAMPLES << endl;
    cout << "\t -A (analysis)         " << this->P_DEBUG << endl;
    cout << "\t -C (count)            " << this->P_COUNT << endl;

}


void KG_parameters::help() {
    cout << endl;
    cout << "This tool should be launched with the following parameters ([] = default):\n";
    cout << "\t -W, --width            [" << PARAM_WIDTH << "] / 64 128 256 512" << endl;
    cout << "\t -w, --width_cust       [11111] / 1(scalar), 2 (SSE/128), 3 (AVX/256), 4(AVX/512)" << endl;
    cout << "\t -O, --operations       [" << PARAM_OPERATIONS <<"] / ADD=a MUL=m FMA=f" << endl;
    cout << "\t -C, --compiler         [" << PARAM_COMPILER   <<"] / gcc, clang..." << endl;
    cout << "\t -B,--binding           [no binding] / 0 1 2 ... NbCore\n";
    cout << "\t -D,--dependency        [" << boolalpha << PARAM_DEPENDENCY << "] / true false\n";
    cout << "\t -P,--precision         [" << PARAM_PRECISION << "] / single double\n";
    cout << "\t -L,--loopsize          [" << PARAM_LOOP_SIZE << "]\n";
    cout << "\t -U,--unrolling         [" << PARAM_UNROLLING << "]\n";
    cout << "\t -F,--frequency         [" << PARAM_FREQUENCY << "] check the frequency of the core\n";
    cout << "\t -G,--graphic            grapical output (python required)\n";
    cout << "\t -S,--samples           [" << PARAM_SAMPLES << "]\n";
    cout << "\t -A,--debug             [" << PARAM_DEBUG << "]\n";
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

    if (P_WIDTH_CUSTOM && P_WIDTH_CUSTOM->size() != P_OPERATIONS.length()) {
        printf("/!\\ WRONG CUSTOM WIDTH SIZE\n");
        exit(EXIT_FAILURE);
    }
    check_operations();
}