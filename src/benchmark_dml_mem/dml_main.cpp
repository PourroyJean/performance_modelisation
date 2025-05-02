#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>
#include <cstring>
#include <cerrno>
#include <sys/time.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>

#ifdef COMPILED_WITH_MPI
#include <mpi.h>
#else
// MPI definitions for non-MPI builds
#define MPI_COMM_WORLD 0
#define MPI_DOUBLE 0
#define MPI_SUM 0
#define MPI_MAX 0
#define MPI_Barrier(comm) ((void)0)
#define MPI_Reduce(sb, rb, cnt, dt, op, root, comm) \
    (*(double *)(rb) = *(double *)(sb))
#define MPI_Finalize() ((void)0)
#endif

// Project headers
#include "misc.h"
#include "dml_misc.h"
#include "dml_parameters.h"
#include "dml_benchmark.h"
#include "code_annotation.h" // ANNOTATE macro

using namespace std;

// External globals (defined in dml_misc.cpp)
extern DML_DATA_TYPE *mat;                ///< Benchmark data array pointer
extern int mpi_rank;                      ///< Rank of this process
extern int mpi_size;                      ///< Total number of MPI ranks
extern bool is_I_LOG;                     ///< Flag: logging enabled
extern double overall_total_loops_global; ///< Aggregate loop count across all K/S

// Shared-memory identifier for Huge Pages
static int shmid = -2;

// ----- Function Prototypes -----
void data_alloc_spe(size_t size);
void init_mat(Dml_parameters *p);
int run_benchmark_loops(Dml_parameters *p);
double get_micros();
double compute_bandwidth_metric(
    Dml_parameters *p,
    double time_ns,
    double ops_count,
    int return_unit = 0);

void print_benchmark_headers(Dml_parameters *p, string &full_log_output);
struct StrideParams
{
    int bytes;
    int elems;
    int repeat;
    bool feasible;
    bool is_max;
};
vector<StrideParams> prepare_stride_params(Dml_parameters *p, uint64_t nelems);
void measure_stride(Dml_parameters *p, const StrideParams &sp, uint64_t nelems, double &best_ns, double &worst_ns, double &sum_ns, double &ops_stride_local);
void print_and_log_stride_results(Dml_parameters *p, const StrideParams &sp, double best_ns, double worst_ns, double sum_ns, double ops_stride_local, string &log_row);
void aggregate_and_print_k_results(Dml_parameters *p, double local_k_ops_max, double local_k_time_max_us, string &log_row);
uint64_t get_next_size(Dml_parameters *p, uint64_t current_nelems);

/**
 * @brief Entry point: initializes MPI, parses arguments, runs benchmarks, and prints results.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of argument strings
 * @return Exit code (0 on success)
 */
int main(int argc, char *argv[])
{
    // Initialize MPI (if available)
#ifdef COMPILED_WITH_MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
#else
    mpi_rank = 0;
    mpi_size = 1;
#endif

    // Parse benchmark parameters
    if (mpi_rank == 0)
        cout << "\n--- Benchmark Configuration ---\n";
    Dml_parameters *params = new Dml_parameters();
    params->init_arguments(argc, const_cast<const char **>(argv));
    if (mpi_rank == 0)
        params->print_configuration();

    // Allocate and initialize matrix data
    init_mat(params);

    // Setup optional logging on rank 0
    is_I_LOG = (mpi_rank == 0 && params->m_is_log);
    if (is_I_LOG)
    {
        params->m_log_file.open(params->m_log_file_name, ios::binary);
        if (!params->m_log_file)
        {
            cerr << "ERROR: Failed to open log file '"
                << params->m_log_file_name << "'\n";
            is_I_LOG = false;
        }
        else
        {
            // Record command-line for reproducibility
            stringstream ss;
            for (int i = 1; i < argc; ++i)
                ss << argv[i] << " ";
            params->m_log_file << "# Command: " << argv[0]
                            << " " << ss.str() << "\n";
        }
    }

    // Synchronize and measure overall start time
    MPI_Barrier(MPI_COMM_WORLD);
    double t_start_us = get_micros();

    // Run the main benchmarking loops
    run_benchmark_loops(params);

    // Synchronize and measure overall end time
    MPI_Barrier(MPI_COMM_WORLD);
    double t_end_us = get_micros();

    // Aggregate total loop count across ranks
#ifdef COMPILED_WITH_MPI
    double global_loops = 0.0;
    MPI_Reduce(&overall_total_loops_global, &global_loops, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (mpi_rank == 0)
        overall_total_loops_global = global_loops;
#endif

    // Rank 0 prints final summary
    if (mpi_rank == 0)
    {
        cout << "\n--- Overall Results ---\n";
        double wall_us = t_end_us - t_start_us;
        double wall_ns = wall_us * 1e3;
        // Use the helper function to compute overall metrics
        double ns_per_loop = compute_bandwidth_metric(
            params, wall_ns, overall_total_loops_global, 1); // 1 for ns/op
        double gb_per_s = compute_bandwidth_metric(
            params, wall_ns, overall_total_loops_global, 0); // 0 for GB/s

        printf("%-25s: %s\n", "Benchmark Prefix", params->m_prefix.c_str());
        printf("%-25s: %.0f us\n", "Total Wall Time", wall_us);
        printf("%-25s: %.0f\n", "Total Loops", overall_total_loops_global);
        printf("%-25s: %.3f ns/loop\n", "Average Latency", ns_per_loop);
        printf("%-25s: %.3f GB/s\n", "Aggregate Bandwidth", gb_per_s);
    }

    // Cleanup matrix memory
    if (params->m_is_huge_pages)
    {
        // Detach and remove shared memory segment
        if (mat)
            shmdt(mat);
        if (shmid != -2 && mpi_rank == 0)
            shmctl(shmid, IPC_RMID, nullptr);
    }
    else if (mat)
    {
        free(mat);
    }
    mat = nullptr;

    // Close log file if open
    if (is_I_LOG && params->m_log_file.is_open())
    {
        params->m_log_file.close();
    }

    // Finalize MPI and free parameters
    MPI_Finalize();
    delete params;
    return 0;
}

/**
 * @brief Prints the console and log headers for the benchmark table.
 *
 * Only rank 0 outputs; builds the CSV-style header for logging.
 * @param p                 Benchmark parameters
 * @param full_log_output   String to append CSV-style header for logging
 */
void print_benchmark_headers(Dml_parameters *p, string &full_log_output)
{
    if (mpi_rank != 0)
        return;

    // Build the log header row based on strides and display mode
    string log_header_row = "Size(bytes)";
    int cols_per_stride = 0;
    if (p->m_DISP == DISP_MODE::AVERAGE)
        cols_per_stride = 1;
    else if (p->m_DISP == DISP_MODE::BEST)
        cols_per_stride = 1;
    else if (p->m_DISP == DISP_MODE::TWO)
        cols_per_stride = 2;
    else if (p->m_DISP == DISP_MODE::ALL)
        cols_per_stride = 3;

    for (int stride_bytes : p->m_STRIDE_LIST)
    {
        if (cols_per_stride >= 1)
            log_header_row += ",Stride_" + to_string(stride_bytes) + "_BEST";
        if (cols_per_stride >= 2)
            log_header_row += ",Stride_" + to_string(stride_bytes) + "_WORST";
        if (cols_per_stride >= 3)
            log_header_row += ",Stride_" + to_string(stride_bytes) + "_AVERAGE";
    }
    log_header_row += ",Bandwidth(GB/s)\n";

    // Print the console header
    printf("_ %s Stride   S  ->", p->m_prefix.c_str());
    for (int stride : p->m_STRIDE_LIST)
    {
        int cols = (p->m_DISP == DISP_MODE::ALL) ? 3 : ((p->m_DISP == DISP_MODE::TWO) ? 2 : 1);
        for (int i = 0; i < cols; ++i)
            printf("%11d", stride);
    }
    printf(" | %15s\n", "MaxS_Agg_BW (GB/s)");

    printf("_ %s Value      ->", p->m_prefix.c_str());
    for (int stride : p->m_STRIDE_LIST)
    {
        if (p->m_DISP == DISP_MODE::AVERAGE)
            printf("%11s", "AVERAGE");
        if (p->m_DISP == DISP_MODE::BEST)
            printf("%11s", "BEST");
        if (p->m_DISP == DISP_MODE::TWO)
            printf("%11s%11s", "BEST", "AVERAGE");
        if (p->m_DISP == DISP_MODE::ALL)
            printf("%11s%11s%11s", "BEST", "WORST", "AVERAGE");
    }
    printf(" | %15s\n", "--------------");
    fflush(stdout);

    if (is_I_LOG)
    {
        full_log_output += log_header_row;
    }
}

/**
 * @brief Prepares stride parameters (elements, repeat, feasibility) for a given size.
 *
 * Calculates stride in elements, repetitions per measurement, and checks feasibility.
 * A stride is considered feasible if it's at least the cache line size and
 * results in a sufficient number of steps (>= MIN_OPS_PER_SCAN).
 *
 * @param p         Benchmark parameters
 * @param nelems    Number of elements in the current working set size
 * @return Vector of StrideParams for each stride in p->m_STRIDE_LIST
 */
vector<StrideParams> prepare_stride_params(Dml_parameters *p, uint64_t nelems)
{
    vector<StrideParams> sparams;
    int max_bytes = p->m_STRIDE_LIST.back();

    for (int sb : p->m_STRIDE_LIST)
    {
        StrideParams sp;
        sp.bytes = sb;

        sp.elems = max(1, sb / static_cast<int>(sizeof(DML_DATA_TYPE)));

        // Calculate number of steps and check feasibility based on MIN_OPS_PER_SCAN
        uint64_t steps = (sp.elems > 0) ? nelems / sp.elems : 0;
        sp.feasible = (steps >= MIN_OPS_PER_SCAN);

        // Calculate repeat factor to reach target ops (p->m_MAX_OPS)
        sp.repeat = max<int>(5, p->m_MAX_OPS / max<int>(1ULL, steps));

        // If not enough steps for feasibility, mark repeat as 0 
        if (!sp.feasible) {
            sp.repeat = 0;
        }
        
        sp.is_max = (sb == max_bytes);
        sparams.push_back(sp);
    }
    return sparams;
}

/**
 * @brief Performs the measurement loop for a single stride.
 *
 * Accumulates best, worst, and sum times and total ops for this stride locally.
 * @param p                 Benchmark parameters
 * @param sp                Stride parameters
 * @param nelems            Number of elements in the current working set size
 * @param best_ns           [out] Best time in ns
 * @param worst_ns          [out] Worst time in ns
 * @param sum_ns            [out] Sum of times in ns
 * @param ops_stride_local  [out] Total operations for this stride across all measures
 */
void measure_stride(Dml_parameters *p, const StrideParams &sp, uint64_t nelems,
                    double &best_ns, double &worst_ns, double &sum_ns, double &ops_stride_local)
{

    best_ns = numeric_limits<double>::max();
    worst_ns = 0.0;
    sum_ns = 0.0;
    ops_stride_local = 0.0;

    if (mpi_rank == 0)
    {
        string proc_count_str = (mpi_size > 1) ? to_string(mpi_size) : "1";
        ANNOTATE(string("Stride(B): " + to_string(sp.bytes) + " np: " + proc_count_str).c_str(), "red");
    }

    if (sp.is_max)
        MPI_Barrier(MPI_COMM_WORLD);
    double t0_us = sp.is_max ? get_micros() : 0.0;

    for (int iter = 0; iter < p->m_MAX_MEASURES; ++iter)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        double t1_us = get_micros();
        double ops = p->m_BENCHMARK(p, sp.elems, sp.repeat, (nelems > 0 && sp.elems > 0) ? nelems / sp.elems : 0);
        double t2_us = get_micros();
        MPI_Barrier(MPI_COMM_WORLD);

        if (ops > 0 && ops < BIG_VAL)
        {
            ops_stride_local += ops;
            if (mpi_rank == 0)
            {
                double dt_ns = (t2_us - t1_us) * 1e3;
                best_ns = std::min(best_ns, dt_ns);
                worst_ns = std::max(worst_ns, dt_ns);
                sum_ns += dt_ns;
            }
        }
    }

    // Record outer timing for max stride on rank 0
    if (sp.is_max && mpi_rank == 0)
    {
        double t1b_us = get_micros();
    }
}

/**
 * @brief Prints and logs performance metrics for a single stride on rank 0.
 *
 * @param p                 Benchmark parameters
 * @param sp                Stride parameters (used for feasibility check)
 * @param best_ns           Best time in ns
 * @param worst_ns          Worst time in ns
 * @param sum_ns            Sum of times in ns
 * @param ops_stride_local  Total operations for this stride across all measures
 * @param log_row           CSV log line accumulator for the current size row
 */
void print_and_log_stride_results(Dml_parameters *p, const StrideParams &sp,
    double best_ns, double worst_ns, double sum_ns, double ops_stride_local,
    string &log_row)
{
    if (mpi_rank != 0)
        return;

    if (sp.feasible && ops_stride_local > 0 && ops_stride_local < BIG_VAL)
    {
        auto print_and_log = [&](double time_ns, double ops_count_for_metric, DISP_UNIT unit, bool print_now, bool log_now)
        {
            double display_value;
            if (ops_count_for_metric <= 0.0)
            {
                display_value = 0.0;
            }
            else
            {
                if (unit == DISP_UNIT::GB)
                    display_value = compute_bandwidth_metric(p, time_ns, ops_count_for_metric, 0);
                else if (unit == DISP_UNIT::CY)
                    display_value = compute_bandwidth_metric(p, time_ns, ops_count_for_metric, 2);
                else
                    display_value = compute_bandwidth_metric(p, time_ns, ops_count_for_metric, 1);
            }

            if (print_now)
            {
                if (ops_count_for_metric > 0)
                    printf("%11.2f", display_value);
                else
                    printf("%11s", "-");
            }
            if (log_now && is_I_LOG)
            {
                stringstream ss;
                if (ops_count_for_metric > 0)
                    ss << fixed << setprecision(2) << display_value;
                else
                    ss << "0.00";
                log_row += "," + ss.str();
            }
        };

        double ops_per_single_measure = (p->m_MAX_MEASURES > 0) ? ops_stride_local / p->m_MAX_MEASURES : 0.0;

        if ((p->m_DISP == DISP_MODE::BEST || p->m_DISP == DISP_MODE::ALL || p->m_DISP == DISP_MODE::TWO))
        {
            print_and_log(best_ns, ops_per_single_measure, static_cast<DISP_UNIT>(p->m_unit), true, true);
        }
        if (p->m_DISP == DISP_MODE::ALL)
        {
            print_and_log(worst_ns, ops_per_single_measure, static_cast<DISP_UNIT>(p->m_unit), true, true);
        }
        if ((p->m_DISP == DISP_MODE::ALL || p->m_DISP == DISP_MODE::TWO || p->m_DISP == DISP_MODE::AVERAGE))
        {
            print_and_log(sum_ns, ops_stride_local, static_cast<DISP_UNIT>(p->m_unit), true, true);
        }
    }
    else
    {
        int cols = (p->m_DISP == DISP_MODE::ALL) ? 3 : ((p->m_DISP == DISP_MODE::TWO) ? 2 : 1);
        for (int i = 0; i < cols; ++i)
        {
            printf("%11s", "N/A");
            if (is_I_LOG)
                log_row += ",N/A"; // Log N/A for skipped/infeasible
        }
    }
}

/**
 * @brief Aggregates max-stride metrics across ranks and prints/logs K-aggregate bandwidth on rank 0.
 *
 * @param p                     Benchmark parameters
 * @param local_k_ops_max       Local total ops for the max stride in this K iteration
 * @param local_k_time_max_us   Local max time for the max stride in this K iteration (outer timing)
 * @param log_row               CSV log line accumulator for the current size row
 */
void aggregate_and_print_k_results(Dml_parameters *p, double local_k_ops_max, double local_k_time_max_us, string &log_row)
{
    double global_k_ops_max = 0.0;
    double global_k_time_max_us = 0.0;
#ifdef COMPILED_WITH_MPI
    MPI_Reduce(&local_k_ops_max, &global_k_ops_max, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_k_time_max_us, &global_k_time_max_us, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
#else
    global_k_ops_max = local_k_ops_max;
    global_k_time_max_us = local_k_time_max_us;
#endif

    if (mpi_rank == 0)
    {
        double bandwidth_gb_s = 0.0;
        if (global_k_ops_max > 0 && global_k_ops_max < BIG_VAL && global_k_time_max_us > 0)
        {
            bandwidth_gb_s = compute_bandwidth_metric(p, global_k_time_max_us * 1e3, global_k_ops_max, 0);
        }

        printf(" | %15.2f\n", bandwidth_gb_s);
        fflush(stdout);

        if (is_I_LOG)
        {
            stringstream ss;
            ss << fixed << setprecision(2) << bandwidth_gb_s;
            log_row += "," + ss.str() + "\n";
        }
    }
}

/**
 * @brief Calculates the next working set size based on parameters and current size.
 *
 * Handles stepping logic, clamping to END_SIZE, and ensuring progress.
 * @param p                 Benchmark parameters
 * @param current_nelems    Current working set size in elements
 * @return Next working set size in elements
 */
uint64_t get_next_size(Dml_parameters *p, uint64_t current_nelems)
{
    uint64_t next_nelems = 0;

    if (p->m_SIZE_STEP_FACTOR > 1.0)
    {
        double stepped_size = static_cast<double>(current_nelems) * p->m_SIZE_STEP_FACTOR;
        if (stepped_size <= current_nelems + 1e-9)
        {
            next_nelems = current_nelems + 1;
        }
        else
        {
            next_nelems = static_cast<uint64_t>(stepped_size + 0.5);
        }
    }
    else
    {
        if (current_nelems < p->m_END_SIZE && p->m_START_SIZE != p->m_END_SIZE)
        {
            next_nelems = p->m_END_SIZE;
        }
        else
        {
            return p->m_END_SIZE + 1; // Signal to break the loop if factor <= 1 and not jumping to end
        }
    }

    if (next_nelems > p->m_END_SIZE)
    {
        next_nelems = p->m_END_SIZE;
    }

    if (current_nelems > 0 && next_nelems <= current_nelems && current_nelems < p->m_END_SIZE)
    {
        next_nelems = current_nelems + 1;
    }

    if (current_nelems > 0 && next_nelems == 0)
    {
        if (mpi_rank == 0)
            printf("WARNING: Calculated next size is 0, forcing to current + 1.\n");
        next_nelems = current_nelems + 1;
    }

    return next_nelems;
}

/**
 * @brief Executes benchmarking loops over working set sizes and strides.
 *
 * For each working set size and each stride S, measures performance over multiple
 * iterations, computes best/worst/average times, and logs/prints results.
 * The working set size iterates from m_START_SIZE to m_END_SIZE using m_SIZE_STEP_FACTOR.
 *
 * @param p Benchmark parameters and state
 * @return 0 on success
 */
int run_benchmark_loops(Dml_parameters *p)
{
    double total_ops_local = 0.0;
    string full_log_output;

    print_benchmark_headers(p, full_log_output);

    if (p->m_START_SIZE > p->m_END_SIZE)
    {
        if (mpi_rank == 0)
            printf("NOTICE: START_SIZE (%lu) is greater than END_SIZE (%lu), no benchmarks will be run.\n", p->m_START_SIZE, p->m_END_SIZE);
        return 0;
    }
    if (p->m_START_SIZE == 0)
    {
        if (mpi_rank == 0)
            printf("NOTICE: START_SIZE is 0, skipping benchmark loop.\n");
        return 0;
    }

    uint64_t current_nelems = p->m_START_SIZE;

    while (true)
    {
        uint64_t nelems = current_nelems;
        size_t bytes = nelems * sizeof(DML_DATA_TYPE);

        if (nelems > p->m_END_SIZE)
            break;
        if (nelems > p->m_MAT_NB_ELEM)
        {
            if (mpi_rank == 0)
                printf("NOTICE: K size exceeds matrix range (%lu > %lu), stopping.\n", nelems, p->m_MAT_NB_ELEM);
            break;
        }

        string log_row;
        if (mpi_rank == 0)
        {
            string size_str = convert_size(bytes);
            printf("_ %-10s %10s : ", p->m_prefix.c_str(), size_str.c_str());
            fflush(stdout);
            ANNOTATE(("K=" + size_str).c_str(), "blue");
            if (is_I_LOG)
                log_row += to_string(bytes);
        }

        vector<StrideParams> sparams = prepare_stride_params(p, nelems);

        double k_ops_max_local = 0.0;
        double k_time_max_us_local = 0.0; // Local max time for max stride

        for (auto &sp : sparams)
        {
            if (sp.feasible)
            {
                double best_ns, worst_ns, sum_ns, ops_stride_local;

                if (sp.is_max)
                    MPI_Barrier(MPI_COMM_WORLD);
                double t0_us = sp.is_max ? get_micros() : 0.0;

                // Perform measurements for this stride
                double current_best_ns = numeric_limits<double>::max();
                double current_worst_ns = 0.0;
                double current_sum_ns = 0.0;
                double current_ops_stride_local = 0.0;

                if (mpi_rank == 0)
                {
                    string proc_count_str = (mpi_size > 1) ? to_string(mpi_size) : "1";
                    ANNOTATE(string("Stride(B): " + to_string(sp.bytes) + " np: " + proc_count_str).c_str(), "red");
                }

                for (int iter = 0; iter < p->m_MAX_MEASURES; ++iter)
                {
                    MPI_Barrier(MPI_COMM_WORLD);
                    double t1_us = get_micros();
                    double ops = p->m_BENCHMARK(p, sp.elems, sp.repeat, (nelems > 0 && sp.elems > 0) ? nelems / sp.elems : 0);
                    double t2_us = get_micros();
                    MPI_Barrier(MPI_COMM_WORLD);

                    if (ops > 0 && ops < BIG_VAL)
                    {
                        current_ops_stride_local += ops;
                        if (mpi_rank == 0)
                        {
                            double dt_ns = (t2_us - t1_us) * 1e3;
                            current_best_ns = std::min(current_best_ns, dt_ns);
                            current_worst_ns = std::max(current_worst_ns, dt_ns);
                            current_sum_ns += dt_ns;
                        }
                    }
                }

                // Accumulate local ops for the max stride
                if (sp.is_max)
                    k_ops_max_local += current_ops_stride_local;

                // Record outer timing for max stride on rank 0
                if (sp.is_max && mpi_rank == 0)
                {
                    double t1b_us = get_micros();
                    k_time_max_us_local = t1b_us - t0_us;
                }

                // Print and log results for this stride on rank 0
                print_and_log_stride_results(p, sp, current_best_ns, current_worst_ns, current_sum_ns, current_ops_stride_local, log_row);
            }
            else
            {
                // Stride not feasible or skipped due to alignment - print N/A placeholders
                if (mpi_rank == 0)
                {
                    int cols = (p->m_DISP == DISP_MODE::ALL) ? 3 : ((p->m_DISP == DISP_MODE::TWO) ? 2 : 1);
                    for (int i = 0; i < cols; ++i)
                    {
                        printf("%11s", "N/A");
                        if (is_I_LOG)
                            log_row += ",N/A";
                    }
                }
            }
        }

        // Accumulate total_ops_local with the local ops from the max stride measurements in this K iteration
        total_ops_local += k_ops_max_local;

        // Aggregate max-stride metrics and print K-aggregate bandwidth on rank 0
        aggregate_and_print_k_results(p, k_ops_max_local, k_time_max_us_local, log_row);

        // Append completed log row for this size
        if (mpi_rank == 0 && is_I_LOG)
        {
            full_log_output += log_row;
        }

        // Break the loop if we just processed the end size
        if (nelems == p->m_END_SIZE)
            break;

        // Calculate the next working set size
        current_nelems = get_next_size(p, current_nelems);

        // Break if get_next_size signals to stop (e.g., factor <= 1 and not jumping to end)
        if (current_nelems > p->m_END_SIZE && nelems < p->m_END_SIZE)
        {
            break;
        }
        // Also break if the calculated next size is 0 unexpectedly, though get_next_size tries to prevent this.
        if (current_nelems == 0 && nelems > 0)
        {
            if (mpi_rank == 0)
                printf("WARNING: Calculated next size is 0, stopping benchmark loop.\n");
            break;
        }
    }

    overall_total_loops_global = total_ops_local;

    if (mpi_rank == 0 && is_I_LOG)
    {
        p->m_log_file << full_log_output;
        p->m_log_file.flush();
    }

    return 0;
}

/**
 * @brief Allocates matrix memory using Huge Pages via System V shared memory.
 *
 * Synchronizes shmid across ranks and attaches mat pointer.
 * @param size  Number of bytes to allocate
 */
void data_alloc_spe(size_t size)
{
#ifdef COMPILED_WITH_MPI
    if (mpi_rank == 0)
    {
        shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W | SHM_HUGETLB);
        if (shmid < 0)
        {
            cerr << "ERROR: shmget Huge Pages failed: " << strerror(errno) << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    MPI_Bcast(&shmid, 1, MPI_INT, 0, MPI_COMM_WORLD);
    mat = (DML_DATA_TYPE *)shmat(shmid, NULL, 0);
    MPI_Barrier(MPI_COMM_WORLD);
#else
    mat = (DML_DATA_TYPE *)malloc(size);
    assert(mat != nullptr);
#endif
}

/**
 * @brief Initializes the benchmark matrix, using Huge Pages or aligned malloc.
 *
 * @param p  Benchmark parameters (contains size, page flag, etc.)
 */
void init_mat(Dml_parameters *p)
{
    if (p->m_is_huge_pages)
    {
        data_alloc_spe(p->m_MAT_SIZE);
    }
    else
    {
        int result = posix_memalign((void **)&mat, 64, p->m_MAT_SIZE);
        if (result != 0) {
            std::cerr << "ERROR: posix_memalign failed: " << result << endl;
            exit(1);
        }
    }
    assert(mat != nullptr);
    DML_DATA_TYPE init_val =
        (p->m_type == BENCH_TYPE::READ) ? DML_DATA_TYPE(1.0)
                                        : DML_DATA_TYPE(2.0);
    for (size_t i = 0; i < p->m_MAT_NB_ELEM; ++i)
    {
        mat[i] = init_val;
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

/**
 * @brief Returns current time in microseconds using gettimeofday().
 * @return Timestamp in microseconds
 */
double get_micros()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1e6 + tv.tv_usec;
}

/**
 * @brief Computes performance metric: GB/s, ns/op, or cycles/op.
 *
 * @param p                     Parameters (cache line, CPU freq)
 * @param time_ns               Elapsed time in nanoseconds
 * @param ops_count             Number of operations measured
 * @param return_unit           0=GB/s, 1=ns/op, 2=cycles/op
 * @return Calculated metric value
 */
double compute_bandwidth_metric(
    Dml_parameters *p,
    double time_ns,
    double ops_count,
    int return_unit)
{
    if (time_ns <= 0.0 || ops_count <= 0.0)
        return 0.0;
    double ns_per_op = time_ns / ops_count;
    switch (return_unit)
    {
    case 1: // Latency (ns/op)
        return ns_per_op;
    case 2: // Cycles per op
        return ns_per_op * p->m_GHZ;
    default:
    { // Bandwidth in GB/s
        return p->m_CACHE_LINE / ns_per_op;
    }
    }
}
