#ifndef __tool_freq_misc_H__
#define __tool_freq_misc_H__

#include <map>
#include <string>

using namespace std;
enum {
    SCALAR, SSE, AVX, ADD, MUL, FMA
};

static const string Parameter[] = {"SCALAR", "SSE", "AVX", "ADD", "MUL", "FMA"};
static std::map<int, std::string> mapParameter = {{SCALAR, "SCALAR"},
                                                  {SSE,    "SSE"},
                                                  {AVX,    "AVX"},
                                                  {ADD,    "ADD"},
                                                  {MUL,    "MUL"},
                                                  {FMA,    "FMA"}};

class Tool_freq_parameters {
public:
    int P_SIMD;
    int P_WIDTH;
    string P_OPERATIONS;
    int P_BIND;
    bool P_DEPENDENCY;
    bool P_VERBOSE;
    bool P_HELP;


    Tool_freq_parameters();

    void check_arguments();

    void parse_arguments(int argc, char **argv);

    void parameter_summary();

    void check_operations ();

    void help();

};

#endif //__tool_freq_misc_H__
