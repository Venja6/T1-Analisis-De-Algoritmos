#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <string>
#include <chrono>

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

void algoritmo(Matrix& A, Matrix& B, Matrix& C) {
    int n = A.n; //Tamaño de la matriz/submatriz actual 
    if (n == 32) {
        multiplicar_standard(A, B, C); //Multiplicación estándar para matrices pequeñas (El comportamiento es muy similar cuando ambas soluciones se usan en matrices pequeñas)
        return;
    }
    //Inicio del paso de división de la matriz en submatrices
    int nuevo_n = n / 2;
    int size = nuevo_n * nuevo_n; //tamaño de cada submatriz

    //Creación de submatrices utilizando el mismo vector de datos pero con diferentes puntos de inicio y tamaños
    //Esto evita crear nuevas matrices y reduce el uso de memoria
    auto sub_matriz = [&](Matrix& M, int row, int col) {
        return Matrix{M.datos, M.largo_original, M.punto_inicio + (row * nuevo_n * M.largo_original) + (col * nuevo_n), nuevo_n};
    };

    Matrix A11 = sub_matriz(A, 0, 0), A12 = sub_matriz(A, 0, 1), A21 = sub_matriz(A, 1, 0), A22 = sub_matriz(A, 1, 1);
    Matrix B11 = sub_matriz(B, 0, 0), B12 = sub_matriz(B, 0, 1), B21 = sub_matriz(B, 1, 0), B22 = sub_matriz(B, 1, 1);
    Matrix C11 = sub_matriz(C, 0, 0), C12 = sub_matriz(C, 0, 1), C21 = sub_matriz(C, 1, 0), C22 = sub_matriz(C, 1, 1);

    // Creación de matrices temporales para almacenar los resultados de las operaciones intermedias (M1 a M7)
    vector<int> m1(size, 0), m2(size, 0), m3(size, 0), m4(size, 0), m5(size, 0), m6(size, 0), m7(size, 0);
    vector<int> tA(size), tB(size);
    Matrix M1{m1, nuevo_n, 0, nuevo_n}, M2{m2, nuevo_n, 0, nuevo_n}, M3{m3, nuevo_n, 0, nuevo_n},
           M4{m4, nuevo_n, 0, nuevo_n}, M5{m5, nuevo_n, 0, nuevo_n}, M6{m6, nuevo_n, 0, nuevo_n},
           M7{m7, nuevo_n, 0, nuevo_n};
    // Matrices temporales para almacenar los resultados de las operaciones intermedias (sumas/restas de submatrices)
    Matrix TA{tA, nuevo_n, 0, nuevo_n}, TB{tB, nuevo_n, 0, nuevo_n};

    // M1 = (A11 + A22) * (B11 + B22)
    operar_matriz(A11, A22, TA); operar_matriz(B11, B22, TB);
    algoritmo(TA, TB, M1);

    // M2 = (A21 + A22) * B11
    operar_matriz(A21, A22, TA);
    algoritmo(TA, B11, M2);

    // M3 = A11 * (B12 - B22)
    operar_matriz(B12, B22, TB, true);
    algoritmo(A11, TB, M3);

    // M4 = A22 * (B21 - B11)
    operar_matriz(B21, B11, TB, true);
    algoritmo(A22, TB, M4);

    // M5 = (A11 + A12) * B22
    operar_matriz(A11, A12, TA);
    algoritmo(TA, B22, M5);

    // M6 = (A21 - A11) * (B11 + B12)
    operar_matriz(A21, A11, TA, true); operar_matriz(B11, B12, TB);
    algoritmo(TA, TB, M6);

    // M7 = (A12 - A22) * (B21 + B22)
    operar_matriz(A12, A22, TA, true); operar_matriz(B21, B22, TB);
    algoritmo(TA, TB, M7);

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


int main() {
    int n = 2048;
    vector<int> dataA(n * n, 1), dataB(n * n, 2), dataC(n * n, 0);

    Matrix A{dataA, n, 0, n}, B{dataB, n, 0, n}, C{dataC, n, 0, n};

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
}