#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int rod_cutting(int *P, int n, int *pieces, int *num_pieces) {
    int *revenue = calloc(n + 1, sizeof(int));
    int *first_cut = calloc(n + 1, sizeof(int));
    for (int L = 1; L <= n; L++) {
        int best = -1, best_i = -1;
        for (int i = 1; i <= L; i++) {
            int candidate = P[i] + revenue[L - i];
            if (candidate > best) { best = candidate; best_i = i; }
        }
        revenue[L] = best;
        first_cut[L] = best_i;
    }
    int result = revenue[n];
    if (pieces) {
        int L = n, cnt = 0;
        while (L > 0) { pieces[cnt++] = first_cut[L]; L -= first_cut[L]; }
        if (num_pieces) *num_pieces = cnt;
    }
    free(revenue); free(first_cut);
    return result;
}

void demo(void) {
    int P[] = {0, 1, 5, 8, 9, 10, 17, 17, 20}; 
    int n = 8;
    int pieces[64], num;
    int rev = rod_cutting(P, n, pieces, &num);
    printf("Rod length n=%d, prices P[1..8] = {1,5,8,9,10,17,17,20}\n", n);
    printf("Max revenue = %d (expected 22), cut into pieces: ", rev);
    for (int i = 0; i < num; i++) printf("%d ", pieces[i]);
    printf("(expected 2+6 giving 22, or equivalent optimal split)\n\n");
}

void run_interactive(void) {
    int n;
    printf("Enter rod length n: ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    int *P = malloc((n + 1) * sizeof(int));
    P[0] = 0;
    printf("Enter prices P[1..%d] (price of a piece of each length):\n", n);
    for (int i = 1; i <= n; i++) { printf("P[%d]: ", i); if (scanf("%d", &P[i]) != 1) { free(P); return; } }
    int *pieces = malloc(n * sizeof(int)); int num;
    int rev = rod_cutting(P, n, pieces, &num);
    printf("\nMax revenue = %d\nOptimal pieces: ", rev);
    for (int i = 0; i < num; i++) printf("%d ", pieces[i]);
    printf("\n");
    free(P); free(pieces);
}

void run_benchmark(void) {
    FILE *fp = fopen("Rod_Cutting_with_Reconstruction.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {200, 500, 1000, 2000, 4000, 6000, 8000, 10000, 14000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(7);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        int *P = malloc((n + 1) * sizeof(int));
        P[0] = 0;
        for (int i = 1; i <= n; i++) P[i] = i + rand() % 10;
        clock_t t0 = clock();
        rod_cutting(P, n, NULL, NULL);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%6d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(P);
    }
    fclose(fp);
}

int main(void) {
    printf("Q7: Rod Cutting with Reconstruction (DP, O(n^2))\n");
    printf("Choose mode:\n  1 = Enter your own rod length and prices\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Rod_Cutting_with_Reconstruction.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}
