//ejemplo de como sumar un vecto con n 4 con in_place

#include <iostream>
#include <mpi.h>
#include <vector>
#include <math.h>

//tamaño del vector
#define MAX_ITEMS 25

int sumar(int*tmp, int n){
    int suma = 0;
    for(int i = 0;i<n;i++ ){
        suma+=tmp[i];
    }
    return suma;
}

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);


    int block_size;
    int real_size;
    int padding = 0;

    if(MAX_ITEMS%nprocs!=0){
        real_size = std::ceil((double)MAX_ITEMS/nprocs) * nprocs;
        block_size = real_size/nprocs;
        padding = real_size - MAX_ITEMS;
    }



    //inicializar
    if(rank==0){

        int suma_total = 0;

        std::vector<int> data;

        data.resize((real_size));

        std::printf("Dimension: %d, real_size: %d, block_size: %d, padding: %d\n",
                    MAX_ITEMS, real_size, block_size, padding);

        for(int i = 0; i<MAX_ITEMS;i++){
            data[i]=i;
        }

        MPI_Scatter(data.data(), block_size, MPI_INT,
                    MPI_IN_PLACE, 0, MPI_INT,
                    0, MPI_COMM_WORLD);

        int sum = sumar(data.data(), block_size);

        MPI_Reduce(MPI_IN_PLACE, &suma_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD );

        suma_total = suma_total + sum;

        std::printf("Suma total= %d\n", suma_total);

    }else{

        std::vector<int> data_local(block_size);

        //enviar los datos, que, cuanto, tipo, DONDE recibimos, cuantos recibimos, tipo, root, grupo
        MPI_Scatter(nullptr, 0, MPI_INT,
                    data_local.data(), block_size, MPI_INT,
                    0, MPI_COMM_WORLD);

        if(rank==nprocs-1){
            block_size = block_size-padding;
        }

        int suma_parcial = sumar(data_local.data(), block_size);

        std::printf("RANK_%d: suma parcial = %d\n", rank, suma_parcial);

        MPI_Reduce(&suma_parcial, MPI_IN_PLACE, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD );



    }

    MPI_Finalize();

    return 0;

}
