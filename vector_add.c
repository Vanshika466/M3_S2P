#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void fill_vector(int *vec, int size) {
    for (int i = 0; i < size; i++) {
        vec[i] = rand() % 100;  // Values between 0-99
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int total_size = 1000000;  // 1 million elements
    int *v1 = NULL, *v2 = NULL, *v3 = NULL;
    int local_size = total_size / size;
    int *local_v1 = (int *)malloc(local_size * sizeof(int));
    int *local_v2 = (int *)malloc(local_size * sizeof(int));
    int *local_v3 = (int *)malloc(local_size * sizeof(int));

    // Root process initializes full vectors
    if (rank == 0) {
        v1 = (int *)malloc(total_size * sizeof(int));
        v2 = (int *)malloc(total_size * sizeof(int));
        v3 = (int *)malloc(total_size * sizeof(int));
        srand(time(NULL));  // Seed randomness
        fill_vector(v1, total_size);
        fill_vector(v2, total_size);
    }

    // Scatter vectors to all processes
    MPI_Scatter(v1, local_size, MPI_INT, local_v1, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(v2, local_size, MPI_INT, local_v2, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Local vector addition
    for (int i = 0; i < local_size; i++) {
        local_v3[i] = local_v1[i] + local_v2[i];
    }

    // Gather results back to root
    MPI_Gather(local_v3, local_size, MPI_INT, v3, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate local sum for reduction
    int local_sum = 0;
    for (int i = 0; i < local_size; i++) {
        local_sum += local_v3[i];
    }

    // Global sum reduction
    int global_sum;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Root prints the result
    if (rank == 0) {
        printf("Total sum of v3: %d\n", global_sum);
        free(v1); free(v2); free(v3);
    }

    free(local_v1); free(local_v2); free(local_v3);
    MPI_Finalize();
    return 0;
}
