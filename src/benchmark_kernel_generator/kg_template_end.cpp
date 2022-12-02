
        cycleInEnd = rdtsc();
        timeEnd = mygettime();
        pairVec.push_back(make_pair(cycleInEnd - cycleInStart, timeEnd - timeStart));
        cycle_total += (cycleInEnd - cycleInStart);
        time_total += timeEnd - timeStart;
        instructions_executed_total += instructions_executed;
        
    }

    loop_nb_instruction = (uint64_t) NB_INST * (uint64_t) P_UNROLLING ;
    nb_total_loop_iteration = (uint64_t)(NB_lOOP_IN * (uint64_t) NB_lOOP);
    instructions_total = nb_total_loop_iteration * loop_nb_instruction;
    
    IPC = double(instructions_total) / double(cycle_total);
    double freq = (cycle_total / time_total )/1000000000;

    double flop_cycle_sp  = float((FLOP_SP_PER_LOOP * nb_total_loop_iteration)) / cycle_total;
    double flop_cycle_dp  = float((FLOP_DP_PER_LOOP * nb_total_loop_iteration)) / cycle_total;
    double flops_sp = (freq * 1000000000 * float((FLOP_SP_PER_LOOP * nb_total_loop_iteration)) / (cycle_total));
    double flops_dp = freq * 1000000000 * float((FLOP_DP_PER_LOOP * nb_total_loop_iteration)) / cycle_total;

    //Applying some correction depending on the cpu clock: if the CPU is capped or has turbo: the rdtsc instruction will not work as expected
    float coef_freq = check_frequency(result);
    if (coef_freq < 0.98){
        IPC /=  coef_freq;
        freq *= coef_freq;
        flop_cycle_sp /= coef_freq;
        flop_cycle_dp /= coef_freq;
    } else if (coef_freq > 1.02){
        IPC /= coef_freq;
        freq *= coef_freq;
    }

    // inst_second = IPC * freq;

    result->bench_frequency = freq;
    result->time_total = time_total;
    result->cycles = cycle_total;
    result->instructions = instructions_total;
    result->ipc = IPC;
    result->flop_cycle_sp = flop_cycle_sp;
    result->flop_cycle_dp = flop_cycle_dp;
    result->flops_sp = flops_sp;
    result->flops_dp = flops_dp;

    // cout << "------------------  INSTRUCTIONS SUMMARY -----------------------" << endl;

    // if (P_COUNT) {
    //     instructions_executed_total -= (NB_INST * NB_lOOP);
    //     cout << setw(20) << "instructions_total" << setw(25) << "instructions_executed"     << setw(20) << "cycle_total" << setw(13) << "FREQUENCY" << setw(10) << "IPC" << std::endl;
    //     cout << setw(25) << instructions_total   << setw(25) << instructions_executed_total << setw(20) << cycle_total   << setw(13) << freq        << setw(10) << IPC * Base_vs_Current_freq << std::endl;

    // } else {
    //     cout << "_label_|" <<  setw(18) <<"NB INSTRUCTIONS"    << setw(10) << "Time"     << setw(13) << "FREQUENCY" << setw(20) << "Giga_inst/sec" << setw(10) << "IPC" << std::endl;
    //     cout << "_value_|" <<  setw(18) <<instructions_total   << setw(10) << time_total << setw(13) << freq        << setw(20) <<   inst_second   << setw(10) << IPC   << std::endl;
    // }

    // cout << endl;

    // cout << "----------------------  FLOP SUMMARY  --------------------------" << endl;
    // cout << setw(10) << "PRECISION"  << setw(15) << "FLOP/cycle"   << setw(20) << "FLOP/second" << endl;
    // cout << setw(10) << "Single"     << setw(15) << flop_cycle_sp  << setw(20) << flops_sp << endl ;
    // cout << setw(10) << "Double"     << setw(15) << flop_cycle_dp  << setw(20) << flops_dp << endl ;
    // cout << "----------------------------------------------------------------" << endl;

    // For now we only save results for a single thread.
    if (omp_get_thread_num() == 0) {
        ofstream mFile_template_start(TMP_FILE_monitoring, std::ios_base::binary);
        for (auto it = pairVec.begin(); it != pairVec.end(); it++) {
            mFile_template_start << it->first << " " << to_string(it->second) << endl;
        }
        mFile_template_start.close();
    }

}
