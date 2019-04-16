/*
求积分算法
*/

#include "mpi.h"  
#include <stdio.h>  
#include <cmath>

double Trap(double left_endpt, double right_endpt, double trap_count, double base_len);
double f(double x);

int main1(int argc, char* argv[])
{
	int my_rank = 0, comm_sz = 0, n = 1024, local_n = 0;
	double a = 0.0, b = 3.0, h = 0, local_a = 0, local_b = 0;
	double local_int = 0, total_int = 0;
	int source;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);	//获得进程号
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);	//获得进程个数

	h = (b - a) / n;       /*  h is the same for all processes  */
	local_n = n / comm_sz; /*  So is the number of trapezoids */

	local_a = a + my_rank * local_n*h;
	local_b = local_a + local_n * h;
	local_int = Trap(local_a, local_b, local_n, h);

	if (my_rank != 0)
	{
		MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
	else
	{
		total_int = local_int;
		for (source = 1; source < comm_sz; source++)
		{
			MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0,
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			total_int += local_int;
		}
	}

	if (my_rank == 0)
	{
		printf("With n = %d trapezoids, our estimate\n", n);
		printf("of the integral from %f to %f = %.15e\n", a, b, total_int);

	}
	MPI_Finalize();
	return 0;
}
//子区域的积分函数
double Trap(double left_endpt, double right_endpt, double trap_count, double base_len)
{
	double estimate = 0, x = 0;
	int i;

	estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
	for (i = 1; i <= trap_count - 1; i++)
	{
		x = left_endpt + base_len;
		estimate += f(x);
	}
	estimate = estimate * base_len;
	return estimate;
}
//数学函数
double f(double x)
{
	return pow(x, 3);
}