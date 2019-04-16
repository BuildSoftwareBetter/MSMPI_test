/*
组和通信因子：
	MPI_COMM_WORLD 是全局的通信因子

	MPI_Comm_group (comm,*group) : 获取与通信因子关联的组句柄
	MPI_Group_rank (group,*rank) ： 返回该进程在指定组中的秩
	MPI_Group_size (group,*size) ： 返回该组的进程数
	MPI_Group_excl (group,n,*ranks,*newgroup) ： 从指定的组中淘汰一些进程后建立新组
	MPI_Group_incl (group,n,*ranks,*newgroup) ：从指定的组中提取进程产生新的组
	MPI_Group_intersection (group1,group2,*newgroup) ：获取两个组的交集进程，生成新的组
	MPI_Group_union (group1,group2,*newgroup) ：获取两个组的并集进程，生成新的组
	MPI_Group_difference (group1,group2,*newgroup) ：获取两个进程组中的不同进程，产生新的组
	MPI_Group_compare (group1,group2,*result) ：比较两个组，如果两个组中的成员和秩都一样则返回 MPI_IDENT，只成员一样返回MPI_SIMILAR，否则返回MPI_UNEQUAL
	MPI_Group_free (group) ：释放一个用户创建的组
	MPI_Comm_create (comm,group,*newcomm) ： 根据组和通信因子，创建一个新的通信因子
	MPI_Comm_dup (comm,*newcomm) ： 复制一个新的通信因子
	MPI_Comm_compare (comm1,comm2,*result) ： 该函数比较两个通信因子， 找出两个组内通信因子之间的关系。 如果两个通信因子中的 上下文关系和组都一样，则返回 MPI_IDENT ，如果上下文关系相同，而组不相同则返回 MPI_CONGRUENT ，如果上下文关系不相同，而组相同则返回 MPI_SIMILAR。否则结果为 MPI_UNEQUEL
	MPI_Comm_free (*comm) ：释放用户创建的通信因子

*/

#include <iostream>
#include "mpi.h"
#include <Windows.h>

using namespace std;
#define NPROCS 8 

int main_comm(int argc, char *argv[])
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
		exit(0);
	}

	sendbuf = rank;
	
	/* Extract the original group handle */
	MPI_Comm_group(MPI_COMM_WORLD, &orig_group);	// 获取与通信因子关联的组句柄
	
	/* Divide tasks into two distinct groups based upon rank */
	if (rank < NPROCS / 2) {
		MPI_Group_incl(orig_group, NPROCS / 2, ranks1, &new_group);	// 从组中提取进程，产生新的组
	}
	else {
		MPI_Group_incl(orig_group, NPROCS / 2, ranks2, &new_group);
	}
	
	/* Create new new communicator and then perform collective communications */
	MPI_Comm_create(MPI_COMM_WORLD, new_group, &new_comm);	// 根据通信因子和组生成新的通信因子
	
	MPI_Allreduce(&sendbuf, &recvbuf, 1, MPI_INT, MPI_SUM, new_comm);	// 发送数据到指定的通信因子
	
	MPI_Group_rank(new_group, &new_rank);		// 获取当前进程在组中的秩
	printf("rank= %d newrank= %d recvbuf= %d\n", rank, new_rank, recvbuf);
	
	MPI_Finalize();
}