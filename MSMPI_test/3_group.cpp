/*
����ͨ�ţ�
	���ϲ������������ͣ�
		1. ͬ�����ȴ����н��̶�����
		2. ���ݴ��ݣ��㲥����ɢ���ռ���ȫ����ȫ��
		3. ��Լ�������е�����һ�������ռ����н��̵����ݲ����㣨����MapReduce��
	���ϲ�����������
	ֻ֧��MPI�ı�׼�������ͣ���֧����������

	������
		MPI_Barrier
			MPI_Barrier (comm)
			ͬ����դ�����н��̶������ż���ִ��

		MPI_Bcast
			MPI_Bcast(*buffer,count,datatype,root,comm)
			��ָ����һ���������а����ݹ㲥���͸����е�������������

			��ǰrank==rootʱ����������
			��ǰrank!=rootʱ����root�н�������

		 MPI_Scatter
			MPI_Scatter (*sendbuf,sendcnt,sendtype,*recvbuf, recvcnt,recvtype,root,comm)
			��ָ���ĸ������е����ݷ�ɢ���͸����е����н��̣������Լ���

		MPI_Gather
			MPI_Gather (*sendbuf,sendcnt,sendtype,*recvbuf, recvcount,recvtype,root,comm)
			������ָ��һ�������ռ��������н��̷���������Ϣ��������������� MPI_Scatter ���������෴

		MPI_Allgather
			MPI_Allgather (*sendbuf,sendcount,sendtype,*recvbuf, recvcount,recvtype,comm)
			�����е�ÿ�����̶��ռ��������н��̷���������Ϣ

		MPI_Reduce
			MPI_Reduce (*sendbuf,*recvbuf,count,datatype,op,root,comm)
			���������еĽ����У�ִ��һ����Լ���������ѽ�������ָ����һ��������

		MPI_Allreduce
			MPI_Allreduce (*sendbuf,*recvbuf,count,datatype,op,comm)
			ִ��һ����Լ���������ѽ���㲥�����ڵ����н����С��ò����ȼ�����ִ�� MPI_Reduce ������Ȼ����ִ�� MPI_Bcast

		MPI_Reduce_scatter
			MPI_Reduce_scatter (*sendbuf,*recvbuf,recvcount,datatype, op,comm)
			�����ڰ�Ԫ�ص�λ�Ƚ��й�Լ������ Ȼ��ѽ���ֶκ�ֲ������ڵ����������ϡ� �ò� ���ȼ�����ִ�� MPI_Reduce ������Ȼ����ִ�� MPI_ Scatter ����


		MPI_Alltoall
			MPI_Alltoall (*sendbuf,sendcount,sendtype,*recvbuf, recvcnt,recvtype,comm)
			����ÿ������ִ��һ����ɢ���������Ͳ�ͬ����Ϣ�����ڵ����н����ϣ������Լ���

		MPI_Scan
			MPI_Scan (*sendbuf,*recvbuf,count,datatype,op,comm)
			�����Էֲ��ڽ������ϵ�����ִ��ǰ׺��Լ

*/


#include <iostream>
#include "mpi.h"
#include <Windows.h>

using namespace std;

void barrier_test(int rank)
{
	cout << "rank " << rank << "ready 0" << endl;
	Sleep(rank * 1000);
	cout << "rank " << rank << "ready" << endl;

	MPI_Barrier(MPI_COMM_WORLD);	// ͬ�����������н��̶��ﵽ��Ż�ִ����һ��

	cout << "rank " << rank << "ENTER BARRIER" << endl;
}

void bcast_test(int rank)
{
	char buf[100] = { 0 };
	int root = 0;

	if (rank == root)
		sprintf(buf, "I am ţ�� from %d", rank);


	// ���ͺͽ��ն���buffer����
	MPI_Bcast(buf, 100, MPI_CHAR, root, MPI_COMM_WORLD);// ���еĽ��̶�Ҫ���ã����ú����� buffer �����㲥Դ���̵�ֵ���ǣ���root���̽���������

	cout << "rank " << rank << " recv " << buf << endl;
}

void scatter_test(int rank)
{
	const int bufsize = 4;

	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//int matrix[4][4] = {
	//	{1,2,3,4},
	//	{5,6,7,8},
	//	{9,10,11,12},
	//	{13,14,15,16} };

	int* matrix = new int[bufsize * size];
	for (size_t i = 0; i < bufsize * size; i++)
	{
		matrix[i] = i;
	}


	int recv[bufsize] = { 0 };
	int root = 0;

	MPI_Scatter(matrix, bufsize, MPI_INT, recv, bufsize, MPI_INT, root, MPI_COMM_WORLD);	// �����ݷ�ɢ�����н���

	cout << "rank " << rank << " recv ";
	for (size_t i = 0; i < bufsize; i++)
	{
		cout << recv[i] << " ";
	}
	cout << endl;
}

void gather_test(int rank)
{
	const int bufsize = 4;
	int senbuf[bufsize] = { 10 * rank, 1 + 10 * rank,2 + 10 * rank,3 + 10 * rank };

	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int* recvbuf = new int[bufsize * size];

	int root = 0;
	MPI_Gather(senbuf, bufsize, MPI_INT, recvbuf, bufsize, MPI_INT, root, MPI_COMM_WORLD);	// �������ݣ���ָ��root���̽�������

	if (root == rank)
	{
		cout << "rank " << rank << " recv ";
		for (size_t i = 0; i < bufsize * size; i++)
		{
			cout << recvbuf[i] << " ";
		}
		cout << endl;
	}
	else
	{
		cout << "rank " << rank << " send ";
		for (size_t i = 0; i < bufsize; i++)
		{
			cout << senbuf[i] << " ";
		}
		cout << endl;
	}
}

void allgather_test(int rank)
{
	const int bufsize = 4;
	int senbuf[bufsize] = { 10 * rank, 1 + 10 * rank,2 + 10 * rank,3 + 10 * rank };

	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int* recvbuf = new int[bufsize * size];

	MPI_Allgather(senbuf, bufsize, MPI_INT, recvbuf, bufsize, MPI_INT, MPI_COMM_WORLD);	// �������ݣ����������н��̷��͵�����

	cout << "rank " << rank << " recv ";
	for (size_t i = 0; i < bufsize * size; i++)
	{
		cout << recvbuf[i] << " ";
	}
	cout << endl;
}

void reduce_test(int rank)
{
	int sendbuf = 10 * rank;

	int recv = 0;

	int root = 0;
	MPI_Reduce(&sendbuf, &recv, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);	// ִ�й�Լ����ָ��root���շ���ֵ

	if (root == rank)
	{
		cout << "rank " << rank << " recv " << recv << endl;
	}
	else
	{
		cout << "rank " << rank << " send " << sendbuf << endl;
	}
}

void scan_test(int rank)
{
	int recv = 0;
	int send = rank + 1;
	MPI_Scan(&send, &recv, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);	// ǰ׺��Լ��������С�ڵ�ǰrank�����ֵ

	cout << "rank " << rank << " recv " << recv << endl;
}

int main_group(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//barrier_test(rank);

	//bcast_test(rank);

	//scatter_test(rank);

	//gather_test(rank);

	//allgather_test(rank);

	//reduce_test(rank);

	scan_test(rank);

	MPI_Finalize();
	return 0;
}
