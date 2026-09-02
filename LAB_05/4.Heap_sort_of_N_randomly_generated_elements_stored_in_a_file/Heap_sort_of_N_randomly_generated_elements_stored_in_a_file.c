#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap_int(int *a, int *b) { int t = *a; *a = *b; *b = t; }

void sift_down(int *A, int heap_size, int i) {
    while (1) {
        int left = 2 * i + 1, right = 2 * i + 2, largest = i;
        if (left < heap_size && A[left] > A[largest]) largest = left;
        if (right < heap_size && A[right] > A[largest]) largest = right;
        if (largest == i) break;
        swap_int(&A[i], &A[largest]);
        i = largest;
    }
}

void build_max_heap(int *A, int n) {
    for (int i = n / 2 - 1; i >= 0; i--) sift_down(A, n, i);
}

void heap_sort(int *A, int n) {
    build_max_heap(A, n);
    for (int end = n - 1; end > 0; end--) {
        swap_int(&A[0], &A[end]);   
        sift_down(A, end, 0);
    }
}

int write_array_to_file(const char *path, int *A, int n) {
    FILE *fp = fopen(path, "w");
    if (!fp) return 0;
    for (int i = 0; i < n; i++) fprintf(fp, "%d\n", A[i]);
    fclose(fp);
    return 1;
}

int read_array_from_file(const char *path, int **out) {
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;
    int cap = 1024, n = 0;
    int *A = malloc(cap * sizeof(int));
    int x;
    while (fscanf(fp, "%d", &x) == 1) {
        if (n == cap) { cap *= 2; A = realloc(A, cap * sizeof(int)); }
        A[n++] = x;
    }
    fclose(fp);
    *out = A;
    return n;
}

void demo(void) {
    int A[] = {29, 10, 14, 37, 3, 22, 8, 15};
    int n = 8;
    write_array_to_file("q4_input.txt", A, n);
    printf("Demo: wrote unsorted elements to q4_input.txt\n");

    int *B; int m = read_array_from_file("q4_input.txt", &B);
    heap_sort(B, m);
    write_array_to_file("q4_output.txt", B, m);

    printf("Sorted (also written to q4_output.txt): ");
    for (int i = 0; i < m; i++) printf("%d ", B[i]);
    printf("\n\n");
    free(B);
}

void run_interactive(void) {
    int n;
    printf("Enter N (number of elements): ");
    if (scanf("%d", &n) != 1 || n <= 0) { printf("Invalid N.\n"); return; }
    int *A = malloc(n * sizeof(int));
    printf("Enter %d numbers:\n", n);
    for (int i = 0; i < n; i++) {
        printf("A[%d]: ", i + 1);
        if (scanf("%d", &A[i]) != 1) { printf("Invalid input.\n"); free(A); return; }
    }
    write_array_to_file("q4_input.txt", A, n);
    printf("Wrote input to q4_input.txt\n");

    int *B; int m = read_array_from_file("q4_input.txt", &B);
    heap_sort(B, m);
    write_array_to_file("q4_output.txt", B, m);

    printf("Sorted result (also written to q4_output.txt):\n");
    for (int i = 0; i < m; i++) printf("%d ", B[i]);
    printf("\n");
    free(A); free(B);
}

void run_benchmark(void) {
    FILE *fp = fopen("Heap_sort_of_N_randomly_generated_elements_stored_in_a_file.csv", "w");
    fprintf(fp, "n,time_ms\n");
    int sizes[] = {1000, 5000, 10000, 50000, 100000, 200000, 500000, 1000000, 2000000};
    int ns = sizeof(sizes) / sizeof(sizes[0]);
    srand(4);
    for (int s = 0; s < ns; s++) {
        int n = sizes[s];
        int *A = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) A[i] = rand();
        write_array_to_file("Heap_sort_of_N_randomly_generated_elements_stored_in_a_file_input.txt", A, n); 

        int *B; int m = read_array_from_file("Heap_sort_of_N_randomly_generated_elements_stored_in_a_file_input.txt", &B);
        clock_t t0 = clock();
        heap_sort(B, m);
        clock_t t1 = clock();
        write_array_to_file("Heap_sort_of_N_randomly_generated_elements_stored_in_a_file_output.txt", B, m);

        double ms = 1000.0 * (double)(t1 - t0) / CLOCKS_PER_SEC;
        printf("n=%8d  time=%8.3f ms\n", n, ms);
        fprintf(fp, "%d,%.4f\n", n, ms);
        free(A); free(B);
    }
    fclose(fp);
}

int main(void) {
    printf("Q4: Heap sort of N randomly generated elements stored in a file (O(n log n))\n");
    printf("Choose mode:\n");
    printf("  1 = Enter your own numbers (written to Heap_sort_of_N_randomly_generated_elements_stored_in_a_file_input.txt, sorted to Heap_sort_of_N_randomly_generated_elements_stored_in_a_file_output.txt)\n");
    printf("  2 = Run built-in demo\n");
    printf("  3 = Run timing benchmark (writes Heap_sort_of_N_randomly_generated_elements_stored_in_a_file.csv)\n");
    printf("Choice: ");
    int choice;
    if (scanf("%d", &choice) != 1) return 0;
    srand((unsigned)time(NULL));
    if (choice == 1) run_interactive();
    else if (choice == 2) demo();
    else if (choice == 3) run_benchmark();
    else printf("Invalid choice.\n");
    return 0;
}
