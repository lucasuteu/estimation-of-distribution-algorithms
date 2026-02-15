import re
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np


DATA_DIR = Path("data")
PATTERN = re.compile(r"sig_cga_.*_eps-([-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)\.csv$")


def read_scores(csv_path: Path):
    scores = []
    with csv_path.open("r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split(",")
            scores.append(float(parts[0]))
    return scores


def main():
    grouped = {}

    for p in DATA_DIR.glob("sig_cga*.csv"):
        m = PATTERN.search(p.name)
        if not m:
            continue
        eps = float(m.group(1))
        grouped.setdefault(eps, []).extend(read_scores(p))

    if not grouped:
        print("No sig_cga csv files with eps found in data/.")
        return

    xs = sorted(grouped.keys())
    means = [np.mean(grouped[x]) for x in xs]
    cis = []
    for x in xs:
        vals = np.array(grouped[x], dtype=float)
        if len(vals) <= 1:
            cis.append(0.0)
            continue
        std = np.std(vals, ddof=1)
        se = std / np.sqrt(len(vals))
        cis.append(1.96 * se)  # 95% CI (normal approximation)

    plt.figure(figsize=(7, 4))
    plt.errorbar(xs, means, yerr=cis, fmt="o-", capsize=4)
    plt.xlabel("eps")
    plt.ylabel("average score")
    plt.title("sig_cGA on data/: average score with 95% CI")
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()
