// ============================================================================
//                  π APPROXIMATION USING SYCL PARALLEL INTEGRATION
// 
//   Method:
//      π = 4 * ∫₀¹ sqrt(1 - x²) dx
//
//   Using Riemann Sum with N intervals computed in parallel on SYCL device.
// ============================================================================

#include <sycl/sycl.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace sycl;
using namespace std;

int main() {

    // Number of intervals (increase for higher accuracy)
    const size_t n = 100000000;  // 100 million intervals
    const double dx = 1.0 / n;

    // Select SYCL device
    queue q;
    cout << "SYCL Device: " 
         << q.get_device().get_info<info::device::name>() 
         << "\n\n";

    // Host vector to store partial results
    vector<double> results(n, 0.0);

    {
        // Buffer creation
        buffer<double, 1> result_buf(results.data(), range<1>(n));

        // Submit kernel to queue
        q.submit([&](handler &h) {

            auto result_acc = result_buf.get_access<access::mode::write>(h);

            h.parallel_for<class pi_kernel>(range<1>(n), [=](id<1> i) {
                double x = (i[0] + 0.5) * dx;
                result_acc[i] = sycl::sqrt(1.0 - x * x) * dx;
            });

        });
        // Buffer goes out of scope → data copied back to host
    }

    // Compute π by summing partial results
    double pi = 0.0;
    for (double val : results)
        pi += val;

    pi *= 4.0;

    // Print result with precision
    cout << fixed << setprecision(10);
    cout << "Intervals used      :  " << n << "\n";
    cout << "Approximation of π  :  " << pi << "\n";
    return 0;
}

