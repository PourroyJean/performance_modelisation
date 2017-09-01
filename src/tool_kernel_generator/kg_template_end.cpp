
        cycleInEnd = rdtsc();
        timeEnd = mygettime();
        pairArr[i] = make_pair(cycleInEnd - cycleInStart, timeEnd - timeStart);
        cycle_total += cycleInEnd - cycleInStart;
        instructions_executed_total += instructions_executed;
        }
    ofstream mFile_template_start(TMP_FILE_monitoring, std::ios_base::binary);

    instructions_total = (NB_lOOP_IN * NB_lOOP);
    instructions_total *= NB_INST;
    IPC = double(instructions_total) / double (cycle_total);
    instructions_executed_total -= (NB_INST * NB_lOOP);

    cout << setw (25) <<  "instructions_total" << setw (25) << "instructions_executed"  << setw (20) << "cycle_total" << setw (10) <<  "IPC"<<  std::endl;
    cout << setw (25) <<  instructions_total   << setw (25) << instructions_executed_total    << setw (20) << cycle_total   << setw (10) << IPC <<  std::endl;



        for (int i = 0; i < NB_lOOP; ++i) {
        mFile_template_start << pairArr[i].first << " " << to_string(pairArr[i].second) << endl;
//        cout                 << pairArr[i].first << " " << to_string(pairArr[i].second) << endl;
    }
    mFile_template_start.close();

    return 0;
}