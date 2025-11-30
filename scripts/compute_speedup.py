# compute_speedup.py
import sys, csv, statistics
inp = sys.argv[1]
out = sys.argv[2]
rows = {}
with open(inp) as f:
    reader = csv.DictReader(f)
    for r in reader:
        t = int(r['threads'])
        rows.setdefault(t, []).append(float(r['time_s']))
with open(out, 'w') as f:
    f.write('threads,mean_time,speedup\n')
    t1 = statistics.mean(rows[1])
    for t in sorted(rows.keys()):
        mt = statistics.mean(rows[t])
        sp = t1/mt
        f.write(f"{t},{mt},{sp}\n")
print("Wrote", out)