// ===================================================================
//                 VECTOR ADDITION USING SYCL BUFFERS 
// ===================================================================

#include <sycl/sycl.hpp>
#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <ctime>

using namespace sycl;
using namespace std;

int main() {

    size_t N;
    cout << "Enter size of vectors: ";
    cin >> N;

    srand(time(nullptr));

    // Host vectors
    vector<int> A(N), B(N), C(N);

    // Random initialization
    for (size_t i = 0; i < N; i++) {
        A[i] = rand() % 100;
        B[i] = rand() % 100;
    }

    // SYCL Queue
    queue q{ cpu_selector_v };
    cout << "\nSYCL Device: "
         << q.get_device().get_info<info::device::name>() << "\n\n";

    // Buffers
    buffer<int, 1> bufA(A.data(), range<1>(N));
    buffer<int, 1> bufB(B.data(), range<1>(N));
    buffer<int, 1> bufC(C.data(), range<1>(N));

    // Submit kernel
    q.submit([&](handler& cgh) {

        auto Aacc = bufA.get_access<access::mode::read>(cgh);
        auto Bacc = bufB.get_access<access::mode::read>(cgh);
        auto Cacc = bufC.get_access<access::mode::write>(cgh);

        cgh.parallel_for<class vec_add>(range<1>(N), [=](id<1> idx) {
            Cacc[idx] = Aacc[idx] + Bacc[idx];
        });

    });

    q.wait();

    // Read output
    auto Chost = bufC.get_access<access::mode::read>();

    // ===================================================================
    // FINAL OUTPUT TABLE 
    // ===================================================================

    cout << "====================== VECTOR ADDITION RESULT ======================\n\n";

    cout << left
         << setw(7)  << "Index"
         << setw(10) << "A[i]"
         << setw(10) << "B[i]"
         << setw(15) << "C[i] = A+B" << "\n";

    cout << "-------------------------------------------------------------\n";

    for (size_t i = 0; i < N; i++) {
        cout << left
             << setw(7)  << i
             << setw(10) << A[i]
             << setw(10) << B[i]
             << setw(15) << Chost[i] << "\n";
    }

    cout << "-------------------------------------------------------------\n";

    return 0;
}

