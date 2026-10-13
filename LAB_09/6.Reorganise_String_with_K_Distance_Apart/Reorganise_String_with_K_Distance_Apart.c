#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct { int cnt; unsigned char ch; } HE;
static HE hp[256]; static int hn;
static int better(HE a, HE b) { return a.cnt > b.cnt || (a.cnt == b.cnt && a.ch < b.ch); }
static void hpush(HE x) {
    int i = hn++; hp[i] = x;
    while (i > 0) { int p = (i - 1) / 2; if (better(hp[i], hp[p])) { HE t = hp[i]; hp[i] = hp[p]; hp[p] = t; i = p; } else break; }
}
static HE hpop(void) {
    HE top = hp[0]; hp[0] = hp[--hn]; int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, m = i;
        if (l < hn && better(hp[l], hp[m])) m = l;
        if (r < hn && better(hp[r], hp[m])) m = r;
        if (m == i) break;
        HE t = hp[i]; hp[i] = hp[m]; hp[m] = t; i = m;
    }
    return top;
}

int reorganize_k(const char *S, int K, char *out) {
    int n = strlen(S);
    if (K <= 1) { strcpy(out, S); return 1; }
    int cnt[256] = {0};
    for (int i = 0; i < n; i++) cnt[(unsigned char)S[i]]++;
    hn = 0;
    for (int c = 0; c < 256; c++) if (cnt[c]) { HE e = {cnt[c], (unsigned char)c}; hpush(e); }
    HE qe[512]; int qready[512]; int qh = 0, qt = 0;
    for (int i = 0; i < n; i++) {
        while (qh < qt && qready[qh] <= i) { hpush(qe[qh]); qh++; }
        if (hn == 0) { out[0] = '\0'; return 0; }
        HE e = hpop();
        out[i] = e.ch;
        e.cnt--;
        if (e.cnt > 0) { qe[qt] = e; qready[qt] = i + K; qt++; if (qt == 512) { memmove(qe, qe + qh, (qt - qh) * sizeof(HE)); memmove(qready, qready + qh, (qt - qh) * sizeof(int)); qt -= qh; qh = 0; } }
    }
    out[n] = '\0';
    return 1;
}

int valid(const char *res, const char *S, int K) {
    int n = strlen(S);
    if ((int)strlen(res) != n) return 0;
    int a[256] = {0}, b[256] = {0};
    for (int i = 0; i < n; i++) { a[(unsigned char)S[i]]++; b[(unsigned char)res[i]]++; }
    for (int c = 0; c < 256; c++) if (a[c] != b[c]) return 0;
    int last[256]; for (int c = 0; c < 256; c++) last[c] = -1000000;
    for (int i = 0; i < n; i++) { unsigned char c = res[i]; if (i - last[c] < K) return 0; last[c] = i; }
    return 1;
}

void demo(void) {
    const char *tests[] = {"aabbcc", "aaabc", "aaadbbcc", "aaabbbccc"};
    int Ks[] = {3, 3, 2, 3};
    for (int t = 0; t < 4; t++) {
        char out[256];
        int ok = reorganize_k(tests[t], Ks[t], out);
        printf("S=\"%s\", K=%d -> %s", tests[t], Ks[t], ok ? out : "\"\" (impossible)");
        if (ok) printf("   [valid=%s]", valid(out, tests[t], Ks[t]) ? "YES" : "NO");
        printf("\n");
    }
    printf("\n");
}

void run_interactive(void) {
    char S[4096]; int K;
    printf("Enter string S: "); if (scanf("%4095s", S) != 1) return;
    printf("Enter K: "); if (scanf("%d", &K) != 1 || K < 0) return;
    char *out = malloc(strlen(S) + 2);
    if (reorganize_k(S, K, out)) printf("\nRearranged: %s\n", out);
    else printf("\nImpossible -> empty string \"\"\n");
    free(out);
}

void run_benchmark(void) {
    FILE *fp = fopen("Reorganise_String_with_K_Distance_Apart.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {1000, 10000, 100000, 500000, 1000000, 2000000, 5000000, 10000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(6);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        char *S = malloc(n + 1), *out = malloc(n + 1);
        for (int i = 0; i < n; i++) S[i] = 'a' + rand() % 26; 
        S[n] = '\0';
        clock_t t0 = clock();
        reorganize_k(S, 5, out);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%9d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(S); free(out);
    }
    fclose(fp);
}

int main(void) {
    printf("Q6: Reorganise String K-Distance Apart (greedy + max-heap, O(n log sigma))\n");
    printf("Choose mode:\n  1 = Enter your own S and K\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Reorganise_String_with_K_Distance_Apart.csv, K=5)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}