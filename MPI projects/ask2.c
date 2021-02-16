/* Serial program for matrix-matrix product.
 *
 * VVD
 */


/*
 * Spyridonos Vasileios
 * AM: 2820
 * Username: cse42820
 * E-mail: cs02820@uoi.gr
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1024
int A[N][N], B[N][N], C[N][N];
int readmat(char *fname, int *mat, int n),
    writemat(char *fname, int *mat, int n);

int main(int argc, char * arvg[])
{
	int i, j, k, z, sum, myid, nproc;
	int array[3];
    double start_time, end_time, communications_start_time, communications_end_time, proc_communications_time,
           proc_computations_time, total_communications_time, total_computations_time;

	/* Read matrices from files: "A_file", "B_file"
	 */
	if (readmat("Amat1024", (int *) A, N) < 0)
		exit( 1 + printf("file problem\n") );
	if (readmat("Bmat1024", (int *) B, N) < 0)
		exit( 1 + printf("file problem\n") );

	MPI_Init(&argc, &arvg);

	MPI_Status status;



	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	start_time = MPI_Wtime();

	for (i = myid * N / nproc; i < (myid + 1) * N / nproc; i++) {
		for (j = 0; j < N; j++) {
			for (k = sum = 0; k < N; k++) {
				sum += A[i][k]*B[k][j];
      }

			C[i][j] = sum;


			communications_start_time = MPI_Wtime();

			if (myid == 0) {
				for (z = 0; z < nproc - 1; z++) {
					MPI_Recv(array, 3, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
					C[array[0]][array[1]] = array[2];
				}
			}

			else {
				array[0] = i;
				array[1] = j;
				array[2] = sum;
				MPI_Send(array, 3, MPI_INT, 0, 1, MPI_COMM_WORLD);
			}

			communications_end_time = MPI_Wtime();
			proc_communications_time += communications_end_time - communications_start_time;

		};
	}


	end_time = MPI_Wtime();

	proc_computations_time = end_time - start_time - proc_communications_time;


	MPI_Reduce(&proc_computations_time, &total_computations_time, 1, MPI_DOUBLE, MPI_SUM, 0 , MPI_COMM_WORLD);
	MPI_Reduce(&proc_communications_time, &total_communications_time, 1, MPI_DOUBLE, MPI_SUM, 0 , MPI_COMM_WORLD);

	if (myid == 0){
        printf("\nTo programma ektelestike se: %lf deuterolepta\n", end_time - start_time);
        printf("\nSynolikos xronos pou spatalisan oi diergasies gia ypologismous: %lf\n", total_computations_time);
	    printf("Synolikos xronos pou spatalisan oi diergasies gia epikoinwnies: %lf\n", total_communications_time);
	}

	/* Save result in "Cmat1024"
	 */
	if (myid == 0)
		writemat("Cmat1024", (int *) C, N);

	MPI_Finalize();

	return (0);
}


/* Utilities to read & write matrices from/to files
 * VVD
 */

#define _mat(i,j) (mat[(i)*n + (j)])


int readmat(char *fname, int *mat, int n)
{
	FILE *fp;
	int  i, j;

	if ((fp = fopen(fname, "r")) == NULL)
		return (-1);
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (fscanf(fp, "%d", &_mat(i,j)) == EOF)
			{
				fclose(fp);
				return (-1);
			};
	fclose(fp);
	return (0);
}


int writemat(char *fname, int *mat, int n)
{
	FILE *fp;
	int  i, j;

	if ((fp = fopen(fname, "w")) == NULL)
		return (-1);
	for (i = 0; i < n; i++, fprintf(fp, "\n"))
		for (j = 0; j < n; j++)
			fprintf(fp, " %d", _mat(i, j));
	fclose(fp);
	return (0);
}
