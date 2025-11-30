import os
import subprocess
import time
import csv
import cv2
import matplotlib.pyplot as plt
import numpy as np

# CAMBIAR SI ES NECESARIO
BASE_DIR = r"C:\Users\tutan\Downloads\epidemia_sir"
SEQ_DIR = os.path.join(BASE_DIR, "scripts", "output_seq")
PAR_DIR = os.path.join(BASE_DIR, "scripts", "output_par")
OUT_DIR = os.path.join(BASE_DIR, "output_merge")
VIDEO_FILE = os.path.join(BASE_DIR, "video", "brote.mp4")
CSV_FILE = os.path.join(BASE_DIR, "resultados", "timing.csv")

os.makedirs(SEQ_DIR, exist_ok=True)
os.makedirs(PAR_DIR, exist_ok=True)
os.makedirs(OUT_DIR, exist_ok=True)
os.makedirs(os.path.dirname(VIDEO_FILE), exist_ok=True)
os.makedirs(os.path.dirname(CSV_FILE), exist_ok=True)



# PARAMETROS
N = 1000
dias = 365
infectados = 10



def ejecutar_simulacion(exe_path, N, dias, infectados, cores=None):
    """Ejecuta el .exe y mide tiempo real."""
    print(f"\nEjecutando: {exe_path}")
    print(f" → N={N}, dias={dias}, infectados={infectados}, cores={cores}")

    start = time.time()

    env = os.environ.copy()
    if cores is not None:
        env["OMP_NUM_THREADS"] = str(cores)

    subprocess.run([exe_path, str(N), str(dias), str(infectados)], check=True, env=env)

    return time.time() - start



seq_exe = os.path.join(BASE_DIR, "secuencial", "sir_seq.exe")
print("\nSIMULACION SECUENCIAL")
t_seq = ejecutar_simulacion(seq_exe, N, dias, infectados)
print(f"Tiempo secuencial: {t_seq:.2f}s")



# EJECUCION PARALELA
par_exe = os.path.join(BASE_DIR, "paralelo", "sir_par.exe")
cores_list = [1, 2, 4, 8]
tiempos_paralelo = []

print("\n=== SIMULACION PARALELA ===")
for c in cores_list:
    t = ejecutar_simulacion(par_exe, N, dias, infectados, cores=c)
    print(f"Tiempo paralelo {c} cores: {t:.2f}s")
    tiempos_paralelo.append(t)


# GUARDAR CSV
with open(CSV_FILE, "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["cores", "tiempo_segundos"])
    w.writerow([1, t_seq])
    for i, c in enumerate(cores_list):
        w.writerow([c, tiempos_paralelo[i]])

print(f"\nCSV guardado en: {CSV_FILE}")


print("\nFusionando imagenes")

seq_files = sorted([f for f in os.listdir(SEQ_DIR) if f.endswith(".ppm")])

merged_images = []

for f in seq_files:
    img_s = cv2.imread(os.path.join(SEQ_DIR, f))
    img_p = cv2.imread(os.path.join(PAR_DIR, f))

    if img_p is None:
        img_p = img_s.copy()

    merged = np.hstack((img_s, img_p))
    out_path = os.path.join(OUT_DIR, f)

    cv2.imwrite(out_path, merged)
    merged_images.append(out_path)

print(f"{len(merged_images)} imagenes fusionadas.")
print("Generando video")

first = cv2.imread(merged_images[0])
h, w, _ = first.shape

video = cv2.VideoWriter(VIDEO_FILE, cv2.VideoWriter_fourcc(*"mp4v"), 6, (w, h))

for img_path in merged_images:
    frame = cv2.imread(img_path)
    video.write(frame)

video.release()

print(f"Video generado: {VIDEO_FILE}")
speedup = [t_seq / t for t in tiempos_paralelo]

plt.figure(figsize=(6,5))
plt.plot(cores_list, speedup, marker="o")
plt.xlabel("Nucleos")
plt.ylabel("Speed-Up")
plt.title("Speed-Up Real de la Simulacion Paralela")
plt.grid(True)

speedup_path = os.path.join(BASE_DIR, "resultados", "speedup.png")
plt.savefig(speedup_path, dpi=200)
plt.close()

print(f"Speed-Up guardado en: {speedup_path}")
