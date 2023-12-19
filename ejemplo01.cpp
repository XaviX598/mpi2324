#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    std::printf("Rank %d od %d procs\n", rank, nprocs);

    MPI_Finalize();

}

//para compilar mpicxx ejemplo01.cpp -o ejemplo01 (-o para el nombre que queremos para la salida)
//ejecutar mpiexec -n 4 ./ejemplo01