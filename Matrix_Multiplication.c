#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[]) {

    int rank, numOfProcesses;
    int row1, col1, row2, col2, *matrix1, *matrix2, *subMatrix1, *subResult, *result;
    int i, j, k, portion, rem, root = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);

    if (rank == 0) {
        printf("Please enter the dimensions of the first matrix :\n");
        scanf("%d%d", &row1, &col1);
        matrix1 = malloc(row1 * col1 * sizeof(int));
        printf("Please enter its elements: \n");
        for (i = 0; i < row1; ++i)
            for (j = 0; j < col1; ++j)
                scanf("%d", &matrix1[i * col1 + j]);
        printf("Please enter the dimensions of the second matrix :\n");
        scanf("%d%d", &row2, &col2);
        matrix2 = malloc(row2 * col2 * sizeof(int));
        printf("Please enter its elements: \n");
        for (i = 0; i < row2; ++i)
            for (j = 0; j < col2; ++j)
                scanf("%d", &matrix2[i * col2 + j]);
        if (col1 != row2) {
            printf("multiplication not valid \n");
            return 0;
        }
        portion = row1 / numOfProcesses;
        rem = row1 % numOfProcesses;
    }

    MPI_Bcast(&portion, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Bcast(&col1, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Bcast(&row2, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Bcast(&col2, 1, MPI_INT, root, MPI_COMM_WORLD);
    if (rank != 0)
        matrix2 = malloc(row2 * col2 * sizeof(int));
    MPI_Bcast(matrix2, row2 * col2, MPI_INT, root, MPI_COMM_WORLD);

    subMatrix1 = malloc(portion * col1 * sizeof(int));
    MPI_Scatter(matrix1, portion * col1, MPI_INT, subMatrix1, portion * col1, MPI_INT, root, MPI_COMM_WORLD);

    subResult = malloc(portion * col2 * sizeof(int));

    for (i = 0; i < portion; ++i) {
        for (j = 0; j < col2; ++j) {
            subResult[i * col2 + j] = 0;
            for (k = 0; k < col1; ++k) {
                subResult[i * col2 + j] += subMatrix1[i * col1 + k] * matrix2[k * col2 + j];
            }
        }
    }

    if (rank == 0) {
        result = malloc(row1 * col2 * sizeof(int));
        // remainder
        for (i = row1 - rem; i < row1; ++i) {
            for (j = 0; j < col2; ++j) {
                result[i * col2 + j] = 0;
                for (k = 0; k < col1; ++k) {
                    result[i * col2 + j] += matrix1[i * col1 + k] * matrix2[k * col2 + j];
                }
            }
        }
    }

    MPI_Gather(subResult, portion * col2, MPI_INT, result, portion * col2, MPI_INT, root, MPI_COMM_WORLD);

    if (rank == 0) {
        for (i = 0; i < row1; i++) {
            for (j = 0; j < col2; j++) {
                printf("%d ", result[(i * col2) + j]);
            }
            printf("\n");
        }

    }
    MPI_Finalize();
    return 0;
}
