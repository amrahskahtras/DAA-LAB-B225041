/*
 * coin_sim.c
 *
 * Q2: Fair vs Biased coin.
 *   - Show empirically that P(HEAD) for a fair coin -> 0.5 as
 *     the number of tosses (trials) grows.
 *   - Extend to a biased coin (P(HEAD) = p != 0.5) and compare.
 *
 * Method: for each trial count N in {100, 1000, 10000, 100000, 1000000},
 * run an INDEPENDENT experiment of N tosses and report the observed
 * proportion of heads. As N grows, the observed proportion should get
 * closer to the true probability p (law of large numbers).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Simulate a single coin toss with P(head) = p. Returns 1 for head, 0 for tail. */
int toss(double p) {
    double r = (double) rand() / ((double) RAND_MAX + 1.0); /* uniform in [0,1) */
    return (r < p) ? 1 : 0;
}

/* Run N independent tosses for a coin with given bias p.
 * Returns the observed proportion of heads. */
double run_experiment(double p, long N) {
    long heads = 0;
    for (long i = 0; i < N; i++) {
        heads += toss(p);
    }
    return (double) heads / (double) N;
}

int main(void) {
    srand((unsigned) time(NULL));

    long trials[] = {100, 1000, 10000, 100000, 1000000};
    int num_trials = sizeof(trials) / sizeof(trials[0]);

    FILE *csv = fopen("coin_data.csv", "w");
    if (!csv) { perror("fopen"); return 1; }
    fprintf(csv, "trials,fair_p,biased70_p,biased30_p\n");

    printf("%10s | %-12s | %-14s | %-14s\n", "trials", "Fair(0.50)", "Biased(0.70)", "Biased(0.30)");
    printf("--------------------------------------------------------------\n");

    for (int i = 0; i < num_trials; i++) {
        long N = trials[i];
        double fair    = run_experiment(0.50, N);
        double biased7 = run_experiment(0.70, N);
        double biased3 = run_experiment(0.30, N);

        printf("%10ld | %-12.5f | %-14.5f | %-14.5f\n", N, fair, biased7, biased3);
        fprintf(csv, "%ld,%.6f,%.6f,%.6f\n", N, fair, biased7, biased3);
    }

    fclose(csv);
    printf("\nData written to coin_data.csv\n");
    return 0;
}