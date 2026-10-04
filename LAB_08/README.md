# DAA Lab-08 — Dynamic Programming

**Design and Analysis of Algorithms (DAA)**
BTech (CS-B and CE), 3rd Semester

Nine classic Dynamic Programming problems (medium–hard), implemented in C with correctness demos cross-checked against known textbook/OEIS values, interactive input, and empirical benchmarks plotted against each algorithm's theoretical complexity.

---

## 📋 Problems & complexity summary

| # | Problem | Technique | Complexity |
|---|---------|-----------|------------|
| 1 | Minimum Coin Change | Unbounded-knapsack DP | **O(n·V)** time, O(V) space |
| 2 | Coin Change — total ways | Unbounded-knapsack DP (coin-outer loop ⇒ combinations not permutations) | **O(n·V)** time, O(V) space |
| 3 | Longest Common Subsequence | 2D DP + traceback | **O(m·n)** time & space |
| 4 | Longest Increasing Subsequence | Patience sorting (binary search on tails) | **O(n log n)** time, O(n) space |
| 5 | Maximum Sum Increasing Subsequence | DP (no known n log n generalization) | **O(n²)** time, O(n) space |
| 6 | Edit Distance + traceback | 2D DP + traceback | **O(m·n)** time & space |
| 7 | Rod Cutting + reconstruction | 1D "try every first cut" DP | **O(n²)** time, O(n) space |
| 8 | Optimal Binary Search Tree | Interval DP (CLRS) | **O(n³)** time, O(n²) space |
| 9 | Collatz Conjecture | Iterative trajectory simulation | No closed form (open problem); empirical only |

---

## 🧠 Algorithm notes

**Q1/Q2 (Coin Change):** Both are "unbounded knapsack" style — each coin denomination can be reused without limit. The key difference: Q1's inner loop tries every coin for every amount (minimizing count); Q2 deliberately loops **coins on the outside, amounts on the inside** — this ordering is what makes `1+2` and `2+1` collapse into the same combination (combinations, not permutations).

**Q3 (LCS) / Q6 (Edit Distance):** Both are the standard 2D CLRS-style DP tables, each with a traceback pass that walks backward from `dp[m][n]` to reconstruct the actual subsequence / sequence of edit operations.

**Q4 (LIS):** Uses the O(n log n) **patience sorting** technique (not the straightforward O(n²) DP) — maintain the smallest possible tail value for every achievable subsequence length, and binary-search it for each new element.

**Q5 (MSIS):** Unlike plain LIS, maximizing the *sum* (not just the length) breaks the greedy "smallest tail" invariant that makes patience sorting work, so the standard approach stays at O(n²).

**Q7 (Rod Cutting):** Classic unbounded DP trying every possible length for the *first* piece cut off, recursing on the remainder; reconstructs the actual cut lengths via a parent array.

**Q8 (OBST):** The classic CLRS O(n³) interval DP over `e[i][j]` (expected cost of the optimal tree on keys i..j), trying every possible root. Knuth's root-monotonicity observation can reduce this to O(n²), not implemented here.

**Q9 (Collatz):** An open mathematical conjecture with no known closed-form step-count formula — this is *why* it's open. The program is built modularly (single-step function, full-trajectory function with dynamic allocation, O(1)-space stats function for interval scans) and defensively checks for 64-bit overflow before computing `3n+1`.

---

## 📁 Repository structure

```
daa_lab08/
├── q1_min_coin_change.c
├── q2_coin_change_ways.c
├── q3_lcs.c
├── q4_lis.c
├── q5_msis.c
├── q6_edit_distance.c
├── q7_rod_cutting.c
├── q8_obst.c
├── q9_collatz.c
├── q1_timing.csv … q9_timing.csv
├── q1_plot.png … q9_plot.png, combined_plot.png
├── plot_all.py
└── README.md
```

---

## ⚙️ Build & run

```bash
for f in q1_min_coin_change q2_coin_change_ways q3_lcs q4_lis q5_msis q6_edit_distance q7_rod_cutting q8_obst q9_collatz; do
    gcc -O2 -o $f $f.c -lm
done
```

Every program shows the same menu:

```
Choose mode:
  1 = Enter your own input
  2 = Run built-in demo
  3 = Run timing benchmark (writes the CSV used for the plot)
```

Regenerate all plots: `python3 plot_all.py` (needs matplotlib, numpy).

---

## ✅ Correctness checks (all verified against known values)

| # | Test case | Result | Matches |
|---|-----------|--------|---------|
| 1 | Coins `{1,5,6,8}`, V=11 | 2 coins (5+6) | ✅ classic textbook example |
| 2 | Coins `{1,2,3}`, V=4 | 4 ways | ✅ hand-verified |
| 3 | `AGGTAB` / `GXTXAYB` | LCS=`GTAB`, length 4 | ✅ classic CLRS example |
| 4 | `[10,9,2,5,3,7,101,18]` | LIS=`[2,3,7,18]`, length 4 | ✅ classic LeetCode example |
| 5 | `[1,101,2,3,100,4,5]` | sum=106, seq=`[1,2,3,100]` | ✅ classic example |
| 6 | `SUNDAY` → `SATURDAY` | distance=3 | ✅ classic textbook example |
| 7 | n=8, prices `{1,5,8,9,10,17,17,20}` | revenue=22, cuts 2+6 | ✅ exact CLRS example |
| 8 | CLRS 5-key example | cost=2.75, root=k2 | ✅ exact CLRS example & tree structure |
| 9 | n=27 trajectory; interval `[1,10000]` | 111 steps/peak 9232; record n=6171 (261 steps) | ✅ matches published OEIS records |

---

## 📊 What each plot shows

Each `qN_plot.png` overlays the measured runtime against a scaled theoretical reference curve for that algorithm's complexity class (O(V), O(n log n), O(n²), or O(n³) as applicable). `q9_plot.png` shows the empirical Collatz interval-scan time, which has **no known closed-form complexity** — this is noted explicitly in the plot title. `combined_plot.png` overlays Q1–Q8 for a side-by-side comparison of how differently each complexity class scales.

---

## 👤 Author

BTech CS-B / CE — 3rd Semester
Design and Analysis of Algorithms (DAA), Lab-08
