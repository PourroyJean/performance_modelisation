#ifndef __kg_misc_H__
#define __kg_misc_H__

#include <map>
#include <string>

enum {
    SCALAR, SSE, AVX, ADD, MUL, FMA
};

static const std::string Parameter[] = {"SCALAR", "SSE", "AVX", "ADD", "MUL", "FMA"};
static std::map<int, std::string> mapParameter = {{SCALAR, "SCALAR"},
                                                  {SSE,    "SSE"},
                                                  {AVX,    "AVX"},
                                                  {ADD,    "ADD"},
                                                  {MUL,    "MUL"},
                                                  {FMA,    "FMA"}};

class KG_parameters {
public:
    int         P_WIDTH;
    std::string P_OPERATIONS;
    int         P_BIND;
    bool        P_DEPENDENCY;
    std::string P_PRECISION;
    bool        P_VERBOSE;
    bool        P_HELP;
    unsigned    P_LOOP_SIZE;
    unsigned    P_UNROLLING;
    bool        P_FREQUENCY;
    bool        P_GRAPH;
    unsigned    P_SAMPLES;
    bool        P_COUNT;
    bool        P_DEBUG;

    KG_parameters();

    void check_arguments();

    void parse_arguments(int argc, char **argv);

    void parameter_summary();

    void check_operations ();

    void help();

};

#endif //__kg_misc_H__
