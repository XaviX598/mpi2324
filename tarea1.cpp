#include <iostream>
#include <mpi.h>

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

    //para declarar el vectorsin inicializar
    int arraySize = 200;
    int data[arraySize];

    int blocksize = arraySize/nprocs;

    if(rank==0){

        //llenamos el vector, solo 1 vez entonces en el rank 0
        for (int i = 1; i <= arraySize; ++i) {
            data[i - 1] = i;
        }

        // Enviar datos a ranks i
        for (int rank_id = 1; rank_id < nprocs; rank_id++) {

            int start = (rank_id - 1) * blocksize;
            int end = start + blocksize;

            MPI_Send(&data[start],  // Enviamos los datos
                     end - start,     // Cuántos datos
                     MPI_INT,         // Tipo de dato
                     rank_id,         // Rank a enviar
                     0,               // Tag
                     MPI_COMM_WORLD); // Grupo
        }

        int suma_ranks[nprocs];

        // Desde donde va a sumar el rank 0
        int start = blocksize * (nprocs - 1);

        std::printf("\nDatos recibidos en Rank 0: ");
        for (int i = start; i < arraySize; i++) {
            std::printf("%d ", data[i]);
        }
        std::printf("\n");

        suma_ranks[0] = sumar(&data[start], arraySize - start);


        //recibe las sumas parciales
        for(int rank_id =1 ;rank_id<nprocs ;rank_id++){
            MPI_Recv(&suma_ranks[rank_id], //en donde recibimos,
                     1,//cuantos datos,
                     MPI_INT, //tipo,
                     rank_id, // de donde recibimos,
                     0, //tag,
                     MPI_COMM_WORLD, //grupo,
                     MPI_STATUS_IGNORE);// status
        }

        std::printf("\nSumas parciales: ");
        for (int i = 0; i < nprocs; i++) {
            std::printf("%d", suma_ranks[i]);
            if (i < nprocs - 1) {
                std::printf(", ");
            }
        }

        int suma_total = sumar(suma_ranks, nprocs);

        std::printf("\nSuma total: %d \n", suma_total);

    }else {

        MPI_Recv(data, //en donde recibimos,
                 blocksize,//cuantos datos,
                 MPI_INT, //tipo,
                 0, // de donde recibimos,
                 0, //tag,
                 MPI_COMM_WORLD, //grupo,
                 MPI_STATUS_IGNORE);// status

        std::printf("\nDatos recibidos en Rank_%d: ", rank);
        for (int i = 0; i < blocksize; i++) {
            std::printf("%d ", data[i]);
        }
        std::printf("\n");

        int suma_parcial = sumar(data, blocksize);

        MPI_Send(&suma_parcial,  //enviamos los datos
                 1, //, cuantos datos
                 MPI_INT, // , tipo de dato
                 0,  //, rank a enviar
                 0, //, tag
                 MPI_COMM_WORLD);//, grupo


    }

    MPI_Finalize();

    return 0;
    //para cualquier tamaño del vector y cualquier numero de ranks
//    el rank 0 va desde el ultmo bloque hasta el tamaño de vector
}
