## Requisitos

- Compilador C++ con soporte C++17.  
- Para la versión paralela: compilador con OpenMP.  
- Python 3 o superior (para ejecutar scripts de post-procesamiento).  
- (Opcional) `ffmpeg` si planeas generar video con resultados.

## Compilar

### Versión secuencial (ejecutable estático)

powershell
cd C:\Users\tutan\Downloads\epidemia_sir\secuencial
g++ -std=c++17 -O3 -static -static-libgcc -static-libstdc++ sir_seq.cpp -o sir_seq.exe
cd C:\Users\tutan\Downloads\epidemia_sir\paralelo
g++ -std=c++17 -O3 -fopenmp -static -static-libgcc -static-libstdc++ sir_par.cpp -o sir_par.exe

Ejecutar simulación
# Secuencial
cd C:\Users\tutan\Downloads\epidemia_sir\secuencial
.\sir_seq.exe [parametros]

# Paralela
cd C:\Users\tutan\Downloads\epidemia_sir\paralelo
$env:OMP_NUM_THREADS=4  # Ajusta según cores
.\sir_par.exe [parametros]

Librerias Python
pip install numpy matplotlib opencv-python

Comando Scipts-Pruebas
cd C:\Users\tutan\Downloads\epidemia_sir\scripts
python Pruebas_Todo.py
