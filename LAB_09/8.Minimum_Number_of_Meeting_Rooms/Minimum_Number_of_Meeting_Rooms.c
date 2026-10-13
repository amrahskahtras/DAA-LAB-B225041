#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int cmp_ll(const void *a, const void *b) { long long x = *(long long*)a, y = *(long long*)b; return (x > y) - (x < y); }

int min_rooms(long long *s, long long *e, int n) {
    long long *S = malloc(n * sizeof(long long)), *E = malloc(n * sizeof(long long));
    for (int i = 0; i < n; i++) { S[i] = s[i]; E[i] = e[i]; }
    qsort(S, n, sizeof(long long), cmp_ll);
    qsort(E, n, sizeof(long long), cmp_ll);
    int rooms = 0, ep = 0;
    for (int i = 0; i < n; i++) {
        if (E[ep] <= S[i]) ep++; else rooms++;
    }
    free(S); free(E);
    return rooms;
}

void demo(void) {
    long long s1[] = {0, 5, 15}, e1[] = {30, 10, 20};
    long long s2[] = {7, 2}, e2[] = {10, 4};
    long long s3[] = {1, 2, 3, 4}, e3[] = {10, 10, 10, 10};
    printf("[[0,30],[5,10],[15,20]] -> rooms = %d (expected 2)\n", min_rooms(s1, e1, 3));
    printf("[[7,10],[2,4]]          -> rooms = %d (expected 1)\n", min_rooms(s2, e2, 2));
    printf("[[1,10],[2,10],[3,10],[4,10]] -> rooms = %d (expected 4)\n\n", min_rooms(s3, e3, 4));
}

void run_interactive(void) {
    int n;
    printf("Enter number of meetings n: "); if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    long long *s = malloc(n * sizeof(long long)), *e = malloc(n * sizeof(long long));
    for (int i = 0; i < n; i++) {
        printf("Meeting %d (start end): ", i + 1);
        if (scanf("%lld %lld", &s[i], &e[i]) != 2 || e[i] < s[i]) { printf("Invalid.\n"); free(s); free(e); return; }
    }
    printf("\nMinimum rooms required = %d\n", min_rooms(s, e, n));
    free(s); free(e);
}

void run_benchmark(void) {
    FILE *fp = fopen("Minimum_Number_of_Meeting_Rooms.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {1000, 5000, 10000, 50000, 100000, 500000, 1000000, 2000000, 4000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(8);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        long long *st = malloc(n * sizeof(long long)), *en = malloc(n * sizeof(long long));
        for (int i = 0; i < n; i++) { st[i] = rand() % 1000000; en[i] = st[i] + 1 + rand() % 1000; }
        clock_t t0 = clock();
        min_rooms(st, en, n);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(st); free(en);
    }
    fclose(fp);
}

int main(void) {
    printf("Q8: Minimum Number of Meeting Rooms (sort + sweep, O(n log n))\n");
    printf("Choose mode:\n  1 = Enter your own meetings\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Minimum_Number_of_Meeting_Rooms.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}