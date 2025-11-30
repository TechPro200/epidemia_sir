#!/bin/bash
mkdir -p results
echo "threads,run,time_s" > results/times.csv
for t in 1 2 4 8; do
  export OMP_NUM_THREADS=$t
  for run in 1 1 1; do
    echo "Running threads=$t run=$run"
    /usr/bin/time -f "%e" -o tmp_time.txt ./sir_par 100 100 100
    time_s=$(cat tmp_time.txt)
    echo "$t,$run,$time_s" >> results/times.csv
  done
done
python3 scripts/compute_speedup.py results/times.csv results/speedup.csv