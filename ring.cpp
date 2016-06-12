//
// Created by Vanessa Ulloa on 6/12/16.
//

#include <iostream>
#include "mpi.h"

using namespace std;


int main(int argc, char** argv) {

    /* MPI START    */

    MPI_Init(&argc,&argv);

    int world_rank, world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    MPI_Get_processor_name(processor_name, &name_len);

    int token;

    if(world_rank != 0) {

        MPI_Recv(&token, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        cout << "Process " << world_rank << " received token " << token << " from process " << (world_rank - 1) << ", processor name: " << processor_name << endl;

    } else {

        token = -1;

    }

    MPI_Send(&token, 1, MPI_INT, (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {

        MPI_Recv(&token, 1, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        cout << "Process " << world_rank << " received token " << token << " from process " << (world_rank - 1) << ", processor name: " << processor_name << endl;

    }

    MPI_Finalize();

    /*  MPI END */

    return 0;
}