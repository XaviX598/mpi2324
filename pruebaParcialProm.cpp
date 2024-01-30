#include <iostream>
#include <mpi.h>
#include <vector>
#include <math.h>
#include <ctime>
#include <algorithm>
#include <fstream>

#define  DIMENSION 101

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

double promedio(int* tmp, int block_size) {
    int suma = 0;
    for (int i = 0; i < block_size; i++) {
        suma = tmp[i] + suma;
    }
    return static_cast<double>(suma) / block_size;
}

//realizo dos main ya que me arrojaba un problema por el tamaño del vector, lo hare abajo con datos de prueba para comprobar el funcionamiento
//CON LOS DATOS LEIDOS
/*
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int block_size;
    int real_size;
    int padding = 0;

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

        MPI_Scatter(file.data(), block_size, MPI_INT, MPI_IN_PLACE, 0, MPI_INT, 0, MPI_COMM_WORLD);

        double promedioTotal = 0;
        double prom = promedio(file.data(), block_size);
        std::printf("Promedio rank 0 = %f\n", prom);

        MPI_Reduce(&prom, &promedioTotal, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        promedioTotal += prom;

        if (rank == 0) {
            std::printf("Promedio total = %.2f\n", promedioTotal / nprocs);
        }

    } else {
        std::vector<int> file_local(block_size);

        if (rank == nprocs - 1) {
            file_local.resize(block_size - padding);
        }

        MPI_Scatter(nullptr, 0, MPI_DOUBLE,
                    file_local.data(), file_local.size(), MPI_INT,
                    0, MPI_COMM_WORLD);

//        std::printf("rank_%d: ", rank);
//        for (int i = 0; i < file_local.size(); i++) {
//            std::printf("%d ", file_local[i]);
//        }
//        std::printf("\n");

//        if (rank == nprocs - 1) {
//            block_size = block_size - padding;
//        }

        double promedioParcial = promedio(file_local.data(), block_size);
        std::printf("RANK_%d: promedio parcial = %.2f\n", rank, promedioParcial);

        MPI_Reduce(&promedioParcial, nullptr, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
*/

//CON DATOS DE PRUEBA
int main(int argc, char** argv){

    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //para saber el id del rank
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); //para sacar el numero total de procesos

    int block_size;
    int real_size = DIMENSION;
    int padding = 0;

    if(DIMENSION%nprocs!=0){
        real_size = std::ceil((double)DIMENSION/nprocs) * nprocs;
        block_size = real_size/nprocs;
        padding = real_size - DIMENSION;
    }

    if(rank==0){

        std::vector<int> allData(real_size);

        std::srand(static_cast<unsigned>(std::time(nullptr)));


        for (int i = 0; i < 101; ++i) {
            allData[i] = std::rand() % 101;
        }

        //para comprobar que si se lleno el vector
//        std::printf("El vector es: \n");
//        for(int i=0; i<allData.size(); i++){
//            std::printf("%d ", allData[i]);
//        }
//        std::printf("\n");

        MPI_Scatter(allData.data(), //bufer envio
                    block_size, //cuantos datos vamos a enviar
                    MPI_INT, //tipo de dato
                    MPI_IN_PLACE, 0 , MPI_INT, //para que el rank 0 no reciba nada, count de recepcion, tipo
                    0, //cual rank es el root
                    MPI_COMM_WORLD  ); //comunicador globa

//        std::printf("rank_%d: ", rank );
//        for(int i=0; i<allData.size(); i++){
//            std::printf("%d ", allData[i]);
//
//        }

//----------------------promedio-------------
        double promedioTotal = 0;
        double prom = promedio(allData.data(), block_size);
        std::printf("Promedio rank 0= %f\n", prom);
        MPI_Reduce(MPI_IN_PLACE, &promedioTotal, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );

        promedioTotal = promedioTotal + prom;

        std::printf("Promedio total= %.2f\n", promedioTotal/nprocs);

    }else {

        std::vector<int> allData_local(block_size);

        MPI_Scatter(nullptr,0,MPI_DOUBLE, //datos de envio, aqui va esto ya que estos no van a enviar
                    allData_local.data(), block_size, MPI_INT,
                    0, MPI_COMM_WORLD);

        std::printf("rank_%d: ", rank );
        for(int i=0; i<allData_local.size(); i++){
            std::printf("%d ", allData_local[i]);

        }
        std::printf("\n");

//-----------------------------------promedio----------------------------
        if(rank==nprocs-1){
            block_size = block_size-padding;
        }

        double promedioParcial = promedio(allData_local.data(), block_size);
        std::printf("RANK_%d: promedio parcial = %.2f\n", rank, promedioParcial);

        MPI_Reduce(&promedioParcial, MPI_IN_PLACE, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );

    }


    MPI_Finalize();

    return 0;

}
