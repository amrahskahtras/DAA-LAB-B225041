#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct { long long x, y; } Interval;

int cmp_interval(const void *a, const void *b) {
    const Interval *ia = a, *ib = b;
    if (ia->x != ib->x) return (ia->x < ib->x) ? -1 : 1;
    return 0;
}

int merge_intervals(Interval *in, int n, Interval *out) {
    if (n == 0) return 0;
    Interval *tmp = malloc(n * sizeof(Interval));
    for (int i = 0; i < n; i++) tmp[i] = in[i];
    qsort(tmp, n, sizeof(Interval), cmp_interval);

    int m = 0;
    long long curL = tmp[0].x, curR = tmp[0].y;
    for (int i = 1; i < n; i++) {
        if (tmp[i].x <= curR) {
            if (tmp[i].y > curR) curR = tmp[i].y;
        } else {
            out[m].x = curL; out[m].y = curR; m++;
            curL = tmp[i].x; curR = tmp[i].y;
        }
    }
    out[m].x = curL; out[m].y = curR; m++;
    free(tmp);
    return m;
}

void demo(void) {
    Interval in[] = {{1,3},{2,6},{8,10},{7,18}};
    Interval out[4];
    int m = merge_intervals(in, 4, out);
    printf("Demo: I = {(1,3),(2,6),(8,10),(7,18)} -> merged = {");
    for (int i = 0; i < m; i++) printf("(%lld,%lld)%s", out[i].x, out[i].y, i+1<m?",":"");
    printf("}\n\n");
}

void run_interactive(void) {
    int n;
    printf("Enter number of intervals (n): ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }

    Interval *in = malloc(n * sizeof(Interval));
    Interval *out = malloc(n * sizeof(Interval));

    printf("Enter %d intervals as: <x_i> <y_i>\n", n);
    for (int i = 0; i < n; i++) {
        printf("Interval %d: ", i + 1);
        if (scanf("%lld %lld", &in[i].x, &in[i].y) != 2) {
            printf("Invalid input.\n"); free(in); free(out); return;
        }
    }

    int m = merge_intervals(in, n, out);
    printf("\nMerged intervals: {");
    for (int i = 0; i < m; i++) printf("(%lld,%lld)%s", out[i].x, out[i].y, i+1<m?",":"");
    printf("}\n");

    free(in); free(out);
}

int main(void) {
    printf("Q5: Merge overlapping intervals (O(n log n))\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own intervals\n");
    printf("  2 = Run built-in demo\n");
    printf("  3 = Run timing benchmark (writes Application_of_sorting-V.csv)\n");
    printf("Choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) { run_interactive(); return 0; }
    else if (choice == 2) { demo(); return 0; }
    else if (choice != 3) { printf("Invalid choice.\n"); return 0; }

    FILE *fp = fopen("Application_of_sorting-V.csv", "w");
    fprintf(fp, "n,time_ms\n");

    int sizes[] = {1000, 5000, 10000, 50000, 100000, 200000, 500000, 1000000, 2000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(23);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        Interval *in = malloc(n * sizeof(Interval));
        Interval *out = malloc(n * sizeof(Interval));
        for (int i = 0; i < n; i++) {
            long long l = rand() % (n * 2 + 1);
            long long len = 1 + rand() % 50;
            in[i].x = l; in[i].y = l + len;
        }
        clock_t t0 = clock();
        merge_intervals(in, n, out);
        clock_t t1 = clock();
        double ms = 1000.0 * (double)(t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%8.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(in); free(out);
    }
    fclose(fp);
    return 0;
}