#include <sycl.hpp>
using namespace paras::sycl;

static const int N = 16;

int main() {
    queue q(cpu_selector{});
    std::cout << "Device: " << q.get_device().get_info<info::device::name>() << std::endl;

    std::vector<int> data(N);
    for(int i = 0; i < N; i++) data[i] = i;

    buffer<int, 1> buf(data.data(), range<1>(N));
    q.submit([&](handler& h) {
        auto A = buf.get_access<access::mode::read_write>(h);
        h.parallel_for<class k1>(range<1>(N), [=](id<1> i) {
            A[i] *= 2;
        });
    });

    auto B = buf.get_access<access::mode::read>();
    for(int i = 0; i < N; i++) std::cout << B[i] << std::endl;

    return 0;
}

