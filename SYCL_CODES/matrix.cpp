// ============================================================================
//                          SYCL MATRIX MULTIPLICATION 
// ============================================================================

#include <sycl/sycl.hpp>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

using namespace sycl;
using namespace std;

int main() {

    size_t N;
    cout << "Enter matrix size N (NxN): ";
    cin >> N;

    srand(time(nullptr));

    queue q{ cpu_selector_v };
    cout << "\nSYCL Device: "
         << q.get_device().get_info<info::device::name>() << "\n\n";

    // Allocate USM memory
    int* A = malloc_shared<int>(N*N, q);
    int* B = malloc_shared<int>(N*N, q);
    int* C = malloc_shared<int>(N*N, q);

    // Random initialization
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < N; j++) {
            A[i*N + j] = rand() % 10;
            B[i*N + j] = rand() % 10;
        }

    // Kernel
    q.submit([&](handler& h) {
        h.parallel_for<class kernel>(range<2>(N, N), [=](id<2> idx) {
            size_t i = idx[0], j = idx[1];
            int sum = 0;
            for (size_t k = 0; k < N; k++)
                sum += A[i*N + k] * B[k*N + j];
            C[i*N + j] = sum;
        });
    });
    q.wait();

    // ---------------------------
    //     PRINT FOR MATRIX A
    // ---------------------------
    cout << "Matrix A:\n";
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++)
            cout << setw(4) << A[i*N + j];
        cout << "\n";
    }
    cout << "\n";

    // ---------------------------
    //    PRINT FOR MATRIX B
    // ---------------------------
    cout << "Matrix B:\n";
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++)
            cout << setw(4) << B[i*N + j];
        cout << "\n";
    }
    cout << "\n";

    // ---------------------------
    //   PRINT FOR RESULT C
    // ---------------------------
    cout << "Matrix C = A × B:\n";
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++)
            cout << setw(6) << C[i*N + j];
        cout << "\n";
    }

    free(A, q);
    free(B, q);
    free(C, q);

    return 0;
}

