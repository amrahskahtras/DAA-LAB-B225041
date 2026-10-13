#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct { long long d, f; } Station;

static long long *hp; static int hn;
static void hpush(long long x) {
    int i = hn++; hp[i] = x;
    while (i > 0) { int p = (i - 1) / 2; if (hp[p] < hp[i]) { long long t = hp[p]; hp[p] = hp[i]; hp[i] = t; i = p; } else break; }
}
static long long hpop(void) {
    long long top = hp[0]; hp[0] = hp[--hn]; int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, m = i;
        if (l < hn && hp[l] > hp[m]) m = l;
        if (r < hn && hp[r] > hp[m]) m = r;
        if (m == i) break;
        long long t = hp[i]; hp[i] = hp[m]; hp[m] = t; i = m;
    }
    return top;
}

int cmp_station(const void *a, const void *b) {
    long long x = ((Station*)a)->d, y = ((Station*)b)->d;
    return (x > y) - (x < y);
}
int min_refuel_stops(long long D, long long F, Station *st, int n) {
    Station *s = malloc((n + 1) * sizeof(Station));
    for (int i = 0; i < n; i++) s[i] = st[i];
    qsort(s, n, sizeof(Station), cmp_station);
    s[n].d = D; s[n].f = 0; 
    hp = malloc((n + 1) * sizeof(long long)); hn = 0;
    long long fuel = F, prev = 0; int stops = 0;
    for (int i = 0; i <= n; i++) {
        fuel -= (s[i].d - prev);
        while (fuel < 0 && hn > 0) { fuel += hpop(); stops++; }
        if (fuel < 0) { free(s); free(hp); return -1; }
        hpush(s[i].f);
        prev = s[i].d;
    }
    free(s); free(hp);
    return stops;
}

void demo(void) {
    Station st[] = {{10,60},{20,30},{30,30},{60,40}};
    printf("D=100, F=10, stations (10,60)(20,30)(30,30)(60,40) -> stops = %d (expected 2)\n",
           min_refuel_stops(100, 10, st, 4));
    printf("D=1, F=1, no stations -> stops = %d (expected 0)\n", min_refuel_stops(1, 1, NULL, 0));
    Station st2[] = {{10,100}};
    printf("D=100, F=1, station (10,100) -> stops = %d (expected -1, can't reach first station)\n\n",
           min_refuel_stops(100, 1, st2, 1));
}

void run_interactive(void) {
    long long D, F; int n;
    printf("Enter target distance D: "); if (scanf("%lld", &D) != 1) return;
    printf("Enter starting fuel F: "); if (scanf("%lld", &F) != 1) return;
    printf("Enter number of stations n: "); if (scanf("%d", &n) != 1 || n < 0) return;
    Station *st = malloc((n + 1) * sizeof(Station));
    for (int i = 0; i < n; i++) {
        printf("Station %d (distance fuel): ", i + 1);
        if (scanf("%lld %lld", &st[i].d, &st[i].f) != 2) { free(st); return; }
    }
    int r = min_refuel_stops(D, F, st, n);
    if (r == -1) printf("\nTarget cannot be reached.\n"); else printf("\nMinimum refuelling stops = %d\n", r);
    free(st);
}

void run_benchmark(void) {
    FILE *fp = fopen("Minimum_Initial_Fuel_Reverse_Greedy.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {1000, 5000, 10000, 50000, 100000, 500000, 1000000, 2000000, 4000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(3);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        Station *st = malloc(n * sizeof(Station));
        for (int i = 0; i < n; i++) { st[i].d = 1 + rand() % (n * 10LL); st[i].f = 1 + rand() % 50; }
        long long D = n * 10LL + 5;
        clock_t t0 = clock();
        min_refuel_stops(D, 1000000, st, n);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(st);
    }
    fclose(fp);
}

int main(void) {
    printf("Q3: Minimum refuelling stops (reverse greedy + max-heap, O(n log n))\n");
    printf("Choose mode:\n  1 = Enter your own D, F, stations\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Minimum_Initial_Fuel_Reverse_Greedy.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}