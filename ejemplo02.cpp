#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //para declarar el vectorsin inicializar
    int data[100];

    //std::printf("Rank %d od %d procs\n", rank, nprocs);

    if(rank==0){
        std::printf("total ranks:%d\n", nprocs);
        //llenamos el vector, solo 1 vez entonces en el rank 0
        for(int i =0 ;i<100 ;i++){
            data[i]=i;
        }

        //para enviar datos a ranks i
        for(int i =1 ;i<nprocs ;i++){
            std::printf("Rank 0 enviando datos a RANK_%d\n", i);
            MPI_Send(data,  //enviamos los datos
                     100, //, cuantos datos
                     MPI_INT, // , tipo de dato
                     i,  //, rank a enviar
                     0, //, tag
                     MPI_COMM_WORLD);//, grupo
        }


    }else {
        std::printf("\nRank_%d recibiendo datos\n", rank);

        MPI_Recv(data, //en donde recibimos,
                 100,//cuantos datos,
                 MPI_INT, //tipo,
                 0, // de donde recibimos,
                 0, //tag,
                 MPI_COMM_WORLD, //grupo,
                 MPI_STATUS_IGNORE);// status

        std::string str = " ";

        for(int i =0;i<10;i++){
            str =  str + std::to_string( data[i]) + ",";
        }

        std::printf("Rank_%d recibiendo datos==>%s\n", rank,str.c_str());
    }

    MPI_Finalize();
    return 0;
}
