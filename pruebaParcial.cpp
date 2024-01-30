#include <iostream>
#include <mpi.h>
#include <vector>
#include <math.h>
#include <ctime>
#include <algorithm>
#include <fstream>

#define  DIMENSION 101


//realizo dos main ya que me arrojaba un problema por el tamaño del vector, lo hare abajo con datos de prueba para comprobar el funcionamiento, es el mismo funcionamiento solo que con un vector mas pequeño
std::vector<int> read_file() {
    std::fstream fs("datos.txt", std::ios::in );
    std::string line;
    std::vector<int> ret;
    while( std::getline(fs, line) ){
        ret.push_back( std::stoi(line) );
    }
    fs.close();

    return ret;
}

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

//realizo dos main ya que me arrojaba un problema por el tamaño del vector, lo hare abajo con datos de prueba para comprobar el funcionamiento, es el mismo funcionamiento solo que con un vector mas pequeño
//CON LOS DATOS LEIDOS
/*
int main(int argc, char** argv){

    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //para saber el id del rank
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); //para sacar el numero total de procesos

    int block_size;
    int real_size;
    int padding = 0;


    if(rank==0){

        auto file = read_file();
        //     -----------------para comprobar la lectura de los datos
        //        for(int i=0; i<file.size(); i++) {
        //            std::printf("%d ", file[i]);
        //        }
        //        std::printf("\n");

        if(file.size()%nprocs!=0){
            real_size = std::ceil((double)file.size()/nprocs) * nprocs;
            block_size = real_size/nprocs;
            padding = real_size - file.size();
        }else{
            real_size = file.size();
            block_size = real_size/nprocs;
            padding = real_size - file.size();
        }


        std::printf("Dimension: %ld, real_size: %d, block_size: %d, padding: %d\n",
                    file.size(), real_size, block_size, padding);


        MPI_Scatter(file.data(), //bufer envio
                    block_size, //cuantos datos vamos a enviar
                    MPI_INT, //tipo de dato
                    MPI_IN_PLACE, 0 , MPI_INT, //para que el rank 0 no reciba nada, count de recepcion, tipo
                    0, //cual rank es el root
                    MPI_COMM_WORLD  ); //comunicador globa

//        std::printf("rank_%d: ", rank );
//        for(int i=0; i<file.size(); i++){
//            std::printf("%d ", file[i]);
//
//        }

        std::vector<std::tuple<int, int>> resultado0 = contar(file.data(), block_size);

//        std::printf("Contenido del resultado del rank_%d:\n", rank);
//        for (const auto& tupla : resultado0) {
//            int elemento = std::get<0>(tupla);
//            int conteo = std::get<1>(tupla);
//            std::printf("(%d, %d) ", elemento, conteo);
//        }
//        std::printf("\n");

        //recibimos resultado
        MPI_Gather(MPI_IN_PLACE, 0, MPI_INT,
                   file.data(), file.size()*2 ,MPI_INT,
                   0, MPI_COMM_WORLD);

        file.resize(file.size());

        std::vector<std::tuple<int, int>> resultado1 = contar(file.data(), block_size);

        std::printf("Contenido de todos\n");
        for (const auto& tupla : resultado0) {
            int elemento = std::get<0>(tupla);
            int conteo = std::get<1>(tupla);
            std::printf("(%d, %d) ", elemento, conteo);
        }
        std::printf("\n");



    }else {

        std::vector<int> file_local(block_size);

        MPI_Scatter(nullptr,0,MPI_DOUBLE, //datos de envio, aqui va esto ya que estos no van a enviar
                    file_local.data(), block_size, MPI_INT,
                    0, MPI_COMM_WORLD);
//        std::printf("rank_%d: ", rank );
//        for(int i=0; i<file_local.size(); i++){
//            std::printf("%d ", file_local[i]);
//
//        }
//        std::printf("\n");

        std::vector<std::tuple<int, int>> resultado = contar(file_local.data(), block_size);

        // Imprimir el contenido del resultado con std::printf
//        std::printf("Contenido del resultado del rank_%d:\n", rank);
//        for (const auto& tupla : resultado) {
//            int elemento = std::get<0>(tupla);
//            int conteo = std::get<1>(tupla);
//            std::printf("(%d, %d) ", elemento, conteo);
//        }
//        std::printf("\n");

        //enviamos resultado
        MPI_Gather(resultado.data(), resultado.size()*2, MPI_INT, //envio
                   nullptr, 0 ,MPI_INT, //RECIBIMOS NADA
                   0, MPI_COMM_WORLD);

    }
    MPI_Finalize();

    return 0;

}
*/


//con Datos de prueba, son randomicos
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

//        std::printf("Dimension: %d, real_size: %d, block_size: %d, padding: %d\n",
//                    DIMENSION, real_size, block_size, padding);


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

