#include <sycl.hpp>
using namespace paras::sycl;

static const int N = 256000000;
static const int work_group_size = 1024;

int main(){
  queue q{gpu_selector{}};

  std::cout << "Device: "<< q.get_device().get_info<info::device::name>() << std::endl;

  int *data = malloc_shared<int>(N, q);
  for(int i=0; i<N; i++) data[i] = i;

  q.parallel_for(nd_range<1>(N, work_group_size), [=] (nd_item<1> item){
    int i = item.get_global_id(0);
    data[i] *= 2;
  }).wait();

  for(int i=0; i<4; i++) std::cout << data[i] << std::endl;
  free(data, q);
  return 0;
}
