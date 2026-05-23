#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <string>
#include <chrono>
#include <fstream>

using namespace std;
struct Matrix{
    vector<int>& datos;
    int largo_original; 
    int punto_inicio; 
    int n; //tamaño de la matriz (n x n) (Mientras el algoritmo avance, n se reducirá a la mitad en cada llamada recursiva (submatrices))
    
    //Segmento para trabajar como una matriz incluso cuando solo es un vector 1D (traductor de índices)
    //setter y getter
    int& operator()(int row, int col) {
        return datos[punto_inicio + row * largo_original + col];
    }
    const int& operator()(int row, int col) const {
        return datos[punto_inicio + row * largo_original + col];
    }
};

void leer_matriz(const string& filename, vector<int>& datos, int& n) {
    std::ifstream in(filename);
    if (!in) {
        cerr << "No se pudo abrir archivo: " << filename << endl;
        exit(1);
    }

    if (!(in >> n)) {
        cerr << "Formato inválido: no se pudo leer la dimensión desde " << filename << endl;
        exit(1);
    }

    datos.assign(n * n, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (!(in >> datos[i * n + j])) {
                cerr << "Datos insuficientes en " << filename << ", llenando el resto con ceros" << endl;
                return;
            }
        }
    }
}



void operar_matriz(const Matrix& A, const Matrix& B, Matrix& res, bool subtract = false) {
    for (int i = 0; i < A.n; ++i) {
        for (int j = 0; j < A.n; ++j) {
            if (subtract) {
                res(i, j) = A(i, j) - B(i, j);
            } else {
                res(i, j) = A(i, j) + B(i, j);
            }
        }
    }

}

void multiplicar_standard(const Matrix& A, const Matrix& B, Matrix& C) {
    for (int i = 0; i < A.n; i++) {
        for (int k = 0; k < A.n; k++) {
            int tempA = A(i, k);
            for (int j = 0; j < A.n; j++) {
                C(i, j) += tempA * B(k, j);
            }
        }
    }
}

void Stranssen(Matrix& A, Matrix& B, Matrix& C, vector<int>& pool, int pool_index) {
    int n = A.n;
    if (n <= 64) { // Un umbral de 64 suele ser el punto dulce con Memory Pool
        multiplicar_standard(A, B, C);
        return;
    }

    int nuevo_n = n / 2;
    int size = nuevo_n * nuevo_n;

    // Traductores de índices para submatrices (no asignan memoria RAM)
    auto sub_matriz = [&](Matrix& M, int row, int col) {
        return Matrix{M.datos, M.largo_original, M.punto_inicio + (row * nuevo_n * M.largo_original) + (col * nuevo_n), nuevo_n};
    };

    Matrix A11 = sub_matriz(A, 0, 0), A12 = sub_matriz(A, 0, 1), A21 = sub_matriz(A, 1, 0), A22 = sub_matriz(A, 1, 1);
    Matrix B11 = sub_matriz(B, 0, 0), B12 = sub_matriz(B, 0, 1), B21 = sub_matriz(B, 1, 0), B22 = sub_matriz(B, 1, 1);
    Matrix C11 = sub_matriz(C, 0, 0), C12 = sub_matriz(C, 0, 1), C21 = sub_matriz(C, 1, 0), C22 = sub_matriz(C, 1, 1);

    Matrix M1{pool, nuevo_n, pool_index + 0 * size, nuevo_n};
    Matrix M2{pool, nuevo_n, pool_index + 1 * size, nuevo_n};
    Matrix M3{pool, nuevo_n, pool_index + 2 * size, nuevo_n};
    Matrix M4{pool, nuevo_n, pool_index + 3 * size, nuevo_n};
    Matrix M5{pool, nuevo_n, pool_index + 4 * size, nuevo_n};
    Matrix M6{pool, nuevo_n, pool_index + 5 * size, nuevo_n};
    Matrix M7{pool, nuevo_n, pool_index + 6 * size, nuevo_n};
    Matrix TA{pool, nuevo_n, pool_index + 7 * size, nuevo_n};
    Matrix TB{pool, nuevo_n, pool_index + 8 * size, nuevo_n};

    // El siguiente nivel recursivo usará el espacio del pool que queda libre 
    // después de nuestras 9 matrices temporales actuales
    int siguiente_pool_index = pool_index + 9 * size;

    // M1 = (A11 + A22) * (B11 + B22)
    operar_matriz(A11, A22, TA); operar_matriz(B11, B22, TB);
    Stranssen(TA, TB, M1, pool, siguiente_pool_index);

    // M2 = (A21 + A22) * B11
    operar_matriz(A21, A22, TA);
    Stranssen(TA, B11, M2, pool, siguiente_pool_index);

    // M3 = A11 * (B12 - B22)
    operar_matriz(B12, B22, TB, true);
    Stranssen(A11, TB, M3, pool, siguiente_pool_index);

    // M4 = A22 * (B21 - B11)
    operar_matriz(B21, B11, TB, true);
    Stranssen(A22, TB, M4, pool, siguiente_pool_index);

    // M5 = (A11 + A12) * B22
    operar_matriz(A11, A12, TA);
    Stranssen(TA, B22, M5, pool, siguiente_pool_index);

    // M6 = (A21 - A11) * (B11 + B12)
    operar_matriz(A21, A11, TA, true); operar_matriz(B11, B12, TB);
    Stranssen(TA, TB, M6, pool, siguiente_pool_index);

    // M7 = (A12 - A22) * (B21 + B22)
    operar_matriz(A12, A22, TA, true); operar_matriz(B21, B22, TB);
    Stranssen(TA, TB, M7, pool, siguiente_pool_index);

    // Combinar resultados en C
    for (int i = 0; i < nuevo_n; i++) {
        for (int j = 0; j < nuevo_n; j++) {
            C11(i, j) = M1(i, j) + M4(i, j) - M5(i, j) + M7(i, j);
            C12(i, j) = M3(i, j) + M5(i, j);
            C21(i, j) = M2(i, j) + M4(i, j);
            C22(i, j) = M1(i, j) - M2(i, j) + M3(i, j) + M6(i, j);
        }
    }
}

/*int main() {
    //Considerar que n (Las dimensiones) son todas iguales y potencias de 2
    int n;
    vector<int> dataA, dataB, dataC;

    leer_matriz("A.txt", dataA, n);
    leer_matriz("B.txt", dataB, n);

    dataC.assign(n * n, 0);
    vector<int> dataC_std(n * n, 0);
    Matrix A{dataA, n, 0, n}, B{dataB, n, 0, n}, C{dataC, n, 0, n};
    Matrix C_std{dataC_std, n, 0, n};

    if (n != A.n || n != B.n) {
        cerr << "Error: Las dimensiones de las matrices no coinciden o no son potencias de 2." << endl;
        return 1;
    }

    auto start = chrono::high_resolution_clock::now();
    algoritmo(A, B, C);
    auto end = chrono::high_resolution_clock::now();
    cout << "Tiempo Strassen: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    Matrix C_std{dataC, n, 0, n};
    auto start_std = chrono::high_resolution_clock::now();
    multiplicar_standard(A, B, C_std);
    auto end_std = chrono::high_resolution_clock::now();
    cout << "Tiempo estándar: " << chrono::duration_cast<chrono::milliseconds>(end_std - start_std).count() << "ms" << endl;
    
    return 0;
}*/