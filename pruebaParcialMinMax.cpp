#include <iostream>
#include <mpi.h>
#include <vector>
#include <math.h>
#include <ctime>
#include <algorithm>
#include <fstream>

std::vector<int> read_file() {
    std::fstream fs("datos.txt", std::ios::in);
    std::string line;
    std::vector<int> ret;
    while (std::getline(fs, line)) {
        ret.push_back(std::stoi(line));
    }
    fs.close();

    return ret;
}

int minimo(int* tmp, int block_size) {
    int minimo = 0;
    for (int i = 0; i < block_size; i++) {
        if(tmp[i]>minimo){
            minimo = tmp[i];
        }
    }
    return minimo;
}

int maximo(int* tmp, int block_size) {
    int maximo = 0;
    for (int i = 0; i < block_size; i++) {
        if(tmp[i]>maximo){
            maximo = tmp[i];
        }
    }
    return maximo;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int block_size;
    int real_size;
    int padding = 0;

    int file_local[real_size];

    if (rank == 0) {

        auto file = read_file();

        if (file.size() % nprocs != 0) {
            real_size = std::ceil(static_cast<double>(file.size()) / nprocs) * nprocs;
            block_size = real_size / nprocs;
            padding = real_size - file.size();
        } else {
            real_size = file.size();
            block_size = real_size / nprocs;
            padding = real_size - file.size();
        }

        //para enviar datos a ranks i
        for(int rank_id =1 ;rank_id<nprocs ;rank_id++){
            std::printf("\nRank 0 enviando datos a RANK_%d\n", rank_id);

            int start = rank_id*block_size;

            MPI_Send(&file[start],  //enviamos los datos
                     block_size, //, cuantos datos
                     MPI_INT, // , tipo de dato
                     rank_id,  //, rank a enviar
                     0, //, tag
                     MPI_COMM_WORLD);//, grupo


            int minimo_rank0 = minimo(file.data(), block_size);
            std::printf("\nminimo del rank 0: %d\n", minimo_rank0);
        }

    } else {

//        file_local.resize(file);

        std::printf("\nRank_%d recibiendo datos\n", rank);

        MPI_Recv(file_local, //en donde recibimos,
                 block_size,//cuantos datos,
                 MPI_INT, //tipo,
                 0, // de donde recibimos,
                 0, //tag,
                 MPI_COMM_WORLD, //grupo,
                 MPI_STATUS_IGNORE);// status

        int minimo_parcial = minimo(file_local, block_size);
        std::printf("\nminimo parcial del rank %d: %d\n",rank, minimo_parcial);

//        MPI_Bcast(minimo_parcial.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}