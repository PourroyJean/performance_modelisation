#include <stdio.h>
#include <omp.h>

#define N 20

int main(int argv, char **argc) {
    int x[N];
    int y[N];
    int errors = 0;
    // initialize
    for (int i = 0; i < N; i++) {
        y[i] = 0;
        x[i] = i;
    }

    // splits the "N" in two, gives half to each team
#pragma omp target teams map(from:y[:N]) map(to:x[:N]) num_teams(2)
#pragma omp distribute
    for (int j = 0; j < N; j += N / 2) {
#pragma omp parallel num_threads(4)
#pragma omp for
        for (int i = j; i < j + N / 2; i++) {
            y[i] = x[i];
            int team = omp_get_team_num();
            int nteams = omp_get_num_teams();
            int tid = omp_get_thread_num();
            int nthreads = omp_get_num_threads();

            printf("Iteration %d given to thread %d from team %d.\n",
                   i, tid, team);

        }

    }

    // error check
    for (int i = 0; i < N; i++) {
        if (y[i] != x[i]) errors++;
    }

    if (errors != 0)
        printf("Failed!\n");
    else
        printf("Success!\n");

    return 0;
}