#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <string>
#include <iomanip>
#include <sstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

enum Estado { S, I, R, M };

void guardarPPM(const vector<int>& grid, int N, int dia, const string& carpeta) {
    fs::create_directories(carpeta);
    ostringstream nombre;
    nombre << carpeta << "/day_" << setw(3) << setfill('0') << dia << ".ppm";

    ofstream out(nombre.str());
    if (!out.is_open()) {
        cerr << "ERROR: No se pudo escribir " << nombre.str() << endl;
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

inline int idx(int i, int j, int N) { return i*N + j; }

int main(int argc, char** argv) {

    int N = (argc > 1) ? atoi(argv[1]) : 100;      
    int dias = (argc > 2) ? atoi(argv[2]) : 50;    
    int init_infectados = (argc > 3) ? atoi(argv[3]) : 1; 

    double p_contagio   = 0.25;
    double p_recuperado = 0.05;
    double p_muerte     = 0.01;

    vector<int> grid(N*N, S);
    vector<int> nuevo(N*N, S);

    int placed = 0;
    int cx = N/2, cy = N/2;

    for (int di = -5; di <= 5 && placed < init_infectados; ++di) {
        for (int dj = -5; dj <= 5 && placed < init_infectados; ++dj) {
            int x = cx + di, y = cy + dj;
            if (x >= 0 && x < N && y >= 0 && y < N) {
                grid[idx(x, y, N)] = I;
                placed++;
            }
        }
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> unif(0.0, 1.0);

    vector<int> new_infections_per_day(dias, 0);

    for (int d = 0; d < dias; ++d) {

        nuevo = grid;
        int new_inf_today = 0;

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {

                int s = grid[idx(i, j, N)];

                if (s == S) {
                    int infectados = 0;

                    if (i > 0     && grid[idx(i-1,j,N)] == I) infectados++;
                    if (i+1 < N   && grid[idx(i+1,j,N)] == I) infectados++;
                    if (j > 0     && grid[idx(i,j-1,N)] == I) infectados++;
                    if (j+1 < N   && grid[idx(i,j+1,N)] == I) infectados++;

                    if (infectados > 0) {
                        double P = 1.0 - pow(1.0 - p_contagio, infectados);
                        if (unif(gen) < P) {
                            nuevo[idx(i,j,N)] = I;
                            new_inf_today++;
                        }
                    }
                }
                else if (s == I) {
                    double r = unif(gen);
                    if (r < p_muerte)
                        nuevo[idx(i,j,N)] = M;
                    else if (r < p_muerte + p_recuperado)
                        nuevo[idx(i,j,N)] = R;
                }
            }
        }

        grid.swap(nuevo);
        new_infections_per_day[d] = new_inf_today;

        if (d % 7 == 0 || d == dias - 1)
            guardarPPM(grid, N, d, "output_seq");
        int totalI = 0;
        for (int v : grid) if (v == I) totalI++;

        cout << "[SECUENCIAL] Dia " << d 
             << " Infectados = " << totalI 
             << " Nuevos=" << new_inf_today 
             << endl;
    }

    ofstream sf("output_seq/stats.csv");
    sf << "day,new_infections\n";
    for (int d = 0; d < dias; d++)
        sf << d << "," << new_infections_per_day[d] << "\n";

    sf.close();
    return 0;
}
