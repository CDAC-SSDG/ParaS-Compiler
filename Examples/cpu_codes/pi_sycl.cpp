#include <sycl.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip> // For setting the precision of the output

using namespace paras::sycl;

int main() {
    // Number of intervals - increase for higher precision
    const size_t n = 100000000; // 100 million intervals
    const double dx = 1.0 / n;

    // Create a queue to select the default device
    queue q(cpu_selector{});

	std::cout << "Device: "<< q.get_device().get_info<info::device::name>() << std::endl;

    // Buffer to store results
    std::vector<double> results(n, 0.0);
    {
        buffer<double, 1> result_buf(results.data(), range<1>(n));

        // Submit command group to the queue
        q.submit([&](handler& h) {
            // Get write access to the buffer on the device
            auto result_acc = result_buf.get_access<access::mode::write>(h);

            // Define the kernel
            h.parallel_for(range<1>(n), [=](id<1> i) {
                double x = (i[0] + 0.5) * dx;
                result_acc[i] = std::sqrt(1.0 - x * x) * dx;
            });
        });
    }

    // Sum up the results
    double pi = 0.0;
    for (const auto& val : results) {
        pi += val;
    }
    pi *= 4.0;

    // Set precision to 10 decimal places and output the result
    std::cout << std::fixed << std::setprecision(10) << "Approximation of Pi: " << pi << std::endl;

    return 0;
}

