#include <stdio.h>
#include <omp.h>

#define UPTO 10000000

long int count,      /* number of primes */
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


void openmp_primes(long int n) {
	long int i, num, divisor, quotient, remainder;

	if (n < 2) return;
	count = 1;                         /* 2 is the first prime */
	lastprime = 2;

	/*
	 * Parallelize the serial algorithm but you are NOT allowed to change it!
	 * Don't add/remove/change global variables
	 */
  #pragma omp parallel for schedule(guided) private(num, divisor, quotient, remainder) num_threads(4) reduction(+:count) reduction(max:lastprime)
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


int main()
{
  double serial_start_time, serial_end_time, serial_final_time,
         parallel_start_time, parallel_end_time, parallel_final_time;

	printf("Serial and parallel prime number calculations:\n\n");

	/* Time the following to compare performance
	 */
  serial_start_time = omp_get_wtime();
	serial_primes(UPTO);        /* time it */
  serial_end_time = omp_get_wtime();
  serial_final_time = serial_end_time - serial_start_time;
	printf("[serial] count = %ld, last = %ld (time = %lf seconds)\n", count, lastprime, serial_final_time);


  parallel_start_time = omp_get_wtime();
	openmp_primes(UPTO);        /* time it */
  parallel_end_time = omp_get_wtime();
  parallel_final_time = parallel_end_time - parallel_start_time;
	printf("[openmp] count = %ld, last = %ld (time = %lf seconds)\n", count, lastprime, parallel_final_time);

	return 0;
}
