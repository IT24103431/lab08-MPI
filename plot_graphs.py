#!/usr/bin/env python3
"""
plot_graphs.py  -  Exercise 4 (graphs)

Reads results/sum_results.csv and results/pi_results.csv (each a
processors,time_seconds table produced by benchmark.sh) and renders:

  results/time_vs_processors.svg  - wall-clock time vs process count, Sum & Pi
  results/speedup.svg             - speedup (T1 / Tp) vs process count, Sum &
                                     Pi, with the ideal linear-speedup line
                                     drawn for reference

Pure standard library (csv + plain SVG) - no matplotlib/numpy required.

Usage: python3 plot_graphs.py
"""
import csv
import os

RESULTS_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "results")

SERIES_COLOR = {"Sum (Exercise 2)": "#2E86AB", "Pi (Exercise 3)": "#C1440E"}
IDEAL_COLOR = "#999999"


def read_csv(path):
    points = []
    with open(path, newline="") as f:
        for row in csv.DictReader(f):
            points.append((int(row["processors"]), float(row["time_seconds"])))
    points.sort()
    return points


def draw_chart(series_dict, xlabel, ylabel, title, out_path, ideal=None, width=700, height=460):
    margin_l, margin_r, margin_t, margin_b = 70, 30, 50, 60
    plot_w = width - margin_l - margin_r
    plot_h = height - margin_t - margin_b

    all_x = sorted({x for pts in series_dict.values() for x, _ in pts})
    all_y = [y for pts in series_dict.values() for _, y in pts]
    if ideal:
        all_y += [y for _, y in ideal]
    y_max = max(all_y) * 1.15 if all_y else 1
    y_min = 0

    def sx(x):
        idx = all_x.index(x)
        if len(all_x) == 1:
            return margin_l + plot_w / 2
        return margin_l + idx * (plot_w / (len(all_x) - 1))

    def sy(y):
        return margin_t + plot_h - (y - y_min) / (y_max - y_min) * plot_h

    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" '
        f'viewBox="0 0 {width} {height}" font-family="Arial, Helvetica, sans-serif">\n'
        f'<rect x="0" y="0" width="{width}" height="{height}" fill="white"/>\n'
    ]
    parts.append(
        f'<text x="{width/2}" y="24" text-anchor="middle" font-size="17" '
        f'font-weight="bold" fill="#1a1a1a">{title}</text>'
    )

    n_ticks = 5
    for i in range(n_ticks + 1):
        yv = y_min + (y_max - y_min) * i / n_ticks
        yy = sy(yv)
        parts.append(
            f'<line x1="{margin_l}" y1="{yy:.1f}" x2="{margin_l+plot_w}" y2="{yy:.1f}" '
            f'stroke="#e0e0e0" stroke-width="1"/>'
        )
        parts.append(
            f'<text x="{margin_l-10}" y="{yy+4:.1f}" text-anchor="end" '
            f'font-size="11" fill="#555">{yv:.3g}</text>'
        )

    for x in all_x:
        xx = sx(x)
        parts.append(
            f'<line x1="{xx:.1f}" y1="{margin_t}" x2="{xx:.1f}" y2="{margin_t+plot_h}" '
            f'stroke="#f0f0f0" stroke-width="1"/>'
        )
        parts.append(
            f'<text x="{xx:.1f}" y="{margin_t+plot_h+20}" text-anchor="middle" '
            f'font-size="11" fill="#555">{x}</text>'
        )

    parts.append(
        f'<line x1="{margin_l}" y1="{margin_t}" x2="{margin_l}" y2="{margin_t+plot_h}" '
        f'stroke="#333" stroke-width="1.5"/>'
    )
    parts.append(
        f'<line x1="{margin_l}" y1="{margin_t+plot_h}" x2="{margin_l+plot_w}" y2="{margin_t+plot_h}" '
        f'stroke="#333" stroke-width="1.5"/>'
    )

    parts.append(
        f'<text x="{margin_l+plot_w/2}" y="{height-15}" text-anchor="middle" '
        f'font-size="13" fill="#333">{xlabel}</text>'
    )
    parts.append(
        f'<text x="18" y="{margin_t+plot_h/2}" text-anchor="middle" font-size="13" fill="#333" '
        f'transform="rotate(-90 18 {margin_t+plot_h/2})">{ylabel}</text>'
    )

    if ideal:
        pts_str = " ".join(f"{sx(x):.1f},{sy(y):.1f}" for x, y in ideal)
        parts.append(
            f'<polyline points="{pts_str}" fill="none" stroke="{IDEAL_COLOR}" '
            f'stroke-width="1.5" stroke-dasharray="5,4"/>'
        )

    legend_y = margin_t
    for name, pts in series_dict.items():
        color = SERIES_COLOR.get(name, "#333333")
        pts_str = " ".join(f"{sx(x):.1f},{sy(y):.1f}" for x, y in pts)
        parts.append(f'<polyline points="{pts_str}" fill="none" stroke="{color}" stroke-width="2.5"/>')
        for x, y in pts:
            parts.append(f'<circle cx="{sx(x):.1f}" cy="{sy(y):.1f}" r="4" fill="{color}"/>')
        parts.append(f'<line x1="{width-190}" y1="{legend_y}" x2="{width-165}" y2="{legend_y}" stroke="{color}" stroke-width="3"/>')
        parts.append(f'<text x="{width-160}" y="{legend_y+4}" font-size="12" fill="#333">{name}</text>')
        legend_y += 18

    if ideal:
        parts.append(
            f'<line x1="{width-190}" y1="{legend_y}" x2="{width-165}" y2="{legend_y}" '
            f'stroke="{IDEAL_COLOR}" stroke-width="1.5" stroke-dasharray="5,4"/>'
        )
        parts.append(f'<text x="{width-160}" y="{legend_y+4}" font-size="12" fill="#333">Ideal linear speedup</text>')

    parts.append("</svg>\n")
    with open(out_path, "w") as f:
        f.write("".join(parts))
    print(f"wrote {out_path}")


def main():
    sum_pts = read_csv(os.path.join(RESULTS_DIR, "sum_results.csv"))
    pi_pts = read_csv(os.path.join(RESULTS_DIR, "pi_results.csv"))

    draw_chart(
        {"Sum (Exercise 2)": sum_pts, "Pi (Exercise 3)": pi_pts},
        xlabel="Number of processors",
        ylabel="Wall-clock time (seconds)",
        title="Time vs Number of Processors",
        out_path=os.path.join(RESULTS_DIR, "time_vs_processors.svg"),
    )

    def speedup(pts):
        t1 = dict(pts)[1]
        return [(p, t1 / t) for p, t in pts]

    sum_speedup = speedup(sum_pts)
    pi_speedup = speedup(pi_pts)
    all_x = sorted({x for x, _ in sum_speedup} | {x for x, _ in pi_speedup})
    ideal = [(x, x) for x in all_x]

    draw_chart(
        {"Sum (Exercise 2)": sum_speedup, "Pi (Exercise 3)": pi_speedup},
        xlabel="Number of processors",
        ylabel="Speedup (T1 / Tp)",
        title="Speedup vs Number of Processors",
        out_path=os.path.join(RESULTS_DIR, "speedup.svg"),
        ideal=ideal,
    )


if __name__ == "__main__":
    main()
