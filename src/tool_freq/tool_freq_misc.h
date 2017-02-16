#ifndef __tool_freq_misc_H__
#define __tool_freq_misc_H__

#include <map>
#include <string>

using namespace std;
enum {
    SCALAR, SSE, AVX, ADD, MUL, FMA
};

static const string Parameter [] = {"SCALAR",  "SSE",  "AVX",  "ADD",  "MUL",  "FMA"};
static std::map<int,std::string> mapParameter = {{SCALAR,"SCALAR"}, {SSE, "SSE"}, {AVX, "AVX"}, {ADD, "ADD"}, {MUL, "MUL"}, {FMA, "FMA"}};
class Tool_freq_parameters {
    public:
    int P_SIMD;
    int P_OPERATION_TYPE;
    int P_OPERATION_NB;
    int P_BIND;
    int P_WIDTH;
    bool P_VERBOSE;
    bool P_HELP;

//    std::map<char,std::string> mapParameter;

    Tool_freq_parameters();

    void check_arguments();
    void parse_arguments(int argc, char **argv);
    void parameter_summary();

};

#endif //__tool_freq_misc_H__
