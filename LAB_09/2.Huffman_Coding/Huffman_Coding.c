#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct { long long freq; int left, right; int order; } Node;

static Node *nodes; static int *heap; static int hsize;

static int less(int a, int b) {
    if (nodes[a].freq != nodes[b].freq) return nodes[a].freq < nodes[b].freq;
    return nodes[a].order < nodes[b].order;
}
static void sift_up(int i) {
    while (i > 0) { int p = (i - 1) / 2; if (less(heap[i], heap[p])) { int t = heap[i]; heap[i] = heap[p]; heap[p] = t; i = p; } else break; }
}
static void sift_down(int i) {
    while (1) {
        int l = 2*i+1, r = 2*i+2, m = i;
        if (l < hsize && less(heap[l], heap[m])) m = l;
        if (r < hsize && less(heap[r], heap[m])) m = r;
        if (m == i) break;
        int t = heap[i]; heap[i] = heap[m]; heap[m] = t; i = m;
    }
}
static void push(int x) { heap[hsize++] = x; sift_up(hsize - 1); }
static int pop(void) { int x = heap[0]; heap[0] = heap[--hsize]; if (hsize) sift_down(0); return x; }

void huffman_lengths(long long *freq, int n, int *len) {
    if (n == 1) { len[0] = 1; return; }
    nodes = malloc((2 * n) * sizeof(Node));
    heap = malloc((2 * n) * sizeof(int));
    hsize = 0;
    for (int i = 0; i < n; i++) { nodes[i].freq = freq[i]; nodes[i].left = nodes[i].right = -1; nodes[i].order = i; push(i); }
    int total = n;
    while (hsize > 1) {
        int a = pop(), b = pop();
        nodes[total].freq = nodes[a].freq + nodes[b].freq;
        nodes[total].left = a; nodes[total].right = b; nodes[total].order = total;
        push(total++);
    }
    int root = pop();
    int *depth = calloc(total, sizeof(int));
    for (int i = root; i >= 0; i--) {
        if (nodes[i].left != -1) { depth[nodes[i].left] = depth[i] + 1; depth[nodes[i].right] = depth[i] + 1; }
    }
    for (int i = 0; i < n; i++) len[i] = depth[i];
    free(depth); free(nodes); free(heap);
}

typedef struct { int sym_idx; int len; } SL;
int cmp_sl(const void *a, const void *b) {
    const SL *x = a, *y = b;
    if (x->len != y->len) return x->len - y->len;
    return x->sym_idx - y->sym_idx;
}

void canonical_codes(int *len, int n, unsigned long long *code) {
    SL *arr = malloc(n * sizeof(SL));
    for (int i = 0; i < n; i++) { arr[i].sym_idx = i; arr[i].len = len[i]; }
    qsort(arr, n, sizeof(SL), cmp_sl);
    unsigned long long c = 0; int prev_len = arr[0].len;
    code[arr[0].sym_idx] = 0;
    for (int k = 1; k < n; k++) {
        c = (c + 1) << (arr[k].len - prev_len);
        prev_len = arr[k].len;
        code[arr[k].sym_idx] = c;
    }
    free(arr);
}

void print_codebook(char *sym, long long *freq, int n) {
    int *len = malloc(n * sizeof(int));
    unsigned long long *code = malloc(n * sizeof(unsigned long long));
    huffman_lengths(freq, n, len);
    canonical_codes(len, n, code);
    SL *arr = malloc(n * sizeof(SL));
    for (int i = 0; i < n; i++) { arr[i].sym_idx = i; arr[i].len = len[i]; }
    qsort(arr, n, sizeof(SL), cmp_sl);
    long long total = 0, wsum = 0;
    printf("Symbol  Freq   Len  Code\n");
    for (int k = 0; k < n; k++) {
        int i = arr[k].sym_idx;
        printf("  %c    %5lld   %2d   ", sym[i], freq[i], len[i]);
        for (int b = len[i] - 1; b >= 0; b--) putchar(((code[i] >> b) & 1) ? '1' : '0');
        printf("\n");
        total += freq[i]; wsum += freq[i] * len[i];
    }
    printf("Expected code length = %.4f bits/symbol\n", (double)wsum / total);
    free(len); free(code); free(arr);
}

void demo(void) {
    char sym[] = {'a','b','c','d','e','f'};
    long long freq[] = {45, 13, 12, 16, 9, 5};
    printf("CLRS example frequencies a:45 b:13 c:12 d:16 e:9 f:5\n");
    print_codebook(sym, freq, 6);
    printf("(expected 2.24 bits/symbol; a=0, b=100, c=101, d=110, e=1110, f=1111)\n\n");
}

void run_interactive(void) {
    int n;
    printf("Enter number of symbols n: ");
    if (scanf("%d", &n) != 1 || n <= 0 || n > 62) { printf("Invalid n (1..62).\n"); return; }
    char *sym = malloc(n); long long *freq = malloc(n * sizeof(long long));
    printf("Enter <symbol> <frequency> for each:\n");
    for (int i = 0; i < n; i++) {
        printf("Symbol %d: ", i + 1);
        if (scanf(" %c %lld", &sym[i], &freq[i]) != 2 || freq[i] <= 0) { printf("Invalid.\n"); free(sym); free(freq); return; }
    }
    print_codebook(sym, freq, n);
    free(sym); free(freq);
}

void run_benchmark(void) {
    FILE *fp = fopen("Huffman_Coding.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {1000, 5000, 10000, 50000, 100000, 200000, 500000, 1000000, 2000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(2);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        long long *freq = malloc(n * sizeof(long long));
        int *len = malloc(n * sizeof(int));
        unsigned long long *code = malloc(n * sizeof(unsigned long long));
        for (int i = 0; i < n; i++) freq[i] = 1 + rand() % 1000000;
        clock_t t0 = clock();
        huffman_lengths(freq, n, len);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        (void)code;
        printf("n=%8d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(freq); free(len); free(code);
    }
    fclose(fp);
}

int main(void) {
    printf("Q2: Huffman Coding + canonical codebook (greedy, O(n log n))\n");
    printf("Choose mode:\n  1 = Enter your own symbols/frequencies\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Huffman_Coding.csv; tree construction)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}