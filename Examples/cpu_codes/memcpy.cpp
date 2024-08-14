#include <sycl.hpp>
using namespace paras::sycl;

static const int N = 16;

  int main() {
  queue q;

  std::cout << "Device: "<< q.get_device().get_info<info::device::name>() << std::endl;

  int *data = static_cast<int *>(malloc(N * sizeof(int)));
  
  for (int i = 0; i < N; i++) data[i] = i;

  int *data_device = static_cast<int *>(malloc_device(N * sizeof(int), q));

  // Copy memory from host to device
  q.memcpy(data_device, data, sizeof(int) * N);

  // Computation on device
  q.parallel_for(range<1>(N), [=](id<1> i) { 
    data_device[i] *= 2; 
  });

  // Copy back memory from device to host
  q.memcpy(data, data_device, sizeof(int) * N);
  
  q.wait();

  for (int i = 0; i < N; i++) std::cout << data[i] << std::endl;
  free(data_device, q);
  free(data);
  return 0;
}
