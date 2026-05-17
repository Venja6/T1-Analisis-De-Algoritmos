#include <iostream>
#include <vector>
#include <random>
<<<<<<< HEAD
#include <chrono>

int x = 16;
int y = x; 
=======
#include <fstream>
#include <string>

>>>>>>> b1dcac7e49c90fa4b51ce44bc2a6ab8229a79f7d
int range_low = 0;
int range_high = 24;


//GENERADOR NUMEROS ALEATORIOS
// 1. Obtener entropía
std::random_device rd; 
// 2. Motor de números aleatorios 
auto seed = rd() ^ static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
std::mt19937 gen(seed);
// 3. Definir la distribución (rango)
std::uniform_int_distribution<> distribucion(range_low, range_high); 
 

<<<<<<< HEAD
//crea matrices de numeros aleatorios de range_low a range_high de tamaño preestablecido, para que el algoritmo funcione se usan potencias de 2
std::vector<std::vector<int>> matrizGenerador(){
=======
//crea matrices de numeros aleatorios de range_low a range_high de tamaño preestablecido, en el enunciado se asumen dimensiones potencia de 2
std::vector<std::vector<int>> matrizGenerador(int x, int y){
>>>>>>> b1dcac7e49c90fa4b51ce44bc2a6ab8229a79f7d
    std::vector<std::vector<int>> matriz(x, std::vector<int>(y));
    std::cout << x << "\n";
    for(int i = 0; i < x; i++){
        for(int k = 0; k < y; k++){
            matriz[i][k] = distribucion(gen);
            //std::cout << matriz[i][k];
            //std::cout << " ";
        }
        //std::cout << "\n";
    }
    return matriz;
};

void saveMatriz(std::vector<std::vector<int>> matriz, std::string nombre_output){
    std::ofstream outFile(nombre_output);
    if (outFile.is_open()) {
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

int main(){
    saveMatriz(matrizGenerador(1024, 1024), "matriz1");
    saveMatriz(matrizGenerador(1024, 1024), "matriz2");
    return 0;
}