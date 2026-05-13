#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <string>

using namespace std;

vector<vector<int>> add_matrices(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    int n = A.size();
    vector<vector<int>> result(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            result[i][j] = A[i][j] + B[i][j];
        }
    }
    return result;
}

vector<vector<int>> subtract_matrices(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    int n = A.size();
    vector<vector<int>> result(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            result[i][j] = A[i][j] - B[i][j];
        }
    }
    return result;
}

tuple<vector<vector<int>>, vector<vector<int>>, vector<vector<int>>, vector<vector<int>>> div_matrix(vector<vector<int>>& matrix) {
    int n = matrix.size();
    vector<vector<int>> M11(n/2, vector<int>(n/2));
    vector<vector<int>> M12(n/2, vector<int>(n/2));
    vector<vector<int>> M21(n/2, vector<int>(n/2));
    vector<vector<int>> M22(n/2, vector<int>(n/2));
    for (int i = 0; i < n/2; i++) {
        for (int j = 0; j < n/2; j++) {
            M11[i][j] = matrix[i][j];
            M12[i][j] = matrix[i][j + n/2];
            M21[i][j] = matrix[i + n/2][j];
            M22[i][j] = matrix[i + n/2][j + n/2];
        }
    }
    return make_tuple(M11, M12, M21, M22);
}

vector<vector<int>> algoritmo(vector<vector<int>>& A, vector<vector<int>>& B) {
    int n = A.size();
    if (n == 1) {
        return {{A[0][0] * B[0][0]}};
    }

    auto [A11, A12, A21, A22] = div_matrix(A);
    auto [B11, B12, B21, B22] = div_matrix(B);

    vector<vector<int>> M1 = algoritmo(add_matrices(A11, A22), add_matrices(B11, B22));
    vector<vector<int>> M2 = algoritmo(add_matrices(A21, A22), B11);
    vector<vector<int>> M3 = algoritmo(A11, subtract_matrices(B12, B22));
    vector<vector<int>> M4 = algoritmo(A22, subtract_matrices(B21, B11));
    vector<vector<int>> M5 = algoritmo(add_matrices(A11, A12), B22);
    vector<vector<int>> M6 = algoritmo(subtract_matrices(A21, A11), add_matrices(B11, B12));
    vector<vector<int>> M7 = algoritmo(subtract_matrices(A12, A22), add_matrices(B21, B22));

    vector<vector<int>> C11 = add_matrices(subtract_matrices(add_matrices(M1, M4), M5), M7);
    vector<vector<int>> C12 = add_matrices(M3, M5);
    vector<vector<int>> C21 = add_matrices(M2, M4);
    vector<vector<int>> C22 = add_matrices(subtract_matrices(add_matrices(M1, M3), M2), M6);

    vector<vector<int>> C(n, vector<int>(n));
    for (int i = 0; i < n / 2; i++) {
        for (int j = 0; j < n / 2; j++) {
            C[i][j] = C11[i][j];
            C[i][j + n / 2] = C12[i][j];
            C[i + n / 2][j] = C21[i][j];
            C[i + n / 2][j + n / 2] = C22[i][j];
        }
    }

    return C;
}

int main() {
    int n = 4; 
    vector<vector<int>> A(n, vector<int>(n));
    vector<vector<int>> B(n, vector<int>(n));

    for (int i = 0; i < n; i++) {  
        for (int j = 0; j < n; j++) {  
            A[i][j] = i * n + j + 1;
            B[i][j] = i * n + j + 1;
        }  
    }

    vector<vector<int>> C = algoritmo(A, B);
  
}