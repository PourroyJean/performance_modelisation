
        //--------------
        cycleInEnd = rdtsc();
        timeEnd = mygettime();
        pairArr[i] = make_pair(cycleInEnd - cycleInStart, timeEnd - timeStart);
    }
    FILE *P_FPT = fopen(TMP_FILE_monitoring, "w+");

    for (int i = 0; i < 100; ++i) {
        string tmp = to_string(pairArr[i].first) + " " + to_string(pairArr[i].second) + "\n";
        fprintf(P_FPT, tmp.c_str());
        std::cout << pairArr[i].first << " - " << pairArr[i].second <<endl;
    }
    //cout << pairArr[1].first <<endl;
    nbCycleIn = cycleInEnd - cycleInStart;
    timeSpent = timeEnd - timeStart;
    unsigned int freq = nbCycleIn / (1000000 * (timeEnd - timeStart));
    //cout << nbCycleIn << ' ' << fixed << setprecision(3) << freq << endl;
    return 0;
}