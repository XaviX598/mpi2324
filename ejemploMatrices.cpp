#include <iostream>
#include <mpi.h>
#include <vector>
#include <math.h>
#define MATRIX_DIMENSION 25

void multiplicarMatriz(double* A, double* b, double* c, int rows, int cols){
    for(int i = 0; i<rows; i++){
        double tmp = 0;
        for(int j = 0; j<cols; j++){
            tmp =  tmp + A[i*cols+j] * b[j];
        }
        c[i] = tmp;
    }
}

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //id de ranks
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); //numero de ranks

    int rows_per_ranks; //cuanats filas necesitamos
    int rows_alloc = MATRIX_DIMENSION;

    int padding = 0; //para saber los valores ue vamos a rellenar

    if(MATRIX_DIMENSION%nprocs!=0){
        rows_alloc = std::ceil((double)MATRIX_DIMENSION/nprocs) * nprocs; //25/4 = 7 * 4 = 28
        padding = rows_alloc - MATRIX_DIMENSION; //28 - 25 = 3
    }

    //en este punto siempre la dimension va a ser exacta
    rows_per_ranks = rows_alloc / nprocs; //sabemos las filas que se van a cada rank

    if(rank == 0){
        //en el rank 0 vamos a inicializar la matriz, no va fuera ya que todos los ranks tendrian la matriz completa
        //imprimimos informacion
        std::printf("Dimension: %d, rows_alloc: %d, rows_per_rank: %d, padding: %d\n",
                    MATRIX_DIMENSION, rows_alloc, rows_per_ranks, padding);
        //no es 25 x 25 ya que debemos aumentar el padding, entonces quedaria 28 filas y 25 columnas
        std::vector<double> A(MATRIX_DIMENSION * rows_alloc);
        //el vector b es el numero de columnas del vector A o o sea 25
        std::vector<double> b(MATRIX_DIMENSION);
        //el vector c resultandote es 28 ya que es cada fila por b al ser 28 filas pues 28
        std::vector<double> c(rows_alloc);

        //rellenamos los vectores A y B
        for(int i = 0; i<MATRIX_DIMENSION;i++){
            for(int j = 0;j<MATRIX_DIMENSION; j++){
                int index = i*MATRIX_DIMENSION+j;
                A[index] = i; // es de 25 por 25 ya que no nos importa lo del relleno
            }

        }

        for(int i = 0; i<MATRIX_DIMENSION;i++) b[i] = 1;

        //enviamos la matriz A, esto mandando bloques a cada rank, esto con el scatter, este madaba bloques por igual
        //se va a mandar 7 filas de 25 elementos , entonces mandamos una mattriz de 7*25
        MPI_Scatter(A.data(), //bufer envio
                    MATRIX_DIMENSION*rows_per_ranks, //cuantos datos vamos a enviar
                    MPI_DOUBLE, //tipo de dato
                    MPI_IN_PLACE, 0 , MPI_DOUBLE, //para que el rank 0 no reciba nada, count de recepcion, tipo
                    0, //cual rank es el root
                    MPI_COMM_WORLD  ); //comunicador globa
        //enviamos la matriz B completa
        MPI_Bcast(b.data(), MATRIX_DIMENSION, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        //realizamos el calculo A x b = C
        multiplicarMatriz(A.data(), b.data(), c.data(), 7, MATRIX_DIMENSION);

        //RECIBIMOS LOS RESULTADOS
        MPI_Gather(MPI_IN_PLACE, 0, MPI_DOUBLE, //envio PONEMOS in plaace para indicar que el tiene su propio datos
                   c.data(), rows_per_ranks ,MPI_DOUBLE, //RECIBIMOS
                   0, MPI_COMM_WORLD);

        c.resize(MATRIX_DIMENSION);

        //imprimimos el resultado
        std::printf("resultado: \n");
        for(int i=0; i<MATRIX_DIMENSION; i++){
            std::printf("%.0f, ", c[i]);
        }

    }else{
        //aqui vamos a recibir
        std::vector<double> A_local(MATRIX_DIMENSION*rows_per_ranks);
        //el vector b va a ser la misma dimension
        std::vector<double> b_local(MATRIX_DIMENSION);
        //vector c que vamos a mandar
        std::vector<double> c_local(rows_per_ranks);

        MPI_Scatter(nullptr,0,MPI_DOUBLE, //datos de envio, aqui va esto ya que estos no van a enviar
                    A_local.data(), MATRIX_DIMENSION*rows_per_ranks, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);

        std::printf("RANK_%d: [%.0f..%.0f]\n", rank, A_local[0], A_local.back());

        //RECIBIMOS MATRIZ b
        MPI_Bcast(b_local.data(), MATRIX_DIMENSION, MPI_DOUBLE, 0, MPI_COMM_WORLD);


        //ultimo rank ya que aqui esta el relleno
        int rows_per_rank_tmp = rows_per_ranks;
        if(rank==nprocs-1){
            rows_per_rank_tmp = MATRIX_DIMENSION - (rank*rows_per_ranks); //3 * 7 = 21 y 25 -21 = 4
            //tambien funciona
//            rows_per_rank_tmp = rows_per_ranks - padding;
        }

        //realizamos el calculo A x b = C
        multiplicarMatriz(A_local.data(), b_local.data(), c_local.data(), rows_per_rank_tmp, MATRIX_DIMENSION);

        //enviamos resultado
        MPI_Gather(c_local.data(), rows_per_ranks, MPI_DOUBLE, //envio
                    nullptr, 0 ,MPI_DOUBLE, //RECIBIMOS NADA
                    0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;

}
