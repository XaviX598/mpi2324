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
    int data[100];

    //std::printf("Rank %d od %d procs\n", rank, nprocs);

    if(rank==0){
//        std::printf("total ranks:%d\n", nprocs);

        //llenamos el vector, solo 1 vez entonces en el rank 0
        for(int i =0 ;i<100 ;i++){
            data[i]=i;
        }

        //para enviar datos a ranks i
        for(int rank_id =1 ;rank_id<nprocs ;rank_id++){
            std::printf("\nRank 0 enviando datos a RANK_%d\n", rank_id);

            int start = rank_id*25;

            MPI_Send(&data[start],  //enviamos los datos
                     25, //, cuantos datos
                     MPI_INT, // , tipo de dato
                     rank_id,  //, rank a enviar
                     0, //, tag
                     MPI_COMM_WORLD);//, grupo
        }

        int suma_ranks[4];

        //hace su suma parcial
        suma_ranks[0] = sumar(data, 25);

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

        std::printf("\nsumas parciales: %d, %d, %d, %d\n", suma_ranks[0],suma_ranks[1],suma_ranks[2],suma_ranks[3]);

        int suma_total = sumar(suma_ranks, 4);

        std::printf("suma total: %d \n", suma_total);

    }else {
        std::printf("\nRank_%d recibiendo datos\n", rank);

        MPI_Recv(data, //en donde recibimos,
                 25,//cuantos datos,
                 MPI_INT, //tipo,
                 0, // de donde recibimos,
                 0, //tag,
                 MPI_COMM_WORLD, //grupo,
                 MPI_STATUS_IGNORE);// status

//        std::string str = " ";
//            int suma = 0;
//        for(int i =0;i<25;i++){
//            str =  str + std::to_string( data[i]) + ",";
//            suma = data[i];
//        }

//        std::printf("\nRank_%d recibiendo datos==>%s\n", rank,str.c_str());

//realizamos la suma parcial de cada uno
        int suma_parcial = sumar(data, 25);
//        std::printf("la suma es: %d", suma);

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
