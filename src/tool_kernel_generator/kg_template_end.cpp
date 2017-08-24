        //--------------
//        cycleInEnd = rdtsc();
//        timeEnd = mygettime();
//        pairArr[i] = make_pair(cycleInEnd - cycleInStart, timeEnd - timeStart);
    }
    ofstream mFile_template_start (TMP_FILE_monitoring, std::ios_base::binary);


    for (int i = 0; i < NB_lOOP; ++i) {
        mFile_template_start << pairArr[i].first << " " << to_string(pairArr[i].second) << endl;
//        cout                 << pairArr[i].first << " " << to_string(pairArr[i].second) << endl;
    }
    mFile_template_start.close();

    return 0;
}