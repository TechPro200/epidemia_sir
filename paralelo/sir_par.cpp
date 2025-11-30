// sir_par.cpp
// Simulación SIR 2D Paralela con OpenMP — versión corregida
// Compatible con g++ en Windows/MSYS2
// Sin std::thread, sin continue en regiones OpenMP

#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <string>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <cmath>
#include <omp.h>

using namespace std;
namespace fs = std::filesystem;

enum Estado { S, I, R, M };
inline int idx(int i,int j,int N){ return i*N + j; }

void guardarPPM(const vector<int>& grid, int N, int dia, const string& carpeta) {
    fs::create_directories(carpeta);
    ostringstream nombre;
    nombre << carpeta << "/day_" << setw(3) << setfill('0') << dia << ".ppm";
    ofstream out(nombre.str());
    out << "P3\n" << N << " " << N << "\n255\n";
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int estado = grid[i*N + j];
            int r=0,g=0,b=0;
            if (estado == S) { r=0; g=200; b=0; }
            else if (estado == I) { r=200; g=0; b=0; }
            else if (estado == R) { r=0; g=0; b=200; }
            else if (estado == M) { r=50; g=50; b=50; }
            out << r << " " << g << " " << b << " ";
        }
        out << "\n";
    }
    out.close();
}

int main(int argc, char** argv) {
    int N = (argc>1) ? atoi(argv[1]) : 100;
    int dias = (argc>2) ? atoi(argv[2]) : 50;
    int init_infectados = (argc>3) ? atoi(argv[3]) : 1;

    double p_contagio   = 0.25;
    double p_recuperado = 0.05;
    double p_muerte     = 0.01;

    vector<int> grid(N*N, S);
    vector<int> nuevo(N*N, S);

    // Semilla inicial: infectados cerca del centro
    int placed = 0;
    int cx = N/2, cy = N/2;
    for (int di=-5; di<=5 && placed < init_infectados; ++di) {
        for (int dj=-5; dj<=5 && placed < init_infectados; ++dj) {
            int x = cx + di, y = cy + dj;
            if (x>=0 && x<N && y>=0 && y<N) {
                grid[idx(x,y,N)] = I;
                ++placed;
            }
        }
    }

    fs::create_directories("output_par");
    vector<int> new_infections_per_day(dias,0);

    for (int d = 0; d < dias; ++d) {

        nuevo = grid;

        int global_new = 0;
        int global_infected = 0;

        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            int nth = omp_get_num_threads();

            // División de filas entre hilos
            int rows_per = N / nth;
            int rem = N % nth;

            int start_row = tid * rows_per + min(tid, rem);
            int my_rows = rows_per + (tid < rem ? 1 : 0);
            int end_row = start_row + my_rows - 1;

            // Si el hilo no tiene filas, no puede usar "continue". Usamos un if.
            if (my_rows > 0) 
            {
                // Buffer con ghost rows
                int buf_rows = my_rows + 2;
                vector<int> buf(buf_rows * N);

                for (int r = 0; r < buf_rows; ++r) {
                    int gr = start_row + (r - 1);
                    if (gr < 0) gr = 0;
                    if (gr >= N) gr = N - 1;
                    for (int c = 0; c < N; ++c)
                        buf[r*N + c] = grid[gr*N + c];
                }

                // RNG por hilo (sin std::thread)
                std::mt19937 gen(tid * 1234567 + d * 99991 + 42);
                uniform_real_distribution<> unif(0.0,1.0);

                int local_new = 0;
                int local_inf = 0;

                for (int r = 1; r <= my_rows; ++r) {
                    int i = start_row + (r - 1);

                    for (int j=0; j<N; j++) {

                        int s = buf[r*N + j];

                        if (s == S) {
                            int inf = 0;
                            if (j>0 && buf[r*N + j-1] == I) inf++;
                            if (j+1<N && buf[r*N + j+1] == I) inf++;
                            if (buf[(r-1)*N + j] == I) inf++;
                            if (buf[(r+1)*N + j] == I) inf++;

                            if (inf > 0) {
                                double P = 1.0 - pow(1.0 - p_contagio, inf);
                                if (unif(gen) < P) {
                                    nuevo[i*N + j] = I;
                                    local_new++;
                                }
                            }
                        }
                        else if (s == I) {
                            double rnum = unif(gen);
                            if (rnum < p_muerte) {
                                nuevo[i*N + j] = M;
                            }
                            else if (rnum < p_muerte + p_recuperado) {
                                nuevo[i*N + j] = R;
                            }
                        }
                    }
                }

                // Contar infectados
                for (int r = 1; r <= my_rows; ++r) {
                    int i = start_row + (r - 1);
                    for (int j=0; j<N; j++)
                        if (nuevo[i*N + j] == I) local_inf++;
                }

                #pragma omp atomic
                global_new += local_new;

                #pragma omp atomic
                global_infected += local_inf;
            }

            #pragma omp barrier
        }

        grid.swap(nuevo);

        new_infections_per_day[d] = global_new;

        guardarPPM(grid, N, d, "output_par");

        cout << "[PARALELO] Dia " << d << " Infectados = " << global_infected
             << " Nuevos=" << global_new << endl;
    }

    return 0;
}
