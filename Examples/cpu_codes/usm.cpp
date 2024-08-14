#include <sycl.hpp>
using namespace paras::sycl;

static const int N = 16;

int main(){
  queue q{cpu_selector{}};

  std::cout << "Device: "<< q.get_device().get_info<info::device::name>() << std::endl;

  int *data = malloc_shared<int>(N, q);
  for(int i=0; i<N; i++) data[i] = i;

  q.parallel_for(range<1>(N), [=] (id<1> i){
    data[i] *= 2;
  }).wait();

  for(int i=0; i<N; i++) std::cout << data[i] << std::endl;
  free(data, q);
  return 0;
}

