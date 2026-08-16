#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef long long ll;

ll **alloc_mat(int n) {
    ll **m = malloc(n * sizeof(ll *));
    for (int i = 0; i < n; i++) m[i] = calloc(n, sizeof(ll));
    return m;
}
void free_mat(ll **m, int n) {
    for (int i = 0; i < n; i++) free(m[i]);
    free(m);
}
void add_mat(ll **A, ll **B, ll **C, int n) {
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) C[i][j] = A[i][j] + B[i][j];
}
void sub_mat(ll **A, ll **B, ll **C, int n) {
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) C[i][j] = A[i][j] - B[i][j];
}


long std_mults; 
void standard_multiply(ll **A, ll **B, ll **C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            ll sum = 0;
            for (int k = 0; k < n; k++) { sum += A[i][k] * B[k][j]; std_mults++; }
            C[i][j] = sum;
        }
}

long strassen_mults; 

void strassen(ll **A, ll **B, ll **C, int n) {
    if (n == 1) {
        C[0][0] = A[0][0] * B[0][0];
        strassen_mults++;
        return;
    }

    int half = n / 2;
    ll **A11=alloc_mat(half), **A12=alloc_mat(half), **A21=alloc_mat(half), **A22=alloc_mat(half);
    ll **B11=alloc_mat(half), **B12=alloc_mat(half), **B21=alloc_mat(half), **B22=alloc_mat(half);

    for (int i = 0; i < half; i++) for (int j = 0; j < half; j++) {
        A11[i][j]=A[i][j];           A12[i][j]=A[i][j+half];
        A21[i][j]=A[i+half][j];      A22[i][j]=A[i+half][j+half];
        B11[i][j]=B[i][j];           B12[i][j]=B[i][j+half];
        B21[i][j]=B[i+half][j];      B22[i][j]=B[i+half][j+half];
    }

    ll **T1=alloc_mat(half), **T2=alloc_mat(half);
    ll **M1=alloc_mat(half), **M2=alloc_mat(half), **M3=alloc_mat(half), **M4=alloc_mat(half);
    ll **M5=alloc_mat(half), **M6=alloc_mat(half), **M7=alloc_mat(half);

    add_mat(A11,A22,T1,half); add_mat(B11,B22,T2,half); strassen(T1,T2,M1,half);
    add_mat(A21,A22,T1,half);                            strassen(T1,B11,M2,half);
    sub_mat(B12,B22,T2,half);                            strassen(A11,T2,M3,half);
    sub_mat(B21,B11,T2,half);                            strassen(A22,T2,M4,half);
    add_mat(A11,A12,T1,half);                             strassen(T1,B22,M5,half);
    sub_mat(A21,A11,T1,half); add_mat(B11,B12,T2,half); strassen(T1,T2,M6,half);
    sub_mat(A12,A22,T1,half); add_mat(B21,B22,T2,half); strassen(T1,T2,M7,half);

    ll **C11=alloc_mat(half), **C12=alloc_mat(half), **C21=alloc_mat(half), **C22=alloc_mat(half);
    ll **X=alloc_mat(half), **Y=alloc_mat(half);

    add_mat(M1,M4,X,half); sub_mat(X,M5,Y,half); add_mat(Y,M7,C11,half);
    add_mat(M3,M5,C12,half);
    add_mat(M2,M4,C21,half);
    sub_mat(M1,M2,X,half); add_mat(X,M3,Y,half); add_mat(Y,M6,C22,half);

    for (int i = 0; i < half; i++) for (int j = 0; j < half; j++) {
        C[i][j]=C11[i][j]; C[i][j+half]=C12[i][j];
        C[i+half][j]=C21[i][j]; C[i+half][j+half]=C22[i][j];
    }

    ll **mats[] = {A11,A12,A21,A22,B11,B12,B21,B22,T1,T2,
                    M1,M2,M3,M4,M5,M6,M7,C11,C12,C21,C22,X,Y};
    for (int i = 0; i < 23; i++) free_mat(mats[i], half);
}

void fill_random(ll **m, int n) {
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) m[i][j] = rand() % 10;
}
int matrices_equal(ll **A, ll **B, int n) {
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) if (A[i][j] != B[i][j]) return 0;
    return 1;
}

int main(void) {
    srand((unsigned) time(NULL));

    FILE *csv = fopen("Matrix_multiplication_using_DandC_Approach.csv", "w");
    fprintf(csv, "n,standard_mults,strassen_mults,n_cubed,n_log2_7\n");

    int sizes[] = {2, 4, 8, 16, 32, 64, 128, 256};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("%6s | %-14s | %-14s | %-14s | %-14s\n", "n", "standard", "strassen", "n^3", "n^log2(7)");
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        ll **A = alloc_mat(n), **B = alloc_mat(n);
        ll **C_std = alloc_mat(n), **C_str = alloc_mat(n);
        fill_random(A, n); fill_random(B, n);

        std_mults = 0;
        standard_multiply(A, B, C_std, n);

        strassen_mults = 0;
        strassen(A, B, C_str, n);

        int correct = matrices_equal(C_std, C_str, n);

        double n3 = pow(n, 3);
        double nlog7 = pow(n, log2(7));

        printf("%6d | %-14ld | %-14ld | %-14.0f | %-14.1f  (correct: %s)\n",
               n, std_mults, strassen_mults, n3, nlog7, correct ? "yes" : "NO");
        fprintf(csv, "%d,%ld,%ld,%.0f,%.2f\n", n, std_mults, strassen_mults, n3, nlog7);

        free_mat(A,n); free_mat(B,n); free_mat(C_std,n); free_mat(C_str,n);
    }

    fclose(csv);
    printf("\nData written to strassen_data.csv\n");
    return 0;
}