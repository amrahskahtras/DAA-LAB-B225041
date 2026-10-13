#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct { double v, w, lambda; } Item;
static char g_winner = '?';

int cmp_lambda_desc(const void *a, const void *b) {
    double la = ((Item*)a)->lambda, lb = ((Item*)b)->lambda;
    return (la < lb) - (la > lb);
}


static double strat_lambda_fill(Item *items, int n, double W, Item *out_order, double *out_frac, int *out_count) {
    Item *sorted = malloc(n * sizeof(Item));
    for (int i = 0; i < n; i++) sorted[i] = items[i];
    qsort(sorted, n, sizeof(Item), cmp_lambda_desc);

    double T = 0; 
    double remaining = W;
    double total_value = 0;
    int cnt = 0;
    for (int i = 0; i < n && remaining > 1e-12; i++) {
        double density = sorted[i].v / sorted[i].w - sorted[i].lambda * T;
        if (density <= 0) { continue; } 
        double take = (sorted[i].w <= remaining) ? sorted[i].w : remaining;
        double frac = take / sorted[i].w;
        total_value += density * take;
        if (out_order) out_order[cnt] = sorted[i];
        if (out_frac) out_frac[cnt] = frac;
        cnt++;
        T += take;
        remaining -= take;
    }
    if (out_count) *out_count = cnt;
    free(sorted);
    return total_value;
}


int cmp_density_desc(const void *a, const void *b) {
    double da = ((Item*)a)->v / ((Item*)a)->w, db = ((Item*)b)->v / ((Item*)b)->w;
    return (da < db) - (da > db);
}

typedef struct { Item it; double amt; } Chosen;
int cmp_chosen_lambda_desc(const void *a, const void *b) {
    double x = ((const Chosen*)a)->it.lambda, y = ((const Chosen*)b)->it.lambda;
    return (x < y) - (x > y);
}

static double strat_density_select(Item *items, int n, double W, Item *out_order, double *out_frac, int *out_count) {
    Item *by_d = malloc(n * sizeof(Item));
    for (int i = 0; i < n; i++) by_d[i] = items[i];
    qsort(by_d, n, sizeof(Item), cmp_density_desc);
    Chosen *ch = malloc(n * sizeof(Chosen)); int m = 0;
    double remaining = W;
    for (int i = 0; i < n && remaining > 1e-12; i++) {
        double take = (by_d[i].w <= remaining) ? by_d[i].w : remaining;
        ch[m].it = by_d[i]; ch[m].amt = take; m++; remaining -= take;
    }
    qsort(ch, m, sizeof(Chosen), cmp_chosen_lambda_desc);
    double T = 0, total = 0; int cnt = 0;
    for (int k = 0; k < m; k++) {
        double density = ch[k].it.v / ch[k].it.w - ch[k].it.lambda * T;
        if (density <= 0) continue;
        total += density * ch[k].amt;
        if (out_order) out_order[cnt] = ch[k].it;
        if (out_frac) out_frac[cnt] = ch[k].amt / ch[k].it.w;
        cnt++; T += ch[k].amt;
    }
    if (out_count) *out_count = cnt;
    free(ch); free(by_d);
    return total;
}

double fractional_knapsack_decay(Item *items, int n, double W, Item *out_order, double *out_frac, int *out_count) {
    Item *oa = malloc(n * sizeof(Item)), *ob = malloc(n * sizeof(Item));
    double *fa = malloc(n * sizeof(double)), *fb = malloc(n * sizeof(double));
    int ca = 0, cb = 0;
    double va = strat_lambda_fill(items, n, W, oa, fa, &ca);
    double vb = strat_density_select(items, n, W, ob, fb, &cb);
    double best; Item *o; double *f; int c;
    if (va >= vb) { best = va; o = oa; f = fa; c = ca; g_winner = 'A'; }
    else          { best = vb; o = ob; f = fb; c = cb; g_winner = 'B'; }
    if (out_order) for (int i = 0; i < c; i++) out_order[i] = o[i];
    if (out_frac) for (int i = 0; i < c; i++) out_frac[i] = f[i];
    if (out_count) *out_count = c;
    free(oa); free(ob); free(fa); free(fb);
    return best;
}

void demo(void) {
    Item items[] = {
        {60, 10, 0.5},  
        {100, 20, 0.1},
        {120, 30, 0.9},
    };
    int n = 3;
    double W = 50;
    Item out_order[3]; double out_frac[3]; int cnt;
    double val = fractional_knapsack_decay(items, n, W, out_order, out_frac, &cnt);
    printf("Items (v,w,lambda): (60,10,0.5) (100,20,0.1) (120,30,0.9), W=%.0f\n", W);
    printf("Processing order (by lambda desc) and fraction taken:\n");
    for (int i = 0; i < cnt; i++)
        printf("  v=%.1f w=%.1f lambda=%.2f -> take fraction %.4f\n",
               out_order[i].v, out_order[i].w, out_order[i].lambda, out_frac[i]);
    printf("Total value = %.4f  (winning strategy: %c)\n", val, g_winner);
    Item cx[] = {{1, 10, 5.0}, {100, 10, 0.1}};
    Item o2[2]; double f2[2]; int c2;
    double v2 = fractional_knapsack_decay(cx, 2, 10, o2, f2, &c2);
    printf("Counterexample items (1,10,5.0) (100,10,0.1), W=10 -> total value = %.4f (strategy %c; pure lambda-fill alone would give 1.0)\n\n", v2, g_winner);
}

void run_interactive(void) {
    int n;
    printf("Enter number of items n: ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid n.\n"); return; }
    Item *items = malloc(n * sizeof(Item));
    printf("Enter v_i w_i lambda_i for each item:\n");
    for (int i = 0; i < n; i++) {
        printf("Item %d (v w lambda): ", i);
        if (scanf("%lf %lf %lf", &items[i].v, &items[i].w, &items[i].lambda) != 3) { free(items); return; }
    }
    double W;
    printf("Enter capacity W: ");
    if (scanf("%lf", &W) != 1) { free(items); return; }

    Item *out_order = malloc(n * sizeof(Item)); double *out_frac = malloc(n * sizeof(double)); int cnt;
    double val = fractional_knapsack_decay(items, n, W, out_order, out_frac, &cnt);
    printf("\nProcessing order and fraction taken:\n");
    for (int i = 0; i < cnt; i++)
        printf("  v=%.2f w=%.2f lambda=%.2f -> take fraction %.4f\n",
               out_order[i].v, out_order[i].w, out_order[i].lambda, out_frac[i]);
    printf("Total value = %.4f  (winning strategy: %c)\n", val, g_winner);
    free(items); free(out_order); free(out_frac);
}

void run_benchmark(void) {
    FILE *fp = fopen("Fractional_Knapsack_with_Deterioration_Rate.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {1000, 5000, 10000, 50000, 100000, 500000, 1000000, 2000000, 4000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(1);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        Item *items = malloc(n * sizeof(Item));
        for (int i = 0; i < n; i++) {
            items[i].v = 1 + rand() % 1000;
            items[i].w = 1 + rand() % 100;
            items[i].lambda = (rand() % 1000) / 100.0;
        }
        double W = n * 10.0;
        clock_t t0 = clock();
        fractional_knapsack_decay(items, n, W, NULL, NULL, NULL);
        clock_t t1 = clock();
        double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%9.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(items);
    }
    fclose(fp);
}

int main(void) {
    printf("Q1: Fractional Knapsack with Deterioration Rate (greedy, O(n log n))\n");
    printf("Choose mode:\n  1 = Enter your own items\n  2 = Run built-in demo\n  3 = Run timing benchmark (writes Fractional_Knapsack_with_Deterioration_Rate.csv)\nChoice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}