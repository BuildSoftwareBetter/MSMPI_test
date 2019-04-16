/*
���ͨ�����ӣ�
	MPI_COMM_WORLD ��ȫ�ֵ�ͨ������

	MPI_Comm_group (comm,*group) : ��ȡ��ͨ�����ӹ���������
	MPI_Group_rank (group,*rank) �� ���ظý�����ָ�����е���
	MPI_Group_size (group,*size) �� ���ظ���Ľ�����
	MPI_Group_excl (group,n,*ranks,*newgroup) �� ��ָ����������̭һЩ���̺�������
	MPI_Group_incl (group,n,*ranks,*newgroup) ����ָ����������ȡ���̲����µ���
	MPI_Group_intersection (group1,group2,*newgroup) ����ȡ������Ľ������̣������µ���
	MPI_Group_union (group1,group2,*newgroup) ����ȡ������Ĳ������̣������µ���
	MPI_Group_difference (group1,group2,*newgroup) ����ȡ�����������еĲ�ͬ���̣������µ���
	MPI_Group_compare (group1,group2,*result) ���Ƚ������飬����������еĳ�Ա���ȶ�һ���򷵻� MPI_IDENT��ֻ��Աһ������MPI_SIMILAR�����򷵻�MPI_UNEQUAL
	MPI_Group_free (group) ���ͷ�һ���û���������
	MPI_Comm_create (comm,group,*newcomm) �� �������ͨ�����ӣ�����һ���µ�ͨ������
	MPI_Comm_dup (comm,*newcomm) �� ����һ���µ�ͨ������
	MPI_Comm_compare (comm1,comm2,*result) �� �ú����Ƚ�����ͨ�����ӣ� �ҳ���������ͨ������֮��Ĺ�ϵ�� �������ͨ�������е� �����Ĺ�ϵ���鶼һ�����򷵻� MPI_IDENT ����������Ĺ�ϵ��ͬ�����鲻��ͬ�򷵻� MPI_CONGRUENT ����������Ĺ�ϵ����ͬ��������ͬ�򷵻� MPI_SIMILAR��������Ϊ MPI_UNEQUEL
	MPI_Comm_free (*comm) ���ͷ��û�������ͨ������

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
	MPI_Comm_group(MPI_COMM_WORLD, &orig_group);	// ��ȡ��ͨ�����ӹ���������
	
	/* Divide tasks into two distinct groups based upon rank */
	if (rank < NPROCS / 2) {
		MPI_Group_incl(orig_group, NPROCS / 2, ranks1, &new_group);	// ��������ȡ���̣������µ���
	}
	else {
		MPI_Group_incl(orig_group, NPROCS / 2, ranks2, &new_group);
	}
	
	/* Create new new communicator and then perform collective communications */
	MPI_Comm_create(MPI_COMM_WORLD, new_group, &new_comm);	// ����ͨ�����Ӻ��������µ�ͨ������
	
	MPI_Allreduce(&sendbuf, &recvbuf, 1, MPI_INT, MPI_SUM, new_comm);	// �������ݵ�ָ����ͨ������
	
	MPI_Group_rank(new_group, &new_rank);		// ��ȡ��ǰ���������е���
	printf("rank= %d newrank= %d recvbuf= %d\n", rank, new_rank, recvbuf);
	
	MPI_Finalize();
}