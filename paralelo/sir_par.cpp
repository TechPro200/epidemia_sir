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
    if (!out.is_open()) {
        cerr << "ERROR: no se pudo abrir " << nombre.str() << " para escribir\n";
        return;
    }
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
    int N = (argc>1) ? atoi(argv[1]) : 200;
    int dias = (argc>2) ? atoi(argv[2]) : 50;
    int init_infectados = (argc>3) ? atoi(argv[3]) : 1;

    double p_contagio   = 0.25;
    double p_recuperado = 0.05;
    double p_muerte     = 0.01;
    double P[5];
    for (int k=0;k<=4;++k) P[k] = 1.0 - pow(1.0 - p_contagio, k);

    vector<int> grid(N*N, S);
    vector<int> nuevo(N*N, S);


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

    const int io_stride = 7; 

    for (int d = 0; d < dias; ++d) {

        int global_new = 0;
        int global_infected = 0;

        
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            mt19937 gen((unsigned int)(tid * 1315423911u + (unsigned int)d * 10007u + 12345u));
            uniform_real_distribution<> unif(0.0, 1.0);

            #pragma omp for schedule(static) reduction(+:global_new, global_infected)
            for (int i = 0; i < N; ++i) {
                for (int j=0; j < N; ++j) {

                    int s = grid[i*N + j];
                    int out = s;

                    if (s == S) {
                        int inf = 0;
                        if (j > 0 && grid[i*N + (j-1)] == I) inf++;
                        if (j+1 < N && grid[i*N + (j+1)] == I) inf++;
                        if (i > 0 && grid[(i-1)*N + j] == I) inf++;
                        if (i+1 < N && grid[(i+1)*N + j] == I) inf++;

                        if (inf > 0) {
                            double prob = P[inf];
                            if (unif(gen) < prob) {
                                out = I;
                                global_new += 1;
                            }
                        }
                    }
                    else if (s == I) {
                        double rnum = unif(gen);
                        if (rnum < p_muerte) {
                            out = M;
                        }
                        else if (rnum < p_muerte + p_recuperado) {
                            out = R;
                        }
                    }

                    nuevo[i*N + j] = out;
                    if (out == I) global_infected += 1;
                }
            } 
        } 

       
        grid.swap(nuevo);
        new_infections_per_day[d] = global_new;
        if (d % io_stride == 0 || d == dias - 1) {
            guardarPPM(grid, N, d, "output_par");
        }

        cout << "[PARALELO] Dia " << d << " Infectados = " << global_infected
             << " Nuevos=" << global_new << endl;
    }

    ofstream sf("output_par/stats.csv");
    if (sf.is_open()) {
        sf << "day,new_infections\n";
        for (int d=0; d<dias; ++d) sf << d << "," << new_infections_per_day[d] << "\n";
        sf.close();
    }

    return 0;
}
