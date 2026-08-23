#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct { long long coord; int delta; } Event; 

int cmp_event(const void *a, const void *b) {
    const Event *ea = a, *eb = b;
    if (ea->coord != eb->coord) return (ea->coord < eb->coord) ? -1 : 1;
    return eb->delta - ea->delta;
}

int max_coverage_point(long long *l, long long *r, int n, long long *out_point) {
    Event *ev = malloc(2 * n * sizeof(Event));
    for (int i = 0; i < n; i++) {
        ev[2*i].coord = l[i];   ev[2*i].delta = +1;
        ev[2*i+1].coord = r[i]; ev[2*i+1].delta = -1;
    }
    qsort(ev, 2 * n, sizeof(Event), cmp_event);

    int count = 0, best = 0;
    long long best_point = 0;
    int i = 0;
    int total = 2 * n;
    while (i < total) {
        long long coord = ev[i].coord;
        int j = i;
        while (j < total && ev[j].coord == coord && ev[j].delta == +1) { count++; j++; }
        if (count > best) { best = count; best_point = coord; }
        while (j < total && ev[j].coord == coord && ev[j].delta == -1) { count--; j++; }
        i = j;
    }
    free(ev);
    if (out_point) *out_point = best_point;
    return best;
}

void demo(void) {
    long long l[] = {10, 20, 50, 15};
    long long r[] = {40, 60, 90, 70};
    long long p;
    int m = max_coverage_point(l, r, 4, &p);
    printf("Demo: S={(10,40),(20,60),(50,90),(15,70)} -> max coverage=%d at point p=%lld\n\n", m, p);
}

void run_interactive(void) {
    int n;
    printf("Enter number of intervals (n): ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }

    long long *l = malloc(n * sizeof(long long));
    long long *r = malloc(n * sizeof(long long));

    printf("Enter %d intervals as: <l_i> <r_i>  (endpoints inclusive)\n", n);
    for (int i = 0; i < n; i++) {
        printf("Interval %d: ", i + 1);
        if (scanf("%lld %lld", &l[i], &r[i]) != 2) {
            printf("Invalid input.\n"); free(l); free(r); return;
        }
    }

    long long p;
    int m = max_coverage_point(l, r, n, &p);
    printf("\nMax coverage = %d intervals, achieved at point p = %lld\n", m, p);

    free(l); free(r);
}

int main(void) {
    printf("Q6: Point covered by the largest number of intervals (O(n log n) sweep)\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own intervals\n");
    printf("  2 = Run built-in demo\n");
    printf("  3 = Run timing benchmark (writes Application_of_sorting-VI.csv)\n");
    printf("Choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) { run_interactive(); return 0; }
    else if (choice == 2) { demo(); return 0; }
    else if (choice != 3) { printf("Invalid choice.\n"); return 0; }

    FILE *fp = fopen("Application_of_sorting-VI.csv", "w");
    fprintf(fp, "n,time_ms\n");

    int sizes[] = {1000, 5000, 10000, 50000, 100000, 200000, 500000, 1000000, 2000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(99);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        long long *l = malloc(n * sizeof(long long));
        long long *r = malloc(n * sizeof(long long));
        for (int i = 0; i < n; i++) {
            long long lo = rand() % (n * 2 + 1);
            long long len = 1 + rand() % 100;
            l[i] = lo; r[i] = lo + len;
        }
        long long p;
        clock_t t0 = clock();
        max_coverage_point(l, r, n, &p);
        clock_t t1 = clock();
        double ms = 1000.0 * (double)(t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%8.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(l); free(r);
    }
    fclose(fp);
    return 0;
}