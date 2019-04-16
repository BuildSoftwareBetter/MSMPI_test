/*
集合通信：
	集合操作的三种类型：
		1. 同步，等待所有进程都就绪
		2. 数据传递，广播、分散、收集、全部到全部
		3. 规约，集合中的其中一个进程收集所有进程的数据并计算（类似MapReduce）
	集合操作是阻塞的
	只支持MPI的标准数据类型，不支持派生类型

	函数：
		MPI_Barrier
			MPI_Barrier (comm)
			同步格栅，所有进程都到达后才继续执行

		MPI_Bcast
			MPI_Bcast(*buffer,count,datatype,root,comm)
			从指定的一个跟进程中把数据广播发送给组中的所有其它进程

			当前rank==root时，发送数据
			当前rank!=root时，从root中接收数据

		 MPI_Scatter
			MPI_Scatter (*sendbuf,sendcnt,sendtype,*recvbuf, recvcnt,recvtype,root,comm)
			把指定的跟进程中的数据分散发送给组中的所有进程（包括自己）

		MPI_Gather
			MPI_Gather (*sendbuf,sendcnt,sendtype,*recvbuf, recvcount,recvtype,root,comm)
			在组中指定一个进程收集组中所有进程发送来的消息，这个函数操作与 MPI_Scatter 函数操作相反

		MPI_Allgather
			MPI_Allgather (*sendbuf,sendcount,sendtype,*recvbuf, recvcount,recvtype,comm)
			在组中的每个进程都收集组中所有进程发送来的消息

		MPI_Reduce
			MPI_Reduce (*sendbuf,*recvbuf,count,datatype,op,root,comm)
			在组内所有的进程中，执行一个规约操作，并把结果存放在指定的一个进程中

		MPI_Allreduce
			MPI_Allreduce (*sendbuf,*recvbuf,count,datatype,op,comm)
			执行一个规约操作，并把结果广播到组内的所有进程中。该操作等价于先执行 MPI_Reduce 操作，然后再执行 MPI_Bcast

		MPI_Reduce_scatter
			MPI_Reduce_scatter (*sendbuf,*recvbuf,recvcount,datatype, op,comm)
			在组内按元素单位先进行规约操作， 然后把结果分段后分布到组内的其它进程上。 该操 作等价于先执行 MPI_Reduce 函数，然后再执行 MPI_ Scatter 函数


		MPI_Alltoall
			MPI_Alltoall (*sendbuf,sendcount,sendtype,*recvbuf, recvcnt,recvtype,comm)
			组中每个进程执行一个发散操作，发送不同的消息到组内的所有进程上（包括自己）

		MPI_Scan
			MPI_Scan (*sendbuf,*recvbuf,count,datatype,op,comm)
			用来对分布在进程组上的数据执行前缀归约

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

	MPI_Barrier(MPI_COMM_WORLD);	// 同步，必须所有进程都达到后才会执行下一步

	cout << "rank " << rank << "ENTER BARRIER" << endl;
}

void bcast_test(int rank)
{
	char buf[100] = { 0 };
	int root = 0;

	if (rank == root)
		sprintf(buf, "I am 牛逼 from %d", rank);


	// 发送和接收都是buffer参数
	MPI_Bcast(buf, 100, MPI_CHAR, root, MPI_COMM_WORLD);// 所有的进程都要调用，调用后所有 buffer 均被广播源进程的值覆盖，非root进程将接收数据

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

	MPI_Scatter(matrix, bufsize, MPI_INT, recv, bufsize, MPI_INT, root, MPI_COMM_WORLD);	// 将数据分散至所有进程

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
	MPI_Gather(senbuf, bufsize, MPI_INT, recvbuf, bufsize, MPI_INT, root, MPI_COMM_WORLD);	// 发送数据，并指定root进程接收数据

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

	MPI_Allgather(senbuf, bufsize, MPI_INT, recvbuf, bufsize, MPI_INT, MPI_COMM_WORLD);	// 发送数据，并接收所有进程发送的数据

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
	MPI_Reduce(&sendbuf, &recv, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);	// 执行规约，并指定root接收返回值

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
	MPI_Scan(&send, &recv, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);	// 前缀规约，返回与小于当前rank计算的值

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
