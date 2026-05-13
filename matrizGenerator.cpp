#include <iostream>
#include <vector>
#include <random>

int x = 10;
int y = 10; 
int range_low = 0;
int range_high = 24;


//GENERADOR NUMEROS ALEATORIOS
// 1. Obtener entropía
std::random_device rd; 
// 2. Motor de números aleatorios (Mersenne Twister)
std::mt19937 gen(rd()); 
// 3. Definir la distribución (rango)
std::uniform_int_distribution<> distribucion(range_low, range_high); 


//crea matrices de numeros aleatorios de range_low a range_high de tamaño preestablecido, en el enunciado se asumen dimensiones potencia de 2
std::vector<std::vector<int>> matrizGenerador(){
    std::vector<std::vector<int>> matriz(x, std::vector<int>(y));
    for(int i = 0; i < x; i++){
        for(int k = 0; k < y; k++){
            matriz[i][k] = distribucion(gen);
            std::cout << matriz[i][k];
            std::cout << " ";
        }
        std::cout << "\n";
    }
    return matriz;
};

int main(){
    matrizGenerador();
    return 0;
}