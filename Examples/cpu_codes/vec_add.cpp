#include<sycl.hpp>
#include<math.h>
using namespace paras::sycl;

constexpr size_t N = 10;

int main() {

    // Create a SYCL queue with the gpu_selector
    	queue myQueue{cpu_selector{}} ;

		std::cout << "Device: "<< myQueue.get_device().get_info<info::device::name>() << std::endl;

    // Allocate memory on the host for vectors A, B, and C
    	std::vector<int> A(N, 1);
    	std::vector<int> B(N, 2);
    	std::vector<int> C(N, 0);

    // Allocate SYCL buffers for vectors A, B, and C
    	buffer<int, 1> bufferA(A.data(), range<1>(N));
    	buffer<int, 1> bufferB(B.data(), range<1>(N));
    	buffer<int, 1> bufferC(C.data(), range<1>(N));


    // Submit a SYCL kernel to the queue
    	myQueue.submit([&](handler& cgh) {
        // Access the buffer data
        auto accessorA = bufferA.get_access<access::mode::read_write>(cgh);
        auto accessorB = bufferB.get_access<access::mode::read_write>(cgh);
        auto accessorC = bufferC.get_access<access::mode::read_write>(cgh);

        // Define a SYCL kernel (lambda function)
        cgh.parallel_for<class VectorAddition>(range<1>(N), [=](id<1> index) {
            accessorC[index] = accessorA[index] + accessorB[index];
        });
    });
	// Wait for the queue to finish
	    myQueue.wait();

   

    // Print the result
	    auto D = bufferC.get_access<access::mode::read_write>();
    	std::cout<<"Result after vector addition:"<<std::endl;    
    	for (size_t i = 0; i < N; ++i) {
        std::cout << "C[" << i << "] = " << D[i] << "\n";
    }
	
    return 0;
}


