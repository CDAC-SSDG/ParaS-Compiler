#include <sycl/sycl.hpp>
#include <iostream>
#include <vector>
#include <iomanip>

using namespace sycl;
using namespace std;

int main() {

    // -------------------------------
    // USER INPUT
    // -------------------------------
    size_t width, height;
    cout << "Enter image width  : ";
    cin >> width;
    cout << "Enter image height : ";
    cin >> height;

    // -------------------------------
    // CREATE IMAGE WITH SAMPLE VALUES
    // -------------------------------
    vector<int> image(width * height);
    for (size_t i = 0; i < width * height; ++i)
        image[i] = i + 1;  // sequential values 1,2,3,...

    // 3×3 Sobel kernel (example)
    const vector<int> kernel = {
         1,  0, -1,
         2,  0, -2,
         1,  0, -1
    };

    vector<int> result(width * height, 0);

    queue q;
    cout << "\nSYCL Device: "
         << q.get_device().get_info<info::device::name>() << "\n\n";

    // -------------------------------
    // BUFFERS
    // -------------------------------
    buffer<int> imageBuf(image.data(), range<1>(width * height));
    buffer<int> kernelBuf(kernel.data(), range<1>(9));
    buffer<int> resultBuf(result.data(), range<1>(width * height));

    // -------------------------------
    // KERNEL SUBMISSION
    // -------------------------------
    q.submit([&](handler &cgh) {
        auto img  = imageBuf.get_access<access::mode::read>(cgh);
        auto ker  = kernelBuf.get_access<access::mode::read>(cgh);
        auto out  = resultBuf.get_access<access::mode::write>(cgh);

        cgh.parallel_for<class convolution>(
            range<2>(height, width),  // (rows, cols)
            [=](item<2> id) {

                int x = id[0];
                int y = id[1];

                int sum = 0;

                // Apply 3×3 convolution
                for (int kx = -1; kx <= 1; ++kx) {
                    for (int ky = -1; ky <= 1; ++ky) {

                        int ix = x + kx;
                        int iy = y + ky;

                        // Boundary check
                        if (ix >= 0 && ix < height && iy >= 0 && iy < width) {
                            sum += img[ix * width + iy] *
                                   ker[(kx + 1) * 3 + (ky + 1)];
                        }
                    }
                }

                out[x * width + y] = sum;
            });
    });

    q.wait();

    // -------------------------------
    // PRINT INPUT IMAGE
    // -------------------------------
    cout << "Input Image:\n";
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j)
            cout << setw(4) << image[i * width + j];
        cout << "\n";
    }
    cout << "\n";

    // -------------------------------
    // PRINT KERNEL
    // -------------------------------
    cout << "Kernel (3x3):\n";
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j)
            cout << setw(4) << kernel[i * 3 + j];
        cout << "\n";
    }
    cout << "\n";

    // -------------------------------
    // PRINT RESULT
    // -------------------------------
    auto resHost = resultBuf.get_access<access::mode::read>();

    cout << "Convolution Result:\n";
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j)
            cout << setw(6) << resHost[i * width + j];
        cout << "\n";
    }
    cout << "\n";

    return 0;
}

