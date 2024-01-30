#include <iostream>
#include <mpi.h>
#include <vector>
#include <math.h>
#include <ctime>
#include <algorithm>

#define  DIMENSION 31

double promedio(int* tmp, int block_size){
    int suma = 0;
    for(int i=0; i<block_size ; i++){
        suma = tmp[i] + suma;
    }
    return (double) suma/block_size;
}


int main(int argc, char** argv){

    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //para saber el id del rank
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); //para sacar el numero total de procesos

    int block_size;
    int real_size = DIMENSION;
    int padding = 0;

    if(DIMENSION%nprocs!=0){
        real_size = std::ceil((double)DIMENSION/nprocs) * nprocs; //aqui sabremos el total de size de la matriz, en elc asod e que dimension no sea exacto, por ejemplo 101 / 4 procesos al no ser igual, seria 25.25 que seria 26 y esto multiplicado por 4 seria 104
        block_size = real_size/nprocs; //entonces aqui se haria 104 / 4 = 26
        padding = real_size - DIMENSION; //para saber cuantos datos seria de sobrante seria 104 - 101 = 3
    }

    if(rank==0){

        std::vector<int> allData(real_size);


        //-----------------------generar un vector con datos randomicos---------------------
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        // Generar el vector con datos aleatorios en el rango de 0 a 100
        for (int i = 0; i < 101; ++i) {
            allData[i] = std::rand() % 101;  // Números aleatorios entre 0 y 100
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

        std::printf("rank_%d: ", rank );
        for(int i=0; i<allData.size(); i++){
            std::printf("%d ", allData[i]);

        }


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