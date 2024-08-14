#include <sycl.hpp>
using namespace paras::sycl;

static const int N = 256;
static const int work_group_size = 64;
static const int num_groups = 4;

int main(){
  queue q{cpu_selector{}};

  std::cout << "Device: "<< q.get_device().get_info<info::device::name>() << std::endl;

  int *data = malloc_shared<int>(N, q);
  for(int i=0; i<N; i++) data[i] = i;

  q.submit([&] (handler &h){
    h.parallel_for_work_group(range<1>(num_groups), range<1>(work_group_size), [=](group<1> g) {
      g.parallel_for_work_item([&](h_item<1> item) {
        int i = item.get_global_id(0);
        data[i] *= 2;
      });
    });
  }).wait();

  for(int i=0; i<4; i++) std::cout << data[i] << std::endl;
  free(data, q);
return 0;
}
