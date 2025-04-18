/**
 * @file benchmark.cpp
 * @brief Memory bandwidth and latency benchmarking across working set sizes and strides using MPI.
 *
 * This program allocates a matrix (possibly using Huge Pages), iterates over
 * logarithmically spaced working set sizes (K) and a list of byte strides (S),
 * measures performance metrics (ns/loop, GB/s) for each combination, and aggregates results.
 * Supports MPI for multi-rank measurements and optional logging to file.
 */

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
   // Dummy MPI definitions for non-MPI builds
   #define MPI_COMM_WORLD 0
   #define MPI_DOUBLE      0
   #define MPI_SUM         0
   #define MPI_MAX         0
   #define MPI_Barrier(comm)      ((void)0)
   #define MPI_Reduce(sb, rb, cnt, dt, op, root, comm) \
           (*(double*)(rb) = *(double*)(sb))
   #define MPI_Finalize()         ((void)0)
 #endif
 
 // Project headers
 #include "misc.h"
 #include "dml_misc.h"
 #include "dml_parameters.h"
 #include "dml_benchmark.h"
 #include "code_annotation.h"  // ANNOTATE macro
 
 using namespace std;
 
 // External globals (defined in dml_misc.cpp)
 extern DML_DATA_TYPE*        mat;                   ///< Benchmark data array pointer
 extern int                   mpi_rank;              ///< Rank of this process
 extern int                   mpi_size;              ///< Total number of MPI ranks
 extern bool                  is_I_LOG;              ///< Flag: logging enabled
 extern double                overall_total_loops_global; ///< Aggregate loop count across all K/S
 
 // Shared-memory identifier for Huge Pages
 static int shmid = -2;
 
 // ----- Function Prototypes -----
 void data_alloc_spe(size_t size);
 void init_mat(Dml_parameters* p);
 int  run_benchmark_loops(Dml_parameters* p);
 double get_micros();
 void print_benchmark_header(Dml_parameters* p, string& log_header);
 void print_rank0_stride_result(
     Dml_parameters* p,
     double best_ns,
     double worst_ns,
     double sum_ns,
     double total_ops,
     string& log_line
 );
 void print_k_aggregate_result(
     Dml_parameters* p,
     double agg_ops,
     double max_time_us,
     string& log_line
 );
 double compute_bandwidth_metric(
     Dml_parameters* p,
     double time_ns,
     double ops_count,
     int return_unit = 0,
     double cache_line_override = 0.0
 );
 
 /**
  * @brief Entry point: initializes MPI, parses arguments, runs benchmarks, and prints results.
  *
  * @param argc Number of command-line arguments
  * @param argv Array of argument strings
  * @return Exit code (0 on success)
  */
 int main(int argc, char* argv[]) {
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
     if (mpi_rank == 0) cout << "\n--- Benchmark Configuration ---\n";
     Dml_parameters* params = new Dml_parameters();
     params->init_arguments(argc, const_cast<const char**>(argv));
     if (mpi_rank == 0) params->print_configuration();
 
     // Allocate and initialize matrix data
     init_mat(params);
 
     // Setup optional logging on rank 0
     is_I_LOG = (mpi_rank == 0 && params->m_is_log);
     if (is_I_LOG) {
         params->m_log_file.open(params->m_log_file_name, ios::binary);
         if (!params->m_log_file) {
             cerr << "ERROR: Failed to open log file '"
                  << params->m_log_file_name << "'\n";
             is_I_LOG = false;
         } else {
             // Record command-line for reproducibility
             stringstream ss;
             for (int i = 1; i < argc; ++i) ss << argv[i] << " ";
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
     MPI_Reduce(&overall_total_loops_global,
                &global_loops, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
     if (mpi_rank == 0) overall_total_loops_global = global_loops;
 #endif
 
     // Rank 0 prints final summary
     if (mpi_rank == 0) {
         cout << "\n--- Overall Results ---\n";
         double wall_us = t_end_us - t_start_us;
         double wall_ns = wall_us * 1e3;
         double ns_per_loop = compute_bandwidth_metric(
             params, wall_ns, overall_total_loops_global, 1);
         double gb_per_s = compute_bandwidth_metric(
             params, wall_ns, overall_total_loops_global, 0);
 
         printf("%-25s: %s\n", "Benchmark Prefix", params->m_prefix.c_str());
         printf("%-25s: %.0f us\n", "Total Wall Time", wall_us);
         printf("%-25s: %.0f\n", "Total Loops", overall_total_loops_global);
         printf("%-25s: %.3f ns/loop\n", "Average Latency", ns_per_loop);
         printf("%-25s: %.3f GB/s\n", "Aggregate Bandwidth", gb_per_s);
     }
 
     // Cleanup matrix memory
     if (params->m_is_huge_pages) {
         // Detach and remove shared memory segment
         if (mat) shmdt(mat);
         if (shmid != -2 && mpi_rank == 0) shmctl(shmid, IPC_RMID, nullptr);
     } else if (mat) {
         free(mat);
     }
     mat = nullptr;
 
     // Close log file if open
     if (is_I_LOG && params->m_log_file.is_open()) {
         params->m_log_file.close();
     }
 
     // Finalize MPI and free parameters
     MPI_Finalize();
     delete params;
     return 0;
 }
 
 /**
  * @brief Executes benchmarking loops over working set sizes (K) and strides (S).
  *
  * For each logarithmic K and each stride S, measures performance over multiple
  * iterations, computes best/worst/average times, and logs/prints results.
  *
  * @param p Benchmark parameters and state
  * @return 0 on success
  */
 int run_benchmark_loops(Dml_parameters* p) {
     double total_ops_local = 0.0;   // Local accumulator for ops
     string log_header;              // Header for logging per-rank output
     string log_full;                // Complete log contents
 
     // Print and optionally log the benchmark table header
     print_benchmark_header(p, log_header);
     if (mpi_rank == 0 && is_I_LOG) LOG_MPI(log_full, log_header);
 
     // Iterate over log-scaled K values
     for (double k_log = p->m_MIN_LOG10;
          k_log <= p->m_MAX_LOG10 + 1e-9;
          k_log += p->m_STEP_LOG10)
     {
         // Compute element count and byte size for this K
         uint64_t nelems = (uint64_t)(exp(k_log * LOG10) + 0.5);
         size_t bytes = nelems * sizeof(DML_DATA_TYPE);
 
         // Stop if K is out of matrix bounds
         if (nelems == 0 || nelems > p->m_MAT_NB_ELEM) {
             if (mpi_rank == 0 && nelems > 0) {
                 printf("NOTICE: K size exceeds matrix range, stopping.\n");
             }
             break;
         }
 
         // Print K row label on rank 0
         if (mpi_rank == 0) {
             printf("_ %s K = %10s", p->m_prefix.c_str(),
                    convert_size(bytes).c_str());
             fflush(stdout);
             ANNOTATE(("K=" + convert_size(bytes)).c_str(), "blue");
         }
 
         // Prepare stride parameters before timing
         struct StrideParams {
             int bytes;         ///< Stride in bytes
             int elems;         ///< Stride in elements
             int repeat;        ///< Repetitions per measurement
             bool feasible;     ///< True if at least MIN_OPS_PER_SCAN
             bool is_max;       ///< True if this is the max stride
         };
         vector<StrideParams> sparams;
         int max_bytes = p->m_STRIDE_LIST.back();
 
         for (int sb : p->m_STRIDE_LIST) {
             StrideParams sp;
             sp.bytes    = sb;
             sp.elems    = max(1, sb / (int)sizeof(DML_DATA_TYPE));
             int steps   = nelems / sp.elems;
             sp.feasible = (steps >= MIN_OPS_PER_SCAN);
             sp.repeat   = max(5, p->m_MAX_OPS / max(1,steps));
             sp.is_max   = (sb == max_bytes);
             sparams.push_back(sp);
         }
 
         double k_ops_max = 0.0;      // Ops for max stride
         double k_time_max_us = 0.0;  // Duration for max stride
 
         // Iterate over stride list
         for (auto& sp : sparams) {
             double best_ns = numeric_limits<double>::max();
             double worst_ns = 0.0;
             double sum_ns = 0.0;
             double ops_stride = 0.0;
 
             if (sp.feasible) {
                 // Synchronize on max stride start
                 if (sp.is_max) MPI_Barrier(MPI_COMM_WORLD);
                 double t0_us = sp.is_max ? get_micros() : 0.0;
 
                 // Perform multiple measurement iterations
                 for (int iter = 0; iter < p->m_MAX_MEASURES; ++iter) {
                     double t1_us = get_micros();
                     double ops = p->m_BENCHMARK(p, sp.elems, sp.repeat, nelems/sp.elems);
                     double t2_us = get_micros();
 
                     if (ops > 0 && ops < BIG_VAL) {
                         total_ops_local += ops;
                         ops_stride      += ops;
                         if (sp.is_max) k_ops_max += ops;
                         if (mpi_rank == 0) {
                             double dt_ns = (t2_us - t1_us) * 1e3;
                             best_ns  = min(best_ns, dt_ns);
                             worst_ns = max(worst_ns, dt_ns);
                             sum_ns  += dt_ns;
                         }
                     }
                 }
 
                 // Record outer timing for max stride
                 if (sp.is_max) {
                     double t1b_us = get_micros();
                     k_time_max_us = t1b_us - t0_us;
                     MPI_Barrier(MPI_COMM_WORLD);
                 }
             } else {
                 // Mark stride as skipped
                 ops_stride = BIG_VAL;
             }
 
             // Print per-stride metrics on rank 0
             if (mpi_rank == 0) {
                 print_rank0_stride_result(p,
                                           best_ns, worst_ns, sum_ns,
                                           ops_stride, log_header);
             }
         }
 
         // Aggregate max-stride metrics across ranks
         double global_ops = 0.0, global_time_us = 0.0;
 #ifdef COMPILED_WITH_MPI
         MPI_Reduce(&k_ops_max,    &global_ops,      1, MPI_DOUBLE, MPI_SUM,  0, MPI_COMM_WORLD);
         MPI_Reduce(&k_time_max_us,&global_time_us, 1, MPI_DOUBLE, MPI_MAX,  0, MPI_COMM_WORLD);
 #else
         global_ops     = k_ops_max;
         global_time_us = k_time_max_us;
 #endif
 
         // Print K-aggregate bandwidth on rank 0
         if (mpi_rank == 0) {
             print_k_aggregate_result(p, global_ops, global_time_us, log_header);
             if (is_I_LOG) LOG_MPI(log_full, log_header);
         }
     }
 
     // Update global loop counter
     overall_total_loops_global = total_ops_local;
 
     // Flush log file
     if (mpi_rank == 0 && is_I_LOG) {
         // Params pointer reused for m_log_file
         // Note: p->m_log_file already open
         // write buffered data
         p->m_log_file.flush();
     }
 
     return 0;
 }
 
 /**
  * @brief Prints the header row of the benchmark table (stride values and labels).
  *
  * Only rank 0 outputs; optionally logs to file.
  * @param p            Benchmark parameters
  * @param log_header   String to append CSV-style header for logging
  */
 void print_benchmark_header(Dml_parameters* p, string& log_header) {
     if (mpi_rank != 0) return;
     printf("_ %s Stride S ->", p->m_prefix.c_str());
     for (int stride : p->m_STRIDE_LIST) {
         printf(" %11d", stride);
     }
     printf(" | %15s\n", "MaxS_Agg_BW (GB/s)");
     if (is_I_LOG) log_header += "...\n";
 }
 
 /**
  * @brief Prints performance metrics for a single stride on rank 0.
  *
  * @param p             Parameters
  * @param best_ns       Best (minimum) time in ns
  * @param worst_ns      Worst (maximum) time in ns
  * @param sum_ns        Sum of all measured times in ns
  * @param total_ops     Total operations counted
  * @param log_line      CSV log line accumulator
  */
 void print_rank0_stride_result(Dml_parameters* p,
                                 double best_ns,
                                 double worst_ns,
                                 double sum_ns,
                                 double total_ops,
                                 string& log_line) {
     if (mpi_rank != 0) return;
     // Compute ops per measure for average
     double ops_per_measure = (p->m_MAX_MEASURES > 0 && total_ops > 0)
                                 ? total_ops / p->m_MAX_MEASURES
                                 : 0.0;
     // Print or skip based on feasibility
     if (total_ops < BIG_VAL && total_ops > 0) {
         // Best
         if (p->m_DISP==DISP_MODE::BEST || p->m_DISP==DISP_MODE::ALL)
             printf(" %11.2f", compute_bandwidth_metric(p, best_ns, ops_per_measure));
         // Worst
         if (p->m_DISP==DISP_MODE::ALL)
             printf(" %11.2f", compute_bandwidth_metric(p, worst_ns, ops_per_measure));
         // Average
         if (p->m_DISP==DISP_MODE::AVERAGE || p->m_DISP==DISP_MODE::ALL)
             printf(" %11.2f", compute_bandwidth_metric(p, sum_ns, total_ops));
     } else {
         // Skipped placeholder
         int fields = (p->m_DISP==DISP_MODE::ALL) ? 3 : 1;
         for (int i = 0; i < fields; ++i) printf(" %11s", "-");
     }
 }
 
 /**
  * @brief Prints aggregated bandwidth for the max-stride of current K on rank 0.
  *
  * @param p            Parameters
  * @param agg_ops      Aggregated operations count
  * @param max_time_us  Maximum elapsed time in microseconds
  * @param log_line     CSV log line accumulator
  */
 void print_k_aggregate_result(Dml_parameters* p,
                               double agg_ops,
                               double max_time_us,
                               string& log_line) {
     if (mpi_rank != 0) return;
     double max_time_ns = max_time_us * 1e3;
     double gb_per_s = compute_bandwidth_metric(p, max_time_ns, agg_ops, 0);
     printf(" | %15.2f\n", gb_per_s);
     if (is_I_LOG) log_line += "," + to_string(gb_per_s) + "\n";
 }
 
 /**
  * @brief Allocates matrix memory using Huge Pages via System V shared memory.
  *
  * Synchronizes shmid across ranks and attaches mat pointer.
  * @param size  Number of bytes to allocate
  */
 void data_alloc_spe(size_t size) {
 #ifdef COMPILED_WITH_MPI
     if (mpi_rank == 0) {
         shmid = shmget(IPC_PRIVATE, size,
                       IPC_CREAT|IPC_EXCL|SHM_R|SHM_W|SHM_HUGETLB);
         if (shmid < 0) {
             cerr << "ERROR: shmget Huge Pages failed: "
                  << strerror(errno) << endl;
             MPI_Abort(MPI_COMM_WORLD, 1);
         }
     }
     MPI_Bcast(&shmid, 1, MPI_INT, 0, MPI_COMM_WORLD);
     mat = (DML_DATA_TYPE*)shmat(shmid, NULL, 0);
     MPI_Barrier(MPI_COMM_WORLD);
 #else
     mat = (DML_DATA_TYPE*)malloc(size);
     assert(mat != nullptr);
 #endif
 }
 
 /**
  * @brief Initializes the benchmark matrix, using Huge Pages or aligned malloc.
  *
  * @param p  Benchmark parameters (contains size, page flag, etc.)
  */
 void init_mat(Dml_parameters* p) {
     if (p->m_is_huge_pages) {
         data_alloc_spe(p->m_MAT_SIZE);
     } else {
         posix_memalign((void**)&mat, 64, p->m_MAT_SIZE);
     }
     assert(mat != nullptr);
     DML_DATA_TYPE init_val =
         (p->m_type == BENCH_TYPE::READ) ? DML_DATA_TYPE(1.0)
                                         : DML_DATA_TYPE(2.0);
     for (size_t i = 0; i < p->m_MAT_NB_ELEM; ++i) {
         mat[i] = init_val;
     }
     MPI_Barrier(MPI_COMM_WORLD);
 }
 
 /**
  * @brief Returns current time in microseconds using gettimeofday().
  * @return Timestamp in microseconds
  */
 double get_micros() {
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
  * @param cache_line_override   Override for cache line size (0 for default)
  * @return Calculated metric value
  */
 double compute_bandwidth_metric(
     Dml_parameters* p,
     double time_ns,
     double ops_count,
     int return_unit,
     double cache_line_override
 ) {
     if (time_ns <= 0.0 || ops_count <= 0.0) return 0.0;
     double ns_per_op = time_ns / ops_count;
     switch (return_unit) {
         case 1:  // Latency
             return ns_per_op;
         case 2:  // Cycles per op
             return ns_per_op * p->m_GHZ;
         default: {  // Bandwidth in GB/s
             double line_bytes = (cache_line_override > 0.0)
                 ? cache_line_override
                 : p->m_CACHE_LINE;
             return line_bytes / ns_per_op;
         }
     }
 }
 