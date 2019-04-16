#include "mpi.h" 
#include <stdio.h> 
#define NPROCS 8 
int main5(int argc, char *argv[])
{
	int rank, new_rank, sendbuf, recvbuf, numtasks,
		ranks1[4] = { 0,1,2,3 }, ranks2[4] = { 4,5,6,7 };

	MPI_Group orig_group, new_group;
	MPI_Comm new_comm;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);


	if (numtasks != NPROCS) {
		printf("Must specify MP_PROCS= %d. Terminating.\n", NPROCS);
		MPI_Finalize();
		return 0;
	}

	sendbuf = rank;
	
	/* 获取原始组句柄 Extract the original group handle */
	MPI_Comm_group(MPI_COMM_WORLD, &orig_group);

	/* 分成两组 Divide tasks into two distinct groups based upon rank */
	if (rank < NPROCS / 2) {
		// 从指定的组中提取部分进程，产生新的组
		MPI_Group_incl(orig_group, NPROCS / 2, ranks1, &new_group);
	}
	else {

		MPI_Group_incl(orig_group, NPROCS / 2, ranks2, &new_group);
	}
	/* 创建新的通信因子 Create new new communicator and then perform collective communications */
	MPI_Comm_create(MPI_COMM_WORLD, new_group, &new_comm);

	// 执行一个规约操作，并把结果广播到组内的所有进程中
	MPI_Allreduce(&sendbuf, &recvbuf, 1, MPI_INT, MPI_SUM, new_comm);

	// 返回该进程在指定组中的秩
	MPI_Group_rank(new_group, &new_rank);
	printf("rank= %d newrank= %d recvbuf= %d\n", rank, new_rank, recvbuf);
	MPI_Finalize();
}