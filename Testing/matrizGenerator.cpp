#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <chrono>

int range_low = 0;
int range_high = 24;

// GENERADOR NUMEROS ALEATORIOS
//  1. Obtener entropía
std::random_device rd;
// 2. Motor de números aleatorios
auto seed = rd() ^ static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
std::mt19937 gen(seed);
// 3. Definir la distribución (rango)
std::uniform_int_distribution<> distribucion(range_low, range_high);
std::uniform_real_distribution<double> distr_real(0.0, 1.0);

// crea matrices de numeros aleatorios de range_low a range_high de tamaño preestablecido, en el enunciado se asumen dimensiones potencia de 2
std::vector<std::vector<int>> matrizGenerador(int x, int y){
    std::vector<std::vector<int>> matriz(x, std::vector<int>(y));
    std::cout << x << "\n";
    for (int i = 0; i < x; i++)
    {
        for (int k = 0; k < y; k++)
        {
            matriz[i][k] = distribucion(gen);
            // std::cout << matriz[i][k];
            // std::cout << " ";
        }
        // std::cout << "\n";
    }
    return matriz;
};

//generador de matrices de numeros reales entre 0 y 1
std::vector<std::vector<double>> matrizRealGenerador(int x, int y){
    std::vector<std::vector<double>> matriz(x, std::vector<double>(y));
    for (int i = 0; i < x; i++){
        for (int j = 0; j < y; j++){
            matriz[i][j] = distr_real(gen);
        }
    }
    return matriz;
}

//generador de matrices diagonales de numeros aleatorios entre range_low y range_high
std::vector<std::vector<int>> matrizDiagonalGenerador(int x, int y){
    std::vector<std::vector<int>> matriz(x, std::vector<int>(y, 0));
    for (int i = 0; i < x; i++){
        matriz[i][i] = distribucion(gen);
    }
    return matriz;
}

//generador de matrices triangulares inferiores de numeros aleatorios entre range_low y range_high
std::vector<std::vector<int>> matrizTriangularInferiorGenerador(int x, int y){
    std::vector<std::vector<int>> matriz(x, std::vector<int>(y, 0));
    for (int i = 0; i < x; i++){
        for (int j = 0; j <= i; j++){
            matriz[i][j] = distribucion(gen);
        }
    }
    return matriz;
}

template <typename T>
void saveMatriz(std::vector<std::vector<T>> matriz, std::string nombre_output){
    std::ofstream outFile(nombre_output);
    if (outFile.is_open()) {
        outFile << matriz.size() << "\n";
        for (const auto& row : matriz) {
            for (size_t i = 0; i < row.size(); ++i) {
                outFile << row[i];
                if (i < row.size() - 1) outFile << " ";
            }
            outFile << "\n";
        }
        outFile.close();
    }
}

/*int main(){
    saveMatriz(matrizGenerador(1024, 1024), "matriz1");
    saveMatriz(matrizGenerador(1024, 1024), "matriz2");
    return 0;
}*/