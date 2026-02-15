import re
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np


DATA_DIR = Path("data")
PLOTS_DIR = Path("plots")

# Filters (set to None to disable a filter)
FITNESS_FILTER = "LeadingOnes"
T_FILTER = 100000
N_FILTER = 100
M_FILTER = 100
SIG_EPS_FILTER = None
CGA_K_FILTER = None

SIG_PATTERN = re.compile(
    r"^sig_cga(?:_test)?_fit-(?P<fitness>.+?)_T-(?P<T>\d+)_n-(?P<n>\d+)_M-(?P<M>\d+)_eps-(?P<eps>[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)\.csv$"
)
CGA_PATTERN = re.compile(
    r"^cga_fit-(?P<fitness>.+?)_T-(?P<T>\d+)_n-(?P<n>\d+)_M-(?P<M>\d+)_K-(?P<K>[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)\.csv$"
)
EA_PATTERN = re.compile(
    r"^ea1_1_fit-(?P<fitness>.+?)_T-(?P<T>\d+)_n-(?P<n>\d+)_M-(?P<M>\d+)\.csv$"
)


def read_rows(csv_path: Path):
    rows = []
    with csv_path.open("r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split(",")
            rows.append((float(parts[0]), float(parts[1])))
    return rows


def match_filters(meta):
    if FITNESS_FILTER is not None and meta["fitness"] != FITNESS_FILTER:
        return False
    if T_FILTER is not None and meta["T"] != T_FILTER:
        return False
    if N_FILTER is not None and meta["n"] != N_FILTER:
        return False
    if M_FILTER is not None and meta["M"] != M_FILTER:
        return False
    return True


def mean_ci(vals):
    vals = np.array(vals, dtype=float)
    mean = float(np.mean(vals))
    if len(vals) <= 1:
        return mean, 0.0
    std = float(np.std(vals, ddof=1))
    se = std / np.sqrt(len(vals))
    return mean, 1.96 * se  # 95% CI (normal approximation)


def _add_metric_values(values, rows, metric):
    if metric == "score":
        values.extend(score for score, _ in rows)
    else:
        values.extend(runtime for _, runtime in rows)


def _choose_best_key(variants):
    # Pick the variant with highest average score.
    best_key = None
    best_mean = -np.inf
    for key, rows in variants.items():
        if not rows:
            continue
        mean_score = float(np.mean([score for score, _ in rows]))
        if mean_score > best_mean:
            best_mean = mean_score
            best_key = key
    return best_key


def _collect_algorithm_values(metric):
    collected = {"sig_cGA": [], "cGA": [], "EA(1,1)": []}
    selected = {"sig_cGA": None, "cGA": None}
    sig_variants = {}
    cga_variants = {}

    for p in DATA_DIR.glob("*.csv"):
        name = p.name

        m = SIG_PATTERN.match(name)
        if m:
            meta = {
                "fitness": m.group("fitness"),
                "T": int(m.group("T")),
                "n": int(m.group("n")),
                "M": int(m.group("M")),
            }
            if match_filters(meta):
                eps = float(m.group("eps"))
                sig_variants.setdefault(eps, []).extend(read_rows(p))
            continue

        m = CGA_PATTERN.match(name)
        if m:
            meta = {
                "fitness": m.group("fitness"),
                "T": int(m.group("T")),
                "n": int(m.group("n")),
                "M": int(m.group("M")),
            }
            if match_filters(meta):
                k_val = float(m.group("K"))
                cga_variants.setdefault(k_val, []).extend(read_rows(p))
            continue

        m = EA_PATTERN.match(name)
        if m:
            meta = {
                "fitness": m.group("fitness"),
                "T": int(m.group("T")),
                "n": int(m.group("n")),
                "M": int(m.group("M")),
            }
            if match_filters(meta):
                _add_metric_values(collected["EA(1,1)"], read_rows(p), metric)

    sig_key = SIG_EPS_FILTER
    if sig_key is None:
        sig_key = _choose_best_key(sig_variants)
    if sig_key is not None:
        for eps, rows in sig_variants.items():
            if np.isclose(eps, sig_key):
                _add_metric_values(collected["sig_cGA"], rows, metric)
                selected["sig_cGA"] = eps
                break

    cga_key = CGA_K_FILTER
    if cga_key is None:
        cga_key = _choose_best_key(cga_variants)
    if cga_key is not None:
        for k_val, rows in cga_variants.items():
            if np.isclose(k_val, cga_key):
                _add_metric_values(collected["cGA"], rows, metric)
                selected["cGA"] = k_val
                break

    return collected, selected


def compare_epsilons():
    PLOTS_DIR.mkdir(parents=True, exist_ok=True)
    grouped = {}

    for p in DATA_DIR.glob("sig_cga*.csv"):
        m = SIG_PATTERN.match(p.name)
        if not m:
            continue
        meta = {
            "fitness": m.group("fitness"),
            "T": int(m.group("T")),
            "n": int(m.group("n")),
            "M": int(m.group("M")),
        }
        if not match_filters(meta):
            continue
        eps = float(m.group("eps"))
        if SIG_EPS_FILTER is not None and not np.isclose(eps, SIG_EPS_FILTER):
            continue
        grouped.setdefault(eps, []).extend(score for score, _ in read_rows(p))

    if not grouped:
        print("No matching sig_cga csv files for current filters.")
        return

    xs = sorted(grouped.keys())
    means = [np.mean(grouped[x]) for x in xs]
    cis = [mean_ci(grouped[x])[1] for x in xs]

    plt.figure(figsize=(7, 4))
    plt.errorbar(xs, means, yerr=cis, fmt="o-", capsize=4)
    plt.xlabel("eps")
    plt.ylabel("average score")
    title = (
        f"sig_cGA: avg score vs eps (95% CI)\n"
        f"fit={FITNESS_FILTER}, T={T_FILTER}, n={N_FILTER}, M={M_FILTER}"
    )
    plt.title(title)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    out_name = (
        f"plot_sig_cga_fit-{FITNESS_FILTER}_T-{T_FILTER}_n-{N_FILTER}_M-{M_FILTER}.png"
    )
    out_path = PLOTS_DIR / out_name
    plt.savefig(out_path, dpi=200)
    print(f"Saved plot: {out_path}")


def compare_algorithms_score():
    PLOTS_DIR.mkdir(parents=True, exist_ok=True)
    values, selected = _collect_algorithm_values(metric="score")
    labels = [k for k in ["sig_cGA", "cGA", "EA(1,1)"] if values[k]]
    if not labels:
        print("No matching files to compare algorithm scores.")
        return

    means = [mean_ci(values[k])[0] for k in labels]
    cis = [mean_ci(values[k])[1] for k in labels]

    x = np.arange(len(labels))
    plt.figure(figsize=(7, 4))
    plt.bar(x, means, yerr=cis, capsize=4)
    plt.xticks(x, labels)
    plt.ylabel("average score")
    plt.title(
        f"Algorithms: average score (95% CI)\n"
        f"fit={FITNESS_FILTER}, T={T_FILTER}, n={N_FILTER}, M={M_FILTER}, "
        f"sig_eps={selected['sig_cGA']}, cga_K={selected['cGA']}"
    )
    plt.grid(True, axis="y", alpha=0.3)
    plt.tight_layout()
    out_name = (
        f"plot_algos_score_fit-{FITNESS_FILTER}_T-{T_FILTER}_n-{N_FILTER}_M-{M_FILTER}.png"
    )
    out_path = PLOTS_DIR / out_name
    plt.savefig(out_path, dpi=200)
    print(f"Saved plot: {out_path}")


def compare_algorithms_runtime():
    PLOTS_DIR.mkdir(parents=True, exist_ok=True)
    values, selected = _collect_algorithm_values(metric="runtime")
    labels = [k for k in ["sig_cGA", "cGA", "EA(1,1)"] if values[k]]
    if not labels:
        print("No matching files to compare algorithm runtimes.")
        return

    means = [mean_ci(values[k])[0] for k in labels]
    cis = [mean_ci(values[k])[1] for k in labels]

    x = np.arange(len(labels))
    plt.figure(figsize=(7, 4))
    plt.bar(x, means, yerr=cis, capsize=4)
    plt.xticks(x, labels)
    plt.ylabel("average runtime (iterations)")
    plt.title(
        f"Algorithms: average runtime (95% CI)\n"
        f"fit={FITNESS_FILTER}, T={T_FILTER}, n={N_FILTER}, M={M_FILTER}, "
        f"sig_eps={selected['sig_cGA']}, cga_K={selected['cGA']}"
    )
    plt.grid(True, axis="y", alpha=0.3)
    plt.tight_layout()
    out_name = (
        f"plot_algos_runtime_fit-{FITNESS_FILTER}_T-{T_FILTER}_n-{N_FILTER}_M-{M_FILTER}.png"
    )
    out_path = PLOTS_DIR / out_name
    plt.savefig(out_path, dpi=200)
    print(f"Saved plot: {out_path}")


if __name__ == "__main__":
    compare_epsilons()
    compare_algorithms_runtime()
    compare_algorithms_score()
