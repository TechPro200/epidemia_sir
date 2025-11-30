# SIR 2D Simulation (Sequential & Parallel) - Minimal Deliverable

## Contents
- `sir_seq.cpp` : sequential version
- `sir_par.cpp` : parallel version (OpenMP)
- `scripts/merge_side_by_side.py` : merge images into side-by-side frames
- `scripts/make_video.sh` : create MP4 with ffmpeg
- `scripts/run_experiments.sh` : run parallel with multiple threads and collect times
- `scripts/compute_speedup.py` : compute average times and speed-up
- `scripts/plot_speedup.py` : plot speed-up

## Build
Sequential:
```
g++ -O3 sir_seq.cpp -o sir_seq
```
Parallel:
```
g++ -O3 -fopenmp sir_par.cpp -o sir_par
```

## Run (quick)
Sequential:
```
./sir_seq 100 100 100
```
Parallel:
```
export OMP_NUM_THREADS=4
./sir_par 100 100 100
```

## Visualize
```
python3 scripts/merge_side_by_side.py
bash scripts/make_video.sh
```

## Experiments (timing)
```
bash scripts/run_experiments.sh
python3 scripts/plot_speedup.py
```