/** uhr: generic time performance tester
 * Author: LELE
 *
 * Things to set up:
 * 0. Includes: include all files to be tested,
 * 1. Time unit: in elapsed_time,
 * 2. What to write on time_data,
 * 3. Data type and distribution of RNG,
 * 4. Additive or multiplicative stepping,
 * 5. The experiments: in outer for loop. */

#include <cstdint>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "utils.cpp"

#include "algoritmo.cpp"
#include "matrizGenerator.cpp"

int main(int argc, char *argv[])
{
    // Validate and sanitize input
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <archivo_salida.csv> <runs>" << std::endl;
        return 1;
    }

    // Leer argumentos básicos
    std::string archivo_salida = argv[1];
    std::int64_t runs = std::stoll(argv[2]);

    // Set up clock variables
    std::int64_t i, executed_runs;
    std::vector<double> times(runs);
    std::vector<double> q;
    double mean_time, time_stdev, dev;
    auto begin_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> elapsed_time = end_time - begin_time;

    // Set up random number generation
    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_int_distribution<std::int64_t> u_distr; // change depending on app

    // File to write time data
    std::ofstream time_data;
    time_data.open(argv[1]);
    time_data << "n,t_mean,t_stdev,t_Q0,t_Q1,t_Q2,t_Q3,t_Q4" << std::endl;

    // Begin testing
    std::cerr << "\033[0;36mRunning tests...\033[0m" << std::endl
              << std::endl;
    executed_runs = 0;

    mean_time = 0;
    time_stdev = 0;

    // Test configuration goes here
    int n = 1024;
    // Generar A
    saveMatriz(matrizGenerador(n, n), "A.txt");
    // Generar B
    saveMatriz(matrizGenerador(n, n), "B.txt");
    vector<int> dataA, dataB, dataC;
    leer_matriz("A.txt", dataA, n);
    leer_matriz("B.txt", dataB, n);

    dataC.assign(n * n, 0);
    vector<int> dataC_std(n * n, 0);
    Matrix A{dataA, n, 0, n}, B{dataB, n, 0, n}, C{dataC, n, 0, n};
    Matrix C_std{dataC_std, n, 0, n};
    std::vector<int> global_memory_pool(3 * n * n, 0);
    // Run to compute elapsed time
    for (i = 0; i < runs; i++)
    {
        // Remember to change total depending on step type
        display_progress(++executed_runs, runs);
        
        dataC.assign(n * n, 0);
        begin_time = std::chrono::high_resolution_clock::now();
        Stranssen(A, B, C, global_memory_pool, 0);
        //multiplicar_standard(A, B, C);
        end_time = std::chrono::high_resolution_clock::now();

        elapsed_time = (end_time - begin_time) / 1e9;
        times[i] = elapsed_time.count();

        mean_time += times[i];
    }

    // Compute statistics
    mean_time /= runs;

    for (i = 0; i < runs; i++)
    {
        dev = times[i] - mean_time;
        time_stdev += dev * dev;
    }

    time_stdev /= runs - 1; // Subtract 1 to get unbiased estimator
    time_stdev = std::sqrt(time_stdev);

    quartiles(times, q);

    time_data << n << "," << mean_time << "," << time_stdev << ",";
    time_data << q[0] << "," << q[1] << "," << q[2] << "," << q[3] << "," << q[4] << std::endl;

    // This is to keep loading bar after testing
    std::cerr << std::endl
              << std::endl;
    std::cerr << "\033[1;32mDone!\033[0m" << std::endl;

    time_data.close();

    return 0;
}
