#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct { long long time; int delta; } Event;

int cmp_event(const void *a, const void *b) {
    const Event *ea = a, *eb = b;
    if (ea->time != eb->time) return (ea->time < eb->time) ? -1 : 1;
    return 0; 
}

int max_simultaneous(long long *a, long long *b, int n, long long *out_time) {
    Event *ev = malloc(2 * n * sizeof(Event));
    for (int i = 0; i < n; i++) {
        ev[2*i].time = a[i];   ev[2*i].delta = +1;
        ev[2*i+1].time = b[i]; ev[2*i+1].delta = -1;
    }
    qsort(ev, 2 * n, sizeof(Event), cmp_event);
    int count = 0, best = 0;
    long long best_time = 0;
    for (int i = 0; i < 2 * n; i++) {
        count += ev[i].delta;
        if (count > best) { best = count; best_time = ev[i].time; }
    }
    free(ev);
    if (out_time) *out_time = best_time;
    return best;
}

void demo(void) {
    long long a[] = {1, 2, 4, 7};
    long long b[] = {5, 3, 9, 8};
    long long t;
    int m = max_simultaneous(a, b, 4, &t);
    printf("Demo: max simultaneous = %d people, achieved at time %lld\n\n", m, t);
}


void run_interactive(void) {
    int n;
    printf("Enter number of persons (n): ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }

    long long *a = malloc(n * sizeof(long long));
    long long *b = malloc(n * sizeof(long long));

    printf("Enter %d pairs as: <entry_time a_i> <exit_time b_i>  (b_i > a_i, all times distinct)\n", n);
    for (int i = 0; i < n; i++) {
        printf("Person %d: ", i + 1);
        if (scanf("%lld %lld", &a[i], &b[i]) != 2 || b[i] <= a[i]) {
            printf("Invalid input (need b_i > a_i).\n");
            free(a); free(b); return;
        }
    }

    long long t;
    int m = max_simultaneous(a, b, n, &t);
    printf("\nMax simultaneous attendees = %d, achieved at time %lld\n", m, t);

    free(a); free(b);
}

int main(void) {
    printf("Q4: Time with the most people simultaneously present (O(n log n) sweep)\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own entry/exit times\n");
    printf("  2 = Run built-in demo\n");
    printf("  3 = Run timing benchmark (writes Application_of_sorting-IV.csv)\n");
    printf("Choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) { run_interactive(); return 0; }
    else if (choice == 2) { demo(); return 0; }
    else if (choice != 3) { printf("Invalid choice.\n"); return 0; }

    FILE *fp = fopen("Application_of_sorting-IV.csv", "w");
    fprintf(fp, "n,time_ms\n");

    int sizes[] = {1000, 5000, 10000, 50000, 100000, 200000, 500000, 1000000, 2000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(11);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        long long *a = malloc(n * sizeof(long long));
        long long *b = malloc(n * sizeof(long long));
        for (int i = 0; i < n; i++) {
            long long lo = (long long)(rand()) * (n / 100 + 1) + i * 2LL; 
            a[i] = lo;
            b[i] = lo + 1 + rand() % (n + 1);
        }
        long long t;
        clock_t t0 = clock();
        max_simultaneous(a, b, n, &t);
        clock_t t1 = clock();
        double ms = 1000.0 * (double)(t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%8.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(a); free(b);
    }
    fclose(fp);
    return 0;
}