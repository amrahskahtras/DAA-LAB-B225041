# DAA Lab-01 — Solutions

Six standalone C programs, one per question. Each compiles with plain `gcc`
and needs no external libraries beyond the standard math library (`-lm`).

## Build & run everything

```bash
gcc -O2 -Wall q1_order_of_growth.c   -o q1 -lm && ./q1
gcc -O2 -Wall q2_coin_simulation.c   -o q2      && ./q2
gcc -O2 -Wall q3_bubble_sort.c       -o q3      && ./q3
gcc -O2 -Wall q4_tower_of_hanoi.c    -o q4      && ./q4
gcc -O2 -Wall q5_partition_point.c   -o q5      && ./q5
gcc -O2 -Wall q6_element_uniqueness.c -o q6     && ./q6
```

Q2, Q3, Q4, Q6 each write a `.csv` file. Run `python3 make_plots.py`
afterwards (needs `matplotlib`) to generate the PNG plots the lab asks for:

```bash
pip install matplotlib --break-system-packages   # if not already installed
python3 make_plots.py
```

## Q1 — Put them in order

Several of the functions (`3^n`, `n^(log2 n)`, `2^(32n)`) overflow a
`double` almost immediately, so instead of computing `f(n)` directly the
program computes **log2(f(n))** analytically for each function (a value
that stays small enough to represent), then sorts by that. Comparing
`log2(f(n))` values gives the same ordering as comparing `f(n))` directly,
without ever overflowing.

Increasing order of growth:

```
1/n  <  log2 n  <  12√n ≈ 50√n  <  n^0.51  <  n log2 n  <  n²−324 ≈ 100n²+6n
   <  2n³  <  n^(log2 n)  <  3^n  <  2^(32n)
```

Note: `12√n`, `50√n` are both Θ(n^0.5); `n²−324` and `100n²+6n` are both
Θ(n²) — within a tie the program breaks ties by the smaller leading
constant. Also note `n^0.51` only truly overtakes `50√n` once
`n > 50^100 ≈ 7.9×10¹⁶⁹` — a good illustration of why Big-O ignores
constants and only cares about behaviour as n → ∞.

## Q2 — Fair vs biased coin

Simulates coin tosses with `rand()`. As the number of tosses grows, the
observed frequency of HEAD converges to the true probability (Law of
Large Numbers) — ~0.5 for the fair coin, ~0.7 for the biased one. See
`q2_coin_plot.png`.

## Q3 — Bubble sort

- **(i)** early-exit version stops as soon as a full pass makes no swaps.
- **(ii)** always runs all `n-1` passes.

On **random** data both are essentially identical (≈ n(n−1)/2
comparisons, i.e. **O(n²)**) because random arrays are almost never
already sorted partway through. The early-exit version's benefit only
shows up on **already-sorted or nearly-sorted** input, where it degrades
gracefully to **O(n)** while the fixed version stays at O(n²) regardless.

## Q4 — Towers of Hanoi

Recursive solution; moves(n) matches the closed form **2ⁿ − 1** exactly
for every n tested (1..20), confirming the recurrence
`T(n) = 2·T(n−1) + 1`. The log-scale plot is a straight line, the
signature of exponential growth — this is why ToH is intractable for
even moderately large n (n = 64 needs > 1.8×10¹⁹ moves).

## Q5 — Partition point

Because the array is sorted (0s then 1s), a binary search finds the
first index containing `1` in **O(log n)** instead of the **O(n)** a
linear scan would need. Verified against a brute-force linear scan on
random test cases.

## Q6 — Element uniqueness

- Brute force (all-pairs) is **O(n²)** in the worst case (no duplicates
  present, so it can never exit early).
- Sort-then-scan-adjacent is **O(n log n)** (dominated by the sort).

To make the comparison fair, the benchmark uses randomly-shuffled
**permutations** (guaranteed no duplicates) so brute force is forced
into its true worst case rather than getting lucky early exits. For
"sufficiently large n" the O(n²) method's comparison count grows far
faster than the O(n log n) method's — the gap widens quickly, e.g. at
n = 8000 brute force needs ~32,000,000 comparisons vs ~8,000 for
sort-then-scan.
