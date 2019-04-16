/*
计算Π的值
*/

#include <stdio.h>
#include "mpi.h"
#include <iostream>
using namespace std;

#define N 1000000

void pi()
{
	double local, pi = 0.0, w;
	int n = N;
	long i;
	w = 1.0 / N;
	for (i = 0; i < n; i++) {

		local = (i + 0.5) * w;

		pi = pi + 4.0 / (1.0 + local * local);

	}
	printf("pi is %f\n", pi*w);
}

double ff(double a)
{
	return (4.0 / (1.0 + a * a));
}

int main_pi(int argc, char *argv[])
{
	/*pi();
	system("pause");*/


	int done = 0, n, myid, numprocs, i;
	double PI25DT = 3.141592653589793238462643;
	double mypi, pi, h, sum, x;
	double startwtime, endwtime;
	int namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Get_processor_name(processor_name, &namelen);

	//fprintf(stderr, "Process %d on %s\n", myid, processor_name);

	n = 0;
	while (!done) {

		if (myid == 0) {
			//printf("Enter the number of intervals: (0 quits) ");
			cout << ("Enter the number of intervals: (0 quits) ") << endl;
			scanf("%d", &n);
			startwtime = MPI_Wtime();
		}

		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);	// 广播数据到所有其他进程

		if (n == 0)
			done = 1;
		else
		{

			h = 1.0 / (double)n;
			sum = 0.0;

			// 每个进程各分配部分 i 值进行计算
			for (i = myid + 1; i <= n; i += numprocs) {
				x = h * ((double)i - 0.5);
				sum += ff(x);
			}

			mypi = h * sum;

			MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);	// 执行规约操作，并把数据发送至指定进程

			if (myid == 0) {

				printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));

				endwtime = MPI_Wtime();

				printf("wall clock time = %f\n", endwtime - startwtime);
			}
		}
	}

	MPI_Finalize();

	return 0;
}