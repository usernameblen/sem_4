import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import sys
import os

fixed_p_file = "fixed_p.csv"
fixed_n_file = "fixed_n.csv"

if not os.path.exists(fixed_p_file) or not os.path.exists(fixed_n_file):
    print("CSV files not found. Run sum_calc first.")
    sys.exit(1)

df_n = pd.read_csv(fixed_p_file)  # фиксированный p, меняется n
df_p = pd.read_csv(fixed_n_file)  # фиксированный n, меняется p

fig, axes = plt.subplots(1, 2, figsize=(14, 6))
fig.patch.set_facecolor("#0f0f14")

colors = {"ops_direct": "#e05c5c", "ops_asympt": "#5cb8e0", "ops_bernoulli": "#7ee05c"}
labels = {"ops_direct": "Direct (Def.)", "ops_asympt": "Asymptotic (Euler)", "ops_bernoulli": "Bernoulli (Exact)"}

ax1 = axes[0]
ax1.set_facecolor("#16161e")
for col in ["ops_direct", "ops_asympt", "ops_bernoulli"]:
    ax1.plot(df_n["n"], df_n[col], label=labels[col], color=colors[col], linewidth=2)
ax1.set_xlabel("n", color="#cccccc", fontsize=12)
ax1.set_ylabel("Operations count", color="#cccccc", fontsize=12)
ax1.set_title("Operations vs n", color="#ffffff", fontsize=14, fontweight="bold")
ax1.legend(facecolor="#1e1e28", edgecolor="#444", labelcolor="#dddddd", fontsize=10)
ax1.tick_params(colors="#aaaaaa")
ax1.spines["bottom"].set_color("#444")
ax1.spines["left"].set_color("#444")
ax1.spines["top"].set_color("#444")
ax1.spines["right"].set_color("#444")
ax1.yaxis.set_major_formatter(ticker.FuncFormatter(lambda x, _: f"{int(x):,}"))
ax1.grid(True, color="#2a2a3a", linestyle="--", linewidth=0.5)

ax2 = axes[1]
ax2.set_facecolor("#16161e")
for col in ["ops_direct", "ops_asympt", "ops_bernoulli"]:
    ax2.plot(df_p["p"], df_p[col], label=labels[col], color=colors[col], linewidth=2, marker="o", markersize=5)
ax2.set_xlabel("p", color="#cccccc", fontsize=12)
ax2.set_ylabel("Operations count", color="#cccccc", fontsize=12)
ax2.set_title("Operations vs p", color="#ffffff", fontsize=14, fontweight="bold")
ax2.legend(facecolor="#1e1e28", edgecolor="#444", labelcolor="#dddddd", fontsize=10)
ax2.tick_params(colors="#aaaaaa")
ax2.spines["bottom"].set_color("#444")
ax2.spines["left"].set_color("#444")
ax2.spines["top"].set_color("#444")
ax2.spines["right"].set_color("#444")
ax2.yaxis.set_major_formatter(ticker.FuncFormatter(lambda x, _: f"{int(x):,}"))
ax2.grid(True, color="#2a2a3a", linestyle="--", linewidth=0.5)

plt.suptitle("S(n,p) = Σ k^p — Operation Count Comparison", color="#ffffff", fontsize=15, fontweight="bold", y=1.02)
plt.tight_layout()
plt.savefig("ops_combined.png", dpi=150, bbox_inches="tight", facecolor="#0f0f14")
print("Saved: ops_combined.png")

# --- Средние для fixed_n.csv (фиксированный n, меняется p) ---
avg_direct_n    = df_p["ops_direct"].mean()
avg_asympt_n    = df_p["ops_asympt"].mean()
avg_bernoulli_n = df_p["ops_bernoulli"].mean()

print("\n--- Average operation counts (fixed_n.csv) ---")
print(f"  Direct:    {avg_direct_n:>12,.1f}")
print(f"  Asymptot:  {avg_asympt_n:>12,.1f}")
print(f"  Bernoulli: {avg_bernoulli_n:>12,.1f}")

best_n = min(
    [("Direct", avg_direct_n), ("Asymptotic (Euler)", avg_asympt_n), ("Bernoulli (Exact)", avg_bernoulli_n)],
    key=lambda x: x[1]
)
print(f"Most efficient (fixed n): {best_n[0]}")

# --- Средние для fixed_p.csv (фиксированный p, меняется n) ---
avg_direct_p    = df_n["ops_direct"].mean()
avg_asympt_p    = df_n["ops_asympt"].mean()
avg_bernoulli_p = df_n["ops_bernoulli"].mean()

print("\n--- Average operation counts (fixed_p.csv) ---")
print(f"  Direct:    {avg_direct_p:>12,.1f}")
print(f"  Asymptot:  {avg_asympt_p:>12,.1f}")
print(f"  Bernoulli: {avg_bernoulli_p:>12,.1f}")

best_p = min(
    [("Direct", avg_direct_p), ("Asymptotic (Euler)", avg_asympt_p), ("Bernoulli (Exact)", avg_bernoulli_p)],
    key=lambda x: x[1]
)
print(f"Most efficient (fixed p): {best_p[0]}")

print("\nNote: Asymptotic method is approximate (O(n^(p-1)) error).")
print("      Bernoulli method is exact with O(p^2) operations independent of n.")
print("      Direct method has O(n * log p) operations — grows linearly with n.")