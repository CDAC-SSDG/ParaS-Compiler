#include <iostream>
#include <vector>
#include <sycl.hpp>

namespace sycl = paras::sycl;

class DotProductKernel;

class DotProduct {
public:
    DotProduct(int N) : array_size(N) {
        // Initialize vectors with values 1 and 2
        d_a.resize(array_size, 1.0f);
        d_b.resize(array_size, 2.0f);
    }

    void computeDotProduct() {
        float result = 0.0f;

        // Create a SYCL device queue
        sycl::queue queue(sycl::gpu_selector{});
		std::cout << "Device: "<< queue.get_device().get_info<sycl::info::device::name>() << std::endl;

        // Create device buffers
        sycl::buffer<float, 1> d_a_buffer(d_a.data(), sycl::range<1>(array_size));
        sycl::buffer<float, 1> d_b_buffer(d_b.data(), sycl::range<1>(array_size));
        sycl::buffer<float, 1> d_sum_buffer(&result, sycl::range<1>(1));

        // Submit kernel
        queue.submit([&](sycl::handler &cgh) {
            // Accessors for input buffers
            auto ka = d_a_buffer.get_access<sycl::access::mode::read>(cgh);
            auto kb = d_b_buffer.get_access<sycl::access::mode::read>(cgh);

            // Accessor for output buffer
            auto sum_acc = d_sum_buffer.get_access<sycl::access::mode::read_write>(cgh);

            // Kernel
            cgh.parallel_for<DotProductKernel>(
                sycl::range<1>(array_size),
                sycl::reduction(sum_acc, sycl::plus<float>()),
                [=](sycl::id<1> idx, auto& sum) {
                    sum += ka[idx] * kb[idx];
                });
        }).wait();

		// Print the result
        auto D = d_sum_buffer.get_access<sycl::access::mode::read_write>();
        std::cout<<"Result after dot product:"<<std::endl;
       int i=0; 
        std::cout << " " << D[i] << "\n";

        // Return the result
      //  return result;
    }

private:
    std::vector<float> d_a;
    std::vector<float> d_b;
    int array_size;
};

int main() {
    // Define size N
    int N = 10000000;

    // Create DotProduct object
    DotProduct dotProduct(N);

    // Compute dot product
     dotProduct.computeDotProduct();

    // Print result
    //std::cout << "Dot Product: " << result << std::endl;

    return 0;
}

