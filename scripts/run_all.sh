#!/bin/bash
set -e

# -----------------------------
# Ejecutables y carpetas reales
# -----------------------------
EXE_SEQ="/c/Users/tutan/Downloads/epidemia_sir/secuencial/sir_seq.exe"
EXE_PAR="/c/Users/tutan/Downloads/epidemia_sir/paralelo/sir_par.exe"

OUTPUT_SEQ="/c/Users/tutan/Downloads/epidemia_sir/secuencial/output_seq"
OUTPUT_PAR="/c/Users/tutan/Downloads/epidemia_sir/paralelo/output_par"
OUTPUT_MERGE="/c/Users/tutan/Downloads/epidemia_sir/output_merge"
RESULTS="/c/Users/tutan/Downloads/epidemia_sir/results"
VIDEO_DIR="/c/Users/tutan/Downloads/epidemia_sir/video"

mkdir -p "$OUTPUT_SEQ" "$OUTPUT_PAR" "$OUTPUT_MERGE" "$RESULTS" "$VIDEO_DIR"

# -----------------------------
# 1️⃣ Ejecutar simulación secuencial
# -----------------------------
echo "Ejecutando simulación secuencial..."
"$EXE_SEQ" 1000 365 1
echo "Simulación secuencial terminada."

# -----------------------------
# 2️⃣ Ejecutar simulación paralela con distintos threads
# -----------------------------
echo "Ejecutando simulación paralela..."
echo "threads,run,time" > "$RESULTS/times.csv"   # columna 'time'

for t in 1 2 4 8; do
    export OMP_NUM_THREADS=$t
    for run in 1 2 3; do
        echo "Threads=$t, Run=$run"
        /usr/bin/time -f "%e" -o tmp_time.txt "$EXE_PAR" 1000 365 1
        time_s=$(cat tmp_time.txt)
        echo "$t,$run,$time_s" >> "$RESULTS/times.csv"
    done
done

# -----------------------------
# 3️⃣ Calcular speed-up
# -----------------------------
echo "Calculando speed-up..."
python3 /c/Users/tutan/Downloads/epidemia_sir/scripts/compute_speedup.py \
"$RESULTS/times.csv" "$RESULTS/speedup.csv"

# -----------------------------
# 4️⃣ Graficar speed-up
# -----------------------------
echo "Graficando speed-up..."
python3 /c/Users/tutan/Downloads/epidemia_sir/scripts/plot_speedup.py

# -----------------------------
# 5️⃣ Combinar imágenes lado a lado
# -----------------------------
echo "Combinando imágenes secuencial vs paralelo..."
python3 /c/Users/tutan/Downloads/epidemia_sir/scripts/merge_side_by_side.py

# -----------------------------
# 6️⃣ Generar video MP4
# -----------------------------
echo "Generando video..."
ffmpeg -y -framerate 6 -i "$OUTPUT_MERGE/day_%03d.ppm" -c:v libx264 -pix_fmt yuv420p "$VIDEO_DIR/brote.mp4"

echo "=== Todo terminado ==="
echo "Resultados en $RESULTS, video en $VIDEO_DIR/brote.mp4"
