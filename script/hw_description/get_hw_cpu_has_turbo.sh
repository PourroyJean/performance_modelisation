#!/usr/bin/env bash


//TODO
printf("%s\t%d\t%4.2f\t\t\t%u", test, tool_freq_parameters->P_BIND, ipc, (unsigned int) ((double) freq * ipc));
    if (ipc > 1.0001)
        printf("\t\t(Turbo ON)");
    else
        printf("\t\t(Turbo OFF)");
