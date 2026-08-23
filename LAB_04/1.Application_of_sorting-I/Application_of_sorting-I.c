#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct { int number; int color; } Item;
void sort_by_color(Item *in, Item *out, int n) {
    int count[3] = {0, 0, 0};
    for (int i = 0; i < n; i++) count[in[i].color]++;
    int pos[3];
    pos[0] = 0;
    pos[1] = pos[0] + count[0];
    pos[2] = pos[1] + count[1];
    for (int i = 0; i < n; i++) {
        int c = in[i].color;
        out[pos[c]++] = in[i];
    }
}

int read_color_token(void) {
    char tok[32];
    if (scanf("%31s", tok) != 1) return -1;
    if (tok[0] == 'R' || tok[0] == 'r' || tok[0] == '0') return 0;
    if (tok[0] == 'B' || tok[0] == 'b' || tok[0] == '1') return 1;
    if (tok[0] == 'Y' || tok[0] == 'y' || tok[0] == '2') return 2;
    return -1; /* invalid */
}

const char *color_name(int c) {
    return c == 0 ? "Red" : (c == 1 ? "Blue" : "Yellow");
}

void run_interactive(void) {
    int n;
    printf("Enter number of pairs (n): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid n.\n");
        return;
    }

    Item *in = malloc(n * sizeof(Item));
    Item *out = malloc(n * sizeof(Item));

    printf("Enter the %d pairs IN INCREASING ORDER OF NUMBER, as: <number> <color>\n", n);
    printf("(color can be R/B/Y or 0/1/2 for Red/Blue/Yellow)\n");
    for (int i = 0; i < n; i++) {
        printf("Pair %d: ", i + 1);
        int num;
        if (scanf("%d", &num) != 1) { printf("Invalid number.\n"); free(in); free(out); return; }
        int c = read_color_token();
        if (c == -1) { printf("Invalid color.\n"); free(in); free(out); return; }
        in[i].number = num;
        in[i].color = c;
    }

    int sorted_ok = 1;
    for (int i = 1; i < n; i++) if (in[i].number < in[i-1].number) sorted_ok = 0;
    if (!sorted_ok) {
        printf("\nWarning: the numbers you entered are not sorted; the algorithm assumes\n");
        printf("sorted input (per the question), so the 'stability' guarantee may not hold.\n");
    }

    sort_by_color(in, out, n);

    printf("\nInput (number, color), given sorted by number:\n");
    for (int i = 0; i < n; i++) printf("  (%d, %s)\n", in[i].number, color_name(in[i].color));

    printf("\nOutput sorted by colour (all Red, then all Blue, then all Yellow),\n");
    printf("numbers within each colour remain sorted:\n");
    for (int i = 0; i < n; i++) printf("  (%d, %s)\n", out[i].number, color_name(out[i].color));

    free(in); free(out);
}

void demo(void) {
    Item in[6] = {{1,0},{2,1},{3,2},{4,0},{5,1},{6,2}}; 
    Item out[6];
    sort_by_color(in, out, 6);
    printf("Demo input (number,color) sorted by number:\n");
    for (int i = 0; i < 6; i++) printf("(%d,%d) ", in[i].number, in[i].color);
    printf("\nOutput sorted by color (0=R,1=B,2=Y), numbers stay sorted within a color:\n");
    for (int i = 0; i < 6; i++) printf("(%d,%d) ", out[i].number, out[i].color);
    printf("\n\n");
}

void run_benchmark(void) {
    FILE *fp = fopen("Application_of_sorting-I.csv", "w");
    fprintf(fp, "n,time_ms\n");

    int sizes[] = {1000, 5000, 10000, 50000, 100000, 200000, 500000, 1000000, 2000000, 4000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);

    srand(42);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        Item *in = malloc(n * sizeof(Item));
        Item *out = malloc(n * sizeof(Item));
        for (int i = 0; i < n; i++) {
            in[i].number = i;                 
            in[i].color = rand() % 3;
        }
        clock_t t0 = clock();
        sort_by_color(in, out, n);
        clock_t t1 = clock();
        double ms = 1000.0 * (double)(t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%8.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(in); free(out);
    }
    fclose(fp);
}

int main(void) {
    printf("Q1: Sort n (number,color) pairs by colour in O(n)\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own input and see it sorted by colour\n");
    printf("  2 = Run built-in demo\n");
    printf("  3 = Run timing benchmark (writes Application_of_sorting-I.csv, used for the report plot)\n");
    printf("Choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;

    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");

    return 0;
}