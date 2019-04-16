//
#include <stdio.h>
#include "mpi.h"

int main0(int argc, char*argv[])
{
	/*MPI_Init(&argc, &argv);
	printf("hello world\n");
	MPI_Finalize();
	return 0;*/

	printf("enter MPI \n");

	int rank, numproces;
	int namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	// ��ʼ��MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);//�����ͨ�������ڵĽ��̺ţ��ȣ�
	MPI_Comm_size(MPI_COMM_WORLD, &numproces);//����ͨ���ӹ����Ľ�����

	MPI_Get_processor_name(processor_name, &namelen);
	//fprintf(stderr, "hello world! process %d of %d on %s\n", rank, numproces, processor_name);
	fprintf(stdout, "hello world! process %d of %d on %s\n", rank, numproces, processor_name);

	MPI_Finalize();

	return 0;
}