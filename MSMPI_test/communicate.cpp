/*

*/

#include "mpi.h"  
#include <stdio.h>  
#include <string.h>

int main3(int argc, char* argv[])
{
	int my_rank = 0, comm_sz = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);	//获得进程号
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);	//获得进程个数

	if (my_rank == 0)
	{

		const char* str = "hello world";// { 0 };

			//scanf("%s", str);

		for (int target = 1; target < comm_sz; target++)
		{
			MPI_Send(str, strlen(str), MPI_CHAR, target, 0, MPI_COMM_WORLD);
		}
	}
	else
	{
		//int rank, numproces;
		int namelen;
		char processor_name[MPI_MAX_PROCESSOR_NAME];
		//MPI_Comm_rank(MPI_COMM_WORLD, &rank);//获得进程号
		//MPI_Comm_size(MPI_COMM_WORLD, &numproces);//返回通信子的进程数

		MPI_Get_processor_name(processor_name, &namelen);


		char str[100] = { 0 };
		MPI_Recv(str, 50, MPI_CHAR, MPI_ANY_SOURCE, 0,
			MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//fprintf(stderr, "hello world! process %d of %d on %s\n", rank, numproces, processor_name);
		fprintf(stdout, "process %d of %d on %s,recv data : %s\n", my_rank, comm_sz, processor_name, str);
	}

	//if (my_rank == 0)
	//{
	//	printf("With n = %d trapezoids, our estimate\n", n);
	//	printf("of the integral from %f to %f = %.15e\n", a, b, total_int);

	//}
	MPI_Finalize();
	return 0;
}