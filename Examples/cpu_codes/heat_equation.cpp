
#include <iostream>
#include <chrono>
#include <cmath>

#include <sycl.hpp>

using  namespace paras::sycl;

// Key constants used in this program
#define PI  paras::sycl::acos(-1.0) // Pi
#define LINE "--------------------" // A line for fancy output

// Function definitions
void initial_value( queue &queue, const unsigned int n, const double dx, const double length,  buffer<double,2>& u);
void zero( queue &queue, const unsigned int n,  buffer<double,2>& u);
void solve( queue &queue, const unsigned int n, const double alpha, const double dx, const double dt,  buffer<double,2>& u,  buffer<double,2>& u_tmp);
double solution(const double t, const double x, const double y, const double alpha, const double length);
double l2norm(const unsigned int n, const double * u, const int nsteps, const double dt, const double alpha, const double dx, const double length);

// Main function
int main(int argc, char *argv[]) {

  // Start the total program runtime timer
  auto start = std::chrono::high_resolution_clock::now();

  // Problem size, forms an nxn grid
  unsigned int n = 6000;

  // Number of timesteps
  int nsteps = 2;


  // Check for the correct number of arguments
  // Print usage and exits if not correct
  if (argc == 3) {

    // Set problem size from first argument
    n = atoi(argv[1]);
    if (n < 0) {
      std::cerr << "Error: n must be positive" << std::endl;
      exit(EXIT_FAILURE);
    }

    // Set number of timesteps from second argument
    nsteps = atoi(argv[2]);
    if (nsteps < 0) {
      std::cerr << "Error: nsteps must be positive" << std::endl;
      exit(EXIT_FAILURE);
    }
  }


  //
  // Set problem definition
  //
  double alpha = 0.1;          // heat equation coefficient
  double length = 1000.0;      // physical size of domain: length x length square
  double dx = length / (n+1);  // physical size of each cell (+1 as don't simulate boundaries as they are given)
  double dt = 0.5 / nsteps;    // time interval (total time of 0.5s)


  // Stability requires that dt/(dx^2) <= 0.5,
  double r = alpha * dt / (dx * dx);

  // Initalise SYCL queue on a GPU device
   queue queue { cpu_selector{}};

  // Print message detailing runtime configuration
  std::cout
    << std::endl
    << " MMS heat equation" << std::endl << std::endl
    << LINE << std::endl
    << "Problem input" << std::endl << std::endl
    << " Grid size: " << n << " x " << n << std::endl
    << " Cell width: " << dx << std::endl
    << " Grid length: " << length << "x" << length << std::endl
    << std::endl
    << " Alpha: " << alpha << std::endl
    << std::endl
    << " Steps: " <<  nsteps << std::endl
    << " Total time: " << dt*(double)nsteps << std::endl
    << " Time step: " << dt << std::endl
    << " SYCL device: " << queue.get_device().get_info< info::device::name>() << std::endl
    << LINE << std::endl;

  // Stability check
  std::cout << "Stability" << std::endl << std::endl;
  std::cout << " r value: " << r << std::endl;
  if (r > 0.5)
    //std::cout << " Warning: unstable" << std::endl;
  std::cout << LINE << std::endl;


  // Allocate two nxn grids
   buffer<double, 2> u{ range<2>{n,n}};
   buffer<double, 2> u_tmp{ range<2>{n,n}};

  // Set the initial value of the grid under the MMS scheme
  initial_value(queue, n, dx, length, u);
  zero(queue, n, u_tmp);

  // Ensure everything is initalised on the device
  queue.wait();

  //
  // Run through timesteps under the paras::sycl::explicit scheme
  //

  // Start the solve timer
  auto tic = std::chrono::high_resolution_clock::now();
  for (int t = 0; t < nsteps; ++t) {

    // Call the solve kernel
    // Computes u_tmp at the next timestep
    // given the value of u at the current timestep
    solve(queue, n, alpha, dx, dt, u, u_tmp);

    // Pointer swap
    auto tmp = std::move(u);
    u = std::move(u_tmp);
    u_tmp = std::move(tmp);
  }
  // Stop solve timer
  queue.wait();
  auto toc = std::chrono::high_resolution_clock::now();

  // Get access to u on the host
  double *u_host = u.get_access< access::mode::read>().get_pointer();

  //
  // Check the L2-norm of the computed solution
  // against the *known* solution from the MMS scheme
  //
  double norm = l2norm(n, u_host, nsteps, dt, alpha, dx, length);

  // Stop total timer
  auto stop = std::chrono::high_resolution_clock::now();

  // Print results
  std::cout
    << "Results" << std::endl << std::endl
    << "(L2norm): " << norm << std::endl
    << "Solve time (s): " << std::chrono::duration_cast<std::chrono::duration<double>>(toc-tic).count() << std::endl
    << "Total time (s): " << std::chrono::duration_cast<std::chrono::duration<double>>(stop-start).count() << std::endl
    << "Bandwidth (GB/s): " << 1.0E-9*2.0*n*n*nsteps*sizeof(double)/std::chrono::duration_cast<std::chrono::duration<double>>(toc-tic).count() << std::endl
    << LINE << std::endl;

}


// Sets the mesh to an initial value, determined by the MMS scheme
void initial_value( queue& queue, const unsigned int n, const double dx, const double length,  buffer<double,2>& u) {

  queue.submit([&]( handler& cgh) {
    auto ua = u.get_access< access::mode::discard_write>(cgh);

    cgh.parallel_for<class initial_value_kernel>( range<2>{n, n}, [=]( id<2> idx) {
      int i = idx[1];
      int j = idx[0];
      double y = dx * (j+1); // Physical y position
      double x = dx * (i+1); // Physical x position
      ua[idx] =  paras::sycl::sin(PI * x / length) *  paras::sycl::sin(PI * y / length);
    });
  });
}


// Zero the array u
void zero( queue& queue, const unsigned int n,  buffer<double,2>& u) {

  queue.submit([&]( handler& cgh) {
    auto ua = u.get_access< access::mode::discard_write>(cgh);

    cgh.parallel_for<class zero_kernel>( range<2>{n,n}, [=]( id<2> idx) {
      ua[idx] = 0.0;
    });
  });

}


// Compute the next timestep, given the current timestep
void solve( queue& queue, const unsigned int n, const double alpha, const double dx, const double dt,  buffer<double,2>& u_b,  buffer<double,2>& u_tmp_b) {

  // Finite difference constant multiplier
  const double r = alpha * dt / (dx * dx);
  const double r2 = 1.0 - 4.0*r;

  queue.submit([&]( handler& cgh) {
    auto u_tmp = u_tmp_b.get_access< access::mode::discard_write>(cgh);
    auto u = u_b.get_access< access::mode::read>(cgh);

    // Loop over the nxn grid
    cgh.parallel_for<class solve_kernel>( range<2>{n, n}, [=]( id<2> idx) {
      size_t j = idx[0];
      size_t i = idx[1];

      // Update the 5-point stencil, uparas::sycl::sing boundary conditions on the edges of the domain.
      // Boundaries are zero because the MMS solution is zero there.
      u_tmp[j][i] =  r2 * u[j][i] +
      r * ((i < n-1) ? u[j][i+1] : 0.0) +
      r * ((i > 0)   ? u[j][i-1] : 0.0) +
      r * ((j < n-1) ? u[j+1][i] : 0.0) +
      r * ((j > 0)   ? u[j-1][i] : 0.0);
    });
  });
}


// True answer given by the manufactured solution
double solution(const double t, const double x, const double y, const double alpha, const double length) {

  return paras::sycl::exp(-2.0*alpha*PI*PI*t/(length*length)) * paras::sycl::sin(PI*x/length) * paras::sycl::sin(PI*y/length);

}


// Computes the L2-norm of the computed grid and the MMS known solution
// The known solution is the same as the boundary function.
double l2norm(const unsigned int n, const double * u, const int nsteps, const double dt, const double alpha, const double dx, const double length) {

  // Final (real) time simulated
  double time = dt * (double)nsteps;

  // L2-norm error
  double l2norm = 0.0;

  // Loop over the grid and compute difference of computed and known solutions as an L2-norm
  double y = dx;
  for (int j = 0; j < n; ++j) {
    double x = dx;
    for (int i = 0; i < n; ++i) {
      double answer = solution(time, x, y, alpha, length);
      l2norm += (u[i+j*n] - answer) * (u[i+j*n] - answer);

      x += dx;
    }
    y += dx;
  }

  return paras::sycl::sqrt(l2norm);

}

