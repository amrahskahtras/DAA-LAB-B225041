#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

static inline int is_even(uint64_t n) { return (n & 1ULL) == 0; }

uint64_t collatz_step(uint64_t n, int *overflow) {
    if (is_even(n)) return n / 2;
    /* check for overflow before computing 3n+1 */
    if (n > (UINT64_MAX - 1) / 3) { *overflow = 1; return 0; }
    return 3 * n + 1;
}

uint64_t *collatz_trajectory(uint64_t n, int *out_len, uint64_t *out_peak, int *out_overflow) {
    int cap = 64, len = 0;
    uint64_t *seq = malloc(cap * sizeof(uint64_t));
    uint64_t peak = n;
    int overflow = 0;
    uint64_t cur = n;
    seq[len++] = cur;
    while (cur != 1) {
        cur = collatz_step(cur, &overflow);
        if (overflow) break;
        if (cur > peak) peak = cur;
        if (len == cap) { cap *= 2; seq = realloc(seq, cap * sizeof(uint64_t)); }
        seq[len++] = cur;
    }
    *out_len = len;
    *out_peak = peak;
    *out_overflow = overflow;
    return seq;
}

void collatz_stats(uint64_t n, int *out_steps, uint64_t *out_peak, int *out_overflow) {
    uint64_t cur = n, peak = n;
    int steps = 0, overflow = 0;
    while (cur != 1) {
        cur = collatz_step(cur, &overflow);
        if (overflow) break;
        if (cur > peak) peak = cur;
        steps++;
    }
    *out_steps = steps;
    *out_peak = peak;
    *out_overflow = overflow;
}

void collatz_scan_interval(uint64_t a, uint64_t b) {
    uint64_t best_n_steps = a, best_n_peak = a;
    int best_steps = -1;
    uint64_t best_peak = 0;
    for (uint64_t n = a; n <= b; n++) {
        int steps, overflow;
        uint64_t peak;
        collatz_stats(n, &steps, &peak, &overflow);
        if (overflow) { printf("  [n=%llu overflowed, skipped]\n", (unsigned long long)n); continue; }
        if (steps > best_steps) { best_steps = steps; best_n_steps = n; }
        if (peak > best_peak) { best_peak = peak; best_n_peak = n; }
        if (n == b) break; 
    }
    printf("Interval [%llu, %llu]:\n", (unsigned long long)a, (unsigned long long)b);
    printf("  Longest trajectory: n=%llu with %d steps\n", (unsigned long long)best_n_steps, best_steps);
    printf("  Highest peak value: n=%llu reaches peak %llu\n", (unsigned long long)best_n_peak, (unsigned long long)best_peak);
}

void demo(void) {
    printf("Single trajectory for n=27 (famous for taking surprisingly long):\n");
    int len, overflow; uint64_t peak;
    uint64_t *seq = collatz_trajectory(27, &len, &peak, &overflow);
    printf("  Steps = %d, Peak value reached = %llu\n  Trajectory: ", len - 1, (unsigned long long)peak);
    for (int i = 0; i < len && i < 20; i++) printf("%llu ", (unsigned long long)seq[i]);
    if (len > 20) printf("... (%d more) ...", len - 20);
    printf("%llu\n", (unsigned long long)seq[len-1]);
    free(seq);

    printf("\nScanning interval [1, 10000] for the record holders:\n");
    collatz_scan_interval(1, 10000);
    printf("\n");
}

void run_interactive(void) {
    printf("Choose:\n  1 = Trajectory of a single n\n  2 = Scan an interval [a,b]\nChoice: ");
    int sub;
    if (scanf("%d", &sub) != 1) return;
    if (sub == 1) {
        uint64_t n;
        printf("Enter starting value n (>=1): ");
        if (scanf("%llu", (unsigned long long *)&n) != 1 || n < 1) { printf("Invalid n.\n"); return; }
        int len, overflow; uint64_t peak;
        uint64_t *seq = collatz_trajectory(n, &len, &peak, &overflow);
        if (overflow) printf("Overflow occurred during the trajectory (n too large / unlucky sequence).\n");
        printf("Steps = %d, Peak value = %llu\nTrajectory: ", len - 1, (unsigned long long)peak);
        for (int i = 0; i < len; i++) printf("%llu ", (unsigned long long)seq[i]);
        printf("\n");
        free(seq);
    } else if (sub == 2) {
        unsigned long long a, b;
        printf("Enter interval start a: ");
        if (scanf("%llu", &a) != 1 || a < 1) { printf("Invalid a.\n"); return; }
        printf("Enter interval end b (b>=a): ");
        if (scanf("%llu", &b) != 1 || b < a) { printf("Invalid b.\n"); return; }
        collatz_scan_interval(a, b);
    } else printf("Invalid choice.\n");
}

void run_benchmark(void) {
    FILE *fp = fopen("Collatz_Conjecture.csv", "w");
    fprintf(fp, "b,time_ms\n");
    uint64_t ends[] = {1000, 5000, 10000, 50000, 100000, 500000, 1000000, 2000000};
    int ns = sizeof(ends) / sizeof(ends[0]);
    for (int s = 0; s < ns; s++) {
        uint64_t b = ends[s];
        clock_t t0 = clock();
        uint64_t best_n_steps = 1; int best_steps = -1;
        for (uint64_t n = 1; n <= b; n++) {
            int steps, overflow; uint64_t peak;
            collatz_stats(n, &steps, &peak, &overflow);
            if (!overflow && steps > best_steps) { best_steps = steps; best_n_steps = n; }
        }
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("b=%8llu  time=%9.3f ms  (longest trajectory in range: n=%llu, %d steps)\n",
               (unsigned long long)b, ms, (unsigned long long)best_n_steps, best_steps);
        fprintf(fp, "%llu,%.4f\n", (unsigned long long)b, ms);
    }
    fclose(fp);
}

int main(void) {
    printf("Q9: Collatz Conjecture trajectory analysis\n");
    printf("Choose mode:\n  1 = Analyse your own n or interval [a,b]\n  2 = Run built-in demo\n  3 = Run timing benchmark scanning [1,b] (writes Collatz_Conjecture.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}
