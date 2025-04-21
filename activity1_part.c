#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    // Initialize MPI properly with argc/argv
    MPI_Init(&argc, &argv);
   
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get processor name
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print diagnostic info
    printf("[Rank %d/%d] Running on %s\n",
           world_rank, world_size, processor_name);
    fflush(stdout);

    // Simple message passing
    if (world_rank == 0) {
        char *msg = "Hello from master";
        for (int i = 1; i < world_size; i++) {
            MPI_Send(msg, 17, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    } else {
        char msg[17];
        MPI_Recv(msg, 17, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Rank %d received: %s\n", world_rank, msg);
    }

    MPI_Finalize();
    return 0;
}
