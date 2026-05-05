#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define DEBUG 0
#define M  1000000  // Number of sequences
#define N  200       // Number of bases per sequence

unsigned int g_seed = 0;
int fast_rand(void) {
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16) % 5;
}

// The distance between two bases
int base_distance(int base1, int base2){

    if((base1 == 4) || (base2 == 4)){
        return 3;
    }

    if(base1 == base2) {
        return 0;
    }

    if((base1 == 0) && (base2 == 3)) {
        return 1;
    }

    if((base2 == 0) && (base1 == 3)) {
        return 1;
    }

    if((base1 == 1) && (base2 == 2)) {
        return 1;
    }

    if((base2 == 2) && (base1 == 1)) {
        return 1;
    }

    return 2;
}

int main(int argc, char *argv[]) {
    int rank, size;
    int i, j;
    int *data1 = NULL, *data2 = NULL;
    int *local_data1, *local_data2;
    int *result = NULL, *local_result;
    double t_scatter;
    struct timeval tv_start, tv_end;
    double t_comp, t_comm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int base_rows = M / size;
    int remainder = M % size;

    int local_rows = base_rows + (rank < remainder ? 1 : 0);

    int *sendcounts_data = NULL;
    int *displs_data = NULL;
    int *sendcounts_res = NULL;
    int *displs_res = NULL;

    if (rank == 0) {
        sendcounts_data = (int *) malloc(size * sizeof(int));
        displs_data     = (int *) malloc(size * sizeof(int));
        sendcounts_res  = (int *) malloc(size * sizeof(int));
        displs_res      = (int *) malloc(size * sizeof(int));

        int offset = 0;
        for (int p = 0; p < size; p++) {
            int rows_p = base_rows + (p < remainder ? 1 : 0);
            sendcounts_data[p] = rows_p * N;
            displs_data[p] = offset * N;
            sendcounts_res[p] = rows_p;
            displs_res[p] = offset;
            offset += rows_p;
        }

        data1  = (int *) malloc((long)M * N * sizeof(int));
        data2  = (int *) malloc((long)M * N * sizeof(int));
        result = (int *) malloc(M * sizeof(int));

        for (i = 0; i < M; i++)
            for (j = 0; j < N; j++) {
                data1[i * N + j] = fast_rand();
                data2[i * N + j] = fast_rand();
            }
    }

    local_data1 = (int *) malloc((long)local_rows * N * sizeof(int));
    local_data2 = (int *) malloc((long)local_rows * N * sizeof(int));
    local_result = (int *) malloc(local_rows * sizeof(int));

    MPI_Barrier(MPI_COMM_WORLD);


    gettimeofday(&tv_start, NULL);

    MPI_Scatterv(data1, sendcounts_data, displs_data, MPI_INT,
                 local_data1, local_rows * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(data2, sendcounts_data, displs_data, MPI_INT,
                 local_data2, local_rows * N, MPI_INT, 0, MPI_COMM_WORLD);

    gettimeofday(&tv_end, NULL);

    t_scatter = (tv_end.tv_usec - tv_start.tv_usec) / 1e6
    + (tv_end.tv_sec  - tv_start.tv_sec);

    MPI_Barrier(MPI_COMM_WORLD);


    gettimeofday(&tv_start, NULL);

    for (i = 0; i < local_rows; i++) {
        local_result[i] = 0;
        for (j = 0; j < N; j++) {
            local_result[i] += base_distance(local_data1[i * N + j],
                                             local_data2[i * N + j]);
        }
    }

    gettimeofday(&tv_end, NULL);
    t_comp = (tv_end.tv_usec - tv_start.tv_usec) / 1e6
    + (tv_end.tv_sec  - tv_start.tv_sec);

    gettimeofday(&tv_start, NULL);

    MPI_Gatherv(local_result, local_rows, MPI_INT,
                result, sendcounts_res, displs_res, MPI_INT, 0, MPI_COMM_WORLD);

    gettimeofday(&tv_end, NULL);
    t_comm = (tv_end.tv_usec - tv_start.tv_usec) / 1e6
    + (tv_end.tv_sec  - tv_start.tv_sec)+t_scatter;


    double *all_comp = NULL, *all_comm = NULL;
    if (rank == 0) {
        all_comp = (double *) malloc(size * sizeof(double));
        all_comm = (double *) malloc(size * sizeof(double));
    }
    MPI_Gather(&t_comp, 1, MPI_DOUBLE, all_comp, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gather(&t_comm, 1, MPI_DOUBLE, all_comm, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int p = 0; p < size; p++) {
            printf("Process %d: computation = %.6f s, communication (gather) = %.6f s\n",
                   p, all_comp[p], all_comm[p]);
        }

        if (DEBUG == 1){
            long long checksum = 0;
            for (i = 0; i < M; i++) checksum += result[i];
            printf("Checksum: %lld\n", checksum);
        }
        else if (DEBUG == 2){
            for (i = 0; i < M; i++) printf(" %d \t ", result[i]);
        }

        free(all_comp); free(all_comm);
        free(data1); free(data2); free(result);
        free(sendcounts_data); free(displs_data);
        free(sendcounts_res);  free(displs_res);
    }

    free(local_data1); free(local_data2); free(local_result);

    MPI_Finalize();
    return 0;
}
