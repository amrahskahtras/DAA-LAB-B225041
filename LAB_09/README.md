# DAA Lab-09 — Greedy Algorithms

**Design and Analysis of Algorithms (DAA)** — BTech (CS-B and CE), 3rd Semester

Ten greedy-paradigm problems implemented in C. Every program has three modes (your own input / built-in demo / timing benchmark that writes a CSV), and each question has its own graph comparing measured runtime to its theoretical complexity.

## Problems and complexity

| # | Problem | Greedy idea | Time |
|---|---------|-------------|------|
| 1 | Fractional knapsack with decay | Order by decay rate λ (proved by exchange argument); item selection is heuristic (see note) | O(n log n) |
| 2 | Huffman coding + canonical codebook | Merge two least-frequent nodes (min-heap); canonical codes from lengths | O(n log n) |
| 3 | Minimum refuelling stops | Reverse greedy: refuel retroactively at the largest passed station (max-heap) | O(n log n) |
| 4 | Connect sticks | Always join the two shortest (min-heap) — Huffman-style | O(n log n) |
| 5 | Candy distribution | Left-to-right then right-to-left pass, take the max | O(n) |
| 6 | Reorganise string, K apart | Max-heap + cooldown queue | O(n log σ) |
| 7 | Minimise deviation | Double all odds, then repeatedly halve the max (max-heap) | O(n log n · log M) |
| 8 | Minimum meeting rooms | Sort starts/ends separately and sweep | O(n log n) |
| 9 | Hu-Tucker (optimal alphabetic tree) | Merge min-weight *compatible* pair; levels → ordered tree | O(n²) here (O(n log n) possible) |
| 10 | Greedy superstring (open problem) | Merge max-overlap pair; exact bitmask DP as checker | O(n³) selection + overlap cost |

## Honest notes on the harder questions

**Q1 – what is and isn't proven.** Exchange argument: for any fixed set of items, consuming them in *decreasing λ* is optimal (the value difference of swapping adjacent items i, j is `w_i·w_j·(λ_i − λ_j)`). Choosing *which* items to take when capacity W is binding is a different matter, and no single greedy key is optimal: "fill in λ order" earns 1 on `(v,w,λ) = (1,10,5), (100,10,0.1)`, W=10 (taking item 2 alone earns 100), while "pick by v/w, then order by λ" earns 140 vs 160 on the demo. The program runs both (still O(n log n)) and returns the better; it is a heuristic with a proven ordering step, not a claimed global optimum when W binds. Time `t` is modelled as cumulative weight consumed before an item.

**Q9 – validation.** Hu-Tucker was checked against an independent exact O(n³) interval DP on **24,000 random instances** (n ≤ 14, weights from tiny ranges so ties are common): 0 cost mismatches, and the phase-3 recombination reproduced a valid ordered tree every time. The phase-1 sweep finds the best compatible pair in O(n), giving O(n²) overall; the measured runtime scales accordingly.

**Q10 – status of the conjecture.** The Tarhio–Ukkonen conjecture says greedy is a 2-approximation. The ratio is *at least* 2 on the classic family `{c(ab)^m, (ab)^m c, (ba)^m}`, which the demo/benchmark reproduces (ratio 1.994 at m=500, → 2). A September 2026 arXiv preprint (H. Shibata, arXiv:2609.01365) claims a ratio of at least 9/4 for equal-length strings of even length k ≥ 10. It is an unreviewed preprint, listed as provisional by at least one tracker, and **its construction is not reproduced here**; random instances stay well below 2 (worst ratio seen in 3000 random trials: 1.40), which is why a counterexample needs engineered structure. The proven upper bound for greedy is 3.425 (Englert–Matsakis–Veselý, STOC 2022).

## Build and run

```bash
for f in q1_knapsack_decay q2_huffman q3_min_refuel q4_connect_sticks q5_candy \
         q6_reorganize_k q7_min_deviation q8_meeting_rooms q9_hu_tucker q10_superstring; do
  gcc -std=c11 -O2 -o $f $f.c
done
./q2_huffman          # menu: 1 = your input, 2 = demo, 3 = benchmark (writes CSV)
python3 plot_all.py   # needs matplotlib, numpy
```

## Verified examples

| # | Input | Result |
|---|-------|--------|
| 1 | `(60,10,0.5) (100,20,0.1) (120,30,0.9)`, W=50 | 160 (λ order 0.9 → 0.1) |
| 2 | CLRS `a:45 b:13 c:12 d:16 e:9 f:5` | 2.24 bits; `0,100,101,110,1110,1111` |
| 3 | D=100, F=10, `(10,60)(20,30)(30,30)(60,40)` | 2 stops |
| 4 | `[2,4,3]`, `[1,8,3,5]` | 14, 30 |
| 5 | `[1,0,2]`, `[1,2,2]`, `[1,3,4,5,2]` | 5, 4, 11 |
| 6 | `aabbcc` K=3; `aaabc` K=3 | `abcabc` (validated); impossible |
| 7 | `[1,2,3,4]`, `[4,1,5,20,3]`, `[2,10,8]` | 1, 3, 3 |
| 8 | `[[0,30],[5,10],[15,20]]` | 2 rooms |
| 9 | `1..8`; `9,1,1,9,1,1,9` | 102; 77 (both match exact DP) |
| 10 | `{c(ab)^m,...}` | greedy 4m+2 vs optimal 2m+4 |

Q10 was also run under AddressSanitizer/UBSan with no findings.

## Files

`q1_…q10_*.c` sources · `qN_timing.csv` data · `qN_plot.png` one graph per question · `q9_comparison_plot.png` (Hu-Tucker vs DP) · `q10_ratio_plot.png` + `q10_ratio.csv` (greedy ratio family) · `plot_all.py`.

*Timings come from a single run on one machine and include normal noise (e.g. cache effects at multi-million sizes).*
