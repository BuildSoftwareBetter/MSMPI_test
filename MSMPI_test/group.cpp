/*
组与通信因子：
	相同通信因子（MPI_Comm）的进程构成一个组（MPI_Group）
	所有的进程都属于MPI_COMM_WORLD通信因子所在的组

进程的秩（rank）：
	
*/


#include "mpi.h"  
#include <stdio.h>  
#include <string.h>
#define SIZE 4 

int main4(int argc, char* argv[])
{
	int numtasks, rank, sendcount, recvcount, source;
	float sendbuf[SIZE][SIZE] = {
	{1.0, 2.0, 3.0, 4.0},
	{5.0, 6.0, 7.0, 8.0},
	{9.0, 10.0, 11.0, 12.0},
	{13.0, 14.0, 15.0, 16.0} };
	float recvbuf[SIZE];
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	if (numtasks == SIZE) {
		source = 1;
		sendcount = SIZE;
		recvcount = SIZE;
		MPI_Scatter(sendbuf, sendcount, MPI_FLOAT, recvbuf, recvcount,
			MPI_FLOAT, source, MPI_COMM_WORLD);
		printf("rank= %d Results: %f %f %f %f\n", rank, recvbuf[0],
			recvbuf[1], recvbuf[2], recvbuf[3]);
	}
	else
		printf("Must specify %d processors. Terminating.\n", SIZE);

	MPI_Finalize();

	return 0;
}