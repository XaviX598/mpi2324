#include <iostream>
#include <mpi.h>
#include <vector>
#include <math.h>
#include <ctime>
#include <algorithm>

#define  DIMENSION 101


std::vector<std::tuple<int, int>> contar(int* tmp, int block_size) {
    std::vector<std::tuple<int, int>> resultado;

    for (int i = 0; i < block_size; i++) {
        if (tmp[i] != -1) { // Verificar si el elemento ya ha sido contado
            int conteo = 1; // Inicializar el conteo en 1 para el elemento actual

            // Recorrer el resto del vector para contar ocurrencias
            for (int j = i + 1; j < block_size; j++) {
                if (tmp[j] == tmp[i]) {
                    conteo++;
                    tmp[j] = -1; // Marcar el elemento como contado para evitar duplicados
                }
            }

            // Agregar la tupla (elemento, conteo) al resultado
            resultado.push_back(std::make_tuple(tmp[i], conteo));
        }
    }
    return resultado;
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

//        std::printf("Dimension: %d, real_size: %d, block_size: %d, padding: %d\n",
//                    DIMENSION, real_size, block_size, padding);

        //-------------------------creacion vector con for----------------------------------------
//        std::vector<double> datos(DIMENSION); //definimos el vector
//        for(int i = 0; i<DIMENSION;i++){
//            datos[i] = i+1; //rellenamos el vector
//        }
//        //para comprobar que si se lleno el vector
//        std::printf("El vector es: \n");
//        for(int i=0; i<DIMENSION; i++){
//            std::printf("%.0f, ", datos[i]);
//        }
//        std::printf("\n");

        //------------------------creacion de vector con nuestros datos-----------------------------
//        std::vector<int> datosCreados= {1, 2, 3, 4, 5, 6, 6};
//        //para comprobar que si se lleno el vector
//        std::printf("El vector es: \n");
//        for(int i=0; i<datosCreados.size(); i++){
//            std::printf("%d ", datosCreados[i]);
//        }
//        std::printf("\n");

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

        std::vector<std::tuple<int, int>> resultado0 = contar(allData.data(), block_size);

        // Imprimir el contenido del resultado con std::printf
        std::printf("Contenido del resultado del rank_%d:\n", rank);
        for (const auto& tupla : resultado0) {
            int elemento = std::get<0>(tupla);
            int conteo = std::get<1>(tupla);
            std::printf("(%d, %d) ", elemento, conteo);
        }
        std::printf("\n");

        //recibimos resultado
        MPI_Gather(MPI_IN_PLACE, 0, MPI_INT, //envio PONEMOS in plaace para indicar que el tiene su propio datos
                   allData.data(), allData.size()*2 ,MPI_INT, //RECIBIMOS
                   0, MPI_COMM_WORLD);

        allData.resize(DIMENSION);

        std::vector<std::tuple<int, int>> resultado1 = contar(allData.data(), block_size);
        // Imprimir el contenido del resultado con std::printf
        std::printf("Contenido de todos\n");
        for (const auto& tupla : resultado0) {
            int elemento = std::get<0>(tupla);
            int conteo = std::get<1>(tupla);
            std::printf("(%d, %d) ", elemento, conteo);
        }
        std::printf("\n");



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

        std::vector<std::tuple<int, int>> resultado = contar(allData_local.data(), block_size);

        // Imprimir el contenido del resultado con std::printf
        std::printf("Contenido del resultado del rank_%d:\n", rank);
        for (const auto& tupla : resultado) {
            int elemento = std::get<0>(tupla);
            int conteo = std::get<1>(tupla);
            std::printf("(%d, %d) ", elemento, conteo);
        }
        std::printf("\n");

        //enviamos resultado
        MPI_Gather(resultado.data(), resultado.size()*2, MPI_INT, //envio
                   nullptr, 0 ,MPI_INT, //RECIBIMOS NADA
                   0, MPI_COMM_WORLD);



    }


    MPI_Finalize();

    return 0;

}