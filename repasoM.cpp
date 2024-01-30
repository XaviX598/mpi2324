#include <iostream>
#include <mpi.h>
#include <vector>
#include <math.h>
#include <ctime>
#include <algorithm>

#define DIMENSION 101

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int block_size;
    int real_size = DIMENSION;

    if (DIMENSION % nprocs != 0) {
        real_size = std::ceil((double)DIMENSION / nprocs) * nprocs;
        block_size = real_size / nprocs;
    } else {
        block_size = DIMENSION / nprocs;
    }

    if (rank == 0) {
        std::vector<int> allData(real_size);

        // Generar el vector con datos aleatorios en el rango de 0 a 100
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        for (int i = 0; i < DIMENSION; ++i) {
            allData[i] = std::rand() % 101;
        }

        // Enviar datos a todos los procesos
        for (int i = 1; i < nprocs; ++i) {
            int start = i * block_size;
            MPI_Send(&allData[start], block_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Proceso 0 también trabaja con una parte de los datos
        int start = 0;
        std::vector<int> data_local(allData.begin() + start, allData.begin() + start + block_size);

        std::printf("rank_%d: ", rank);
        for (int i = 0; i < data_local.size(); i++) {
            std::printf("%d ", data_local[i]);
        }
        std::printf("\n");

    } else {
        // Recibir datos
        std::vector<int> data_local(block_size);
        MPI_Recv(data_local.data(), block_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::printf("rank_%d: ", rank);
        for (int i = 0; i < data_local.size(); i++) {
            std::printf("%d ", data_local[i]);
        }
        std::printf("\n");
    }

    MPI_Finalize();

    return 0;
}
