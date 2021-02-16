/*
 * Spyridonos Vasileios
 * AM: 2820
 * Username: cse42820
 * E-mail: cs02820@uoi.gr
 */

#include <stdio.h>
#include <mpi.h>

#define UPTO 10000000

int count,      /* number of primes */
	  lastprime;  /* the last prime found */


void serial_primes(long int n) {
	long int i, num, divisor, quotient, remainder;

	if (n < 2) return;
	count = 1;                         /* 2 is the first prime */
	lastprime = 2;

	for (i = 0; i < (n-1)/2; ++i) {    /* For every odd number */
		num = 2*i + 3;

		divisor = 1;
		do
		{
			divisor += 2;                  /* Divide by the next odd */
			quotient  = num / divisor;
			remainder = num % divisor;
		} while (remainder && divisor <= quotient);  /* Don't go past sqrt */

		if (remainder || divisor == num) /* num is prime */
		{
			count++;
			lastprime = num;
		}
	}
}


void mpi_primes(long int n) {

	int i, myid, nproc, proc_lastprime = 0, proc_count = 0;
	long int num, divisor, quotient, remainder;
	double start_time , end_time, communications_start_time, communications_end_time, proc_communications_time,
           proc_computations_time, total_communications_time, total_computations_time;
	MPI_Status status;

	start_time = MPI_Wtime();

	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    if (n < 2) return;
	count = 1;                         /* 2 is the first prime */
	lastprime = 2;

		/*
		* Parallelize the serial algorithm but you are NOT allowed to change it!
		* Don't add/remove/change global variables
		*/

	for (i = myid; i < (n - 1) / 2; i += nproc) {    /* For every odd number */

		num = 2*i + 3;
		divisor = 1;

		do
		{
			divisor += 2;                  /* Divide by the next odd */
			quotient  = num / divisor;
			remainder = num % divisor;
		} while (remainder && divisor <= quotient);  /* Don't go past sqrt */

		if (remainder || divisor == num) /* num is prime */
		{
			proc_count++;
			proc_lastprime = num;
		}
	}

	communications_start_time = MPI_Wtime();

    MPI_Reduce(&proc_count, &count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&proc_lastprime, &lastprime, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

	communications_end_time = MPI_Wtime();

	count += 1;

	end_time = MPI_Wtime();

	proc_computations_time = end_time - start_time - (communications_end_time - communications_start_time);
	proc_communications_time = communications_end_time - communications_start_time;

    MPI_Reduce(&proc_computations_time, &total_computations_time, 1, MPI_DOUBLE, MPI_SUM, 0 , MPI_COMM_WORLD);
	MPI_Reduce(&proc_communications_time, &total_communications_time, 1, MPI_DOUBLE, MPI_SUM, 0 , MPI_COMM_WORLD);



	if (myid == 0) {
		printf("[mpi] count = %d, last = %d (time = %lf)\n", count, lastprime, end_time - start_time);
		printf("\nSynolikos xronos pou spatalisan oi diergasies gia ypologismous: %lf\n", total_computations_time);
	    printf("Synolikos xronos pou spatalisan oi diergasies gia epikoinwnies: %lf\n", total_communications_time);
	}



}


int main(int argc, char *argv[])
{
	double start_time , end_time;

	printf("Serial and parallel prime number calculations:\n\n");

	/* Time the following to compare performance
	 */

    MPI_Init(&argc, &argv);

	start_time = MPI_Wtime();
	serial_primes(UPTO);        /* time it */
	end_time = MPI_Wtime();
	printf("[serial] count = %d, last = %d (time = %lf)\n", count, lastprime, end_time - start_time);

	mpi_primes(UPTO);        /* time it */

    MPI_Finalize();

	return 0;

}
