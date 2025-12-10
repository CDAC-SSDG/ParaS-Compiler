#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>
#include <sycl.hpp>

#define SIZE 512
#define MCW MPI_COMM_WORLD
#define MAS MPI_ANY_SOURCE

using namespace paras::sycl;

class Matrix {
private:
    int *m1, *m2, *result;
    queue Q;

public:
    Matrix(int rank) {
        m1 = malloc_shared<int>(SIZE * SIZE, Q);
        m2 = malloc_shared<int>(SIZE * SIZE, Q);
        result = malloc_shared<int>(SIZE * SIZE, Q);

        // Initialize matrices only for rank 0 process
        if (rank == 0) {
            srand(time(0));
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    m1[i * SIZE + j] = 1;
                    m2[i * SIZE + j] = 1;
                    result[i * SIZE + j] = 0;
                }
            }
        }
    }

    ~Matrix() {
        free(m1, Q);
        free(m2, Q);
        free(result, Q);
    }

    void display() {
        std::cout << "Result:\n";
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++)
                std::cout << result[i * SIZE + j] << " ";
            std::cout << std::endl;
        }
    }

    void multiply(int start, int end) {
        Q.submit([&](handler &cgh) {
            cgh.parallel_for<class mmult>(
                range<2>(end - start, SIZE), [=](id<2> idx) {
                    int i = start + idx[0];
                    int j = idx[1];
                    result[i * SIZE + j] = 0;  // Initialize result element
                    for (int k = 0; k < SIZE; k++) {
                        result[i * SIZE + j] += m1[i * SIZE + k] * m2[k * SIZE + j];
                    }
                });
        }).wait();
    }

    int* get_address(int matrix_number, int index_i, int index_j) {
        if (matrix_number == 1)
            return &m1[index_i * SIZE + index_j];
        if (matrix_number == 2)
            return &m2[index_i * SIZE + index_j];
        if (matrix_number == 3)
            return &result[index_i * SIZE + index_j];
        return nullptr;
    }
};

int main() {
    int mpi_rank, mpi_size;
    time_t st, et;
    int start, end, epp;

    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MCW, &mpi_size);
    MPI_Comm_rank(MCW, &mpi_rank);

    Matrix obj(mpi_rank);

    epp = SIZE * SIZE / mpi_size;
    start = mpi_rank * SIZE / mpi_size;
    end = start + SIZE / mpi_size;

    if (mpi_rank != 0) {
        // Scatter and broadcast data to worker nodes
        MPI_Scatter(nullptr, epp, MPI_INT, obj.get_address(1, start, 0), epp, MPI_INT, 0, MCW);
        MPI_Bcast(obj.get_address(2, 0, 0), SIZE * SIZE, MPI_INT, 0, MCW);

        // Perform partial matrix multiplication
        obj.multiply(start, end);

        // Gather results from worker nodes
        MPI_Gather(obj.get_address(3, start, 0), epp, MPI_INT, nullptr, epp, MPI_INT, 0, MCW);
    } else {
        // Root process initializes data
        st = time(nullptr);

        MPI_Scatter(obj.get_address(1, 0, 0), epp, MPI_INT, obj.get_address(1, 0, 0), epp, MPI_INT, 0, MCW);
        MPI_Bcast(obj.get_address(2, 0, 0), SIZE * SIZE, MPI_INT, 0, MCW);

        obj.multiply(start, end);

        MPI_Gather(obj.get_address(3, start, 0), epp, MPI_INT, obj.get_address(3, 0, 0), epp, MPI_INT, 0, MCW);

        et = time(nullptr);
        std::cout << "Matrix size: " << SIZE << " Execution time: " << et - st << " seconds" << std::endl;

                //obj.display();
    }

    MPI_Finalize();
    return 0;
}
