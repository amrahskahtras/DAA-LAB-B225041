#include <stdio.h>
#include <stdlib.h>

long hanoi(int n, char from_peg, char to_peg, char aux_peg, int print_moves) {
    if (n == 0) return 0;   

    long moves = 0;
    moves += hanoi(n - 1, from_peg, aux_peg, to_peg, print_moves);

    if (print_moves) {
        printf("Move disc %d from %c -> %c\n", n, from_peg, to_peg);
    }
    moves += 1;

    moves += hanoi(n - 1, aux_peg, to_peg, from_peg, print_moves);
    return moves;
}

int main(void) {
    printf("=== Demo: n = 3 discs (moves printed) ===\n");
    hanoi(3, 'A', 'C', 'B', 1);
    FILE *csv = fopen("hanoi_data.csv", "w");
    if (!csv) { perror("fopen"); return 1; }
    fprintf(csv, "n,moves,formula_2n_minus_1\n");

    printf("\n=== Move count vs n ===\n");
    printf("%4s | %-12s | %-12s\n", "n", "moves(sim)", "2^n - 1");
    for (int n = 1; n <= 25; n++) {
        long moves = hanoi(n, 'A', 'C', 'B', 0);   
        long formula = (1L << n) - 1;              
        printf("%4d | %-12ld | %-12ld\n", n, moves, formula);
        fprintf(csv, "%d,%ld,%ld\n", n, moves, formula);
    }

    fclose(csv);
    printf("\nData written to hanoi_data.csv\n");
    return 0;
}