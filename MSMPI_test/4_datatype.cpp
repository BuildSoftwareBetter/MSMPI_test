/*
�����������ͣ�
	1. ������������
		MPI_Type_contiguous
			MPI_Type_contiguous (count,oldtype,*newtype) : ��ͬ�������͹��ɵ���������������

	2. ������������
		MPI_Type_vector ��MPI_Type_hvector��MPI_Type_create_hvector
			MPI_Type_vector (count,blocklength,stride,oldtype,*newtype) : ��ͬ�����������͹��ɵ��µĲ������������ͣ��������������ԭ�㲻ͬ������
			MPI_Type_create_hvector(count,blocklength,stride,oldtype,*newtype) ; ��ͬ�����������͹��ɵ��µ������������ͣ�����Կ��ڴ�ԭ��Ϊ���

	3. ������������
		MPI_Type_indexed ��MPI_Type_hindexed
			MPI_Type_indexed (count,blocklens[],offsets[],old_type,*newtype) : ��MPI_Type_vector���ƣ����ǿ��Ե�������ÿһС��ĳ��ȺͿ��

	4. �ṹ����������
		MPI_Type_struct (������)
			MPI_Type_struct (count,blocklens[],offsets[],old_types,*newtype) : ��ͨ�õ����͹��캯�����������͵�MPI_Type_indexed
		MPI_Type_create_struct(��)
			MPI_Type_create_struct(count, array_of_blocklengths[], array_of_displacements[], array_of_types[],*rowtype);

	5. �������ͳ���
		MPI_Type_extent (������)
			MPI_Type_extent (datatype,*extent) : extent�����������ͳ���
		MPI_Type_get_extent(��)
			MPI_Type_get_extent(rowtype, *lb, *extent) : lb������С�������ͳ��ȣ�extent�����������ͳ���

	6. �ύ�������͵�MPI
		MPI_Type_commit
			MPI_Type_commit (datatype) : �ύ�������ͣ�ʹMPI�Ͽɸ���������

*/



#include <iostream>
#include "mpi.h"
#include <Windows.h>

using namespace std;

#define SIZE 4
#define NELEMENTS 6 

struct rectangle
{
	float x, y;
	int width, height;
};

// ����������������
void contiguous_test(int rank1)
{
	int numtasks, rank, source = 0, dest, tag = 1, i;
	float a[SIZE][SIZE] =
	{ 1.0, 2.0, 3.0, 4.0,
	5.0, 6.0, 7.0, 8.0,
	9.0, 10.0, 11.0, 12.0,
	13.0, 14.0, 15.0, 16.0 };
	float b[SIZE];
	MPI_Status stat;
	MPI_Datatype rowtype;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	MPI_Type_contiguous(SIZE, MPI_FLOAT, &rowtype);	// ������4��������MPI_FLOAT���ɵ��µ���������
	MPI_Type_commit(&rowtype);

	MPI_Aint extent = 0;
	MPI_Aint lb = 0;
	MPI_Type_get_extent(rowtype, &lb, &extent);

	cout << "lb " << lb << ",extent " << extent << endl;

	if (numtasks == SIZE) {
		if (rank == 0) {
			Sleep(1000);
			for (i = 1; i < numtasks; i++)
				MPI_Send(&a[i][0], 1, rowtype, i, tag, MPI_COMM_WORLD);
		}
		else
		{
			MPI_Recv(b, SIZE, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &stat);
			printf("rank= %d b= %3.1f %3.1f %3.1f %3.1f\n",
				rank, b[0], b[1], b[2], b[3]);
		}
	}
	else
		printf("Must specify %d processors. Terminating.\n", SIZE);

}

// ����������������
void vector_test(int rank1)
{
	int numtasks, rank, source = 0, dest, tag = 1, i;
	float a[SIZE][SIZE] =
	{ 1.0, 2.0, 3.0, 4.0,
	5.0, 6.0, 7.0, 8.0,
	9.0, 10.0, 11.0, 12.0,
	13.0, 14.0, 15.0, 16.0 };
	float b[SIZE];
	MPI_Status stat;
	MPI_Datatype columntype;
	MPI_Aint extent = 0;
	MPI_Aint lb = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);


	MPI_Type_vector(1, SIZE, 0, MPI_FLOAT, &columntype);	 // ���� 1���鳤�� 4����� 0 , ����Ե�ǰ����ʼ��Ϊԭ��
	//MPI_Type_vector(SIZE, 1, 0, MPI_FLOAT, &columntype);	 // ���� 4���鳤�� 1����� 0 , ����Ե�ǰ����ʼ��Ϊԭ��
	//MPI_Type_create_hvector(SIZE, 1, sizeof(MPI_FLOAT), MPI_FLOAT, &columntype);	// ���� 4���鳤�� 1����� sizeof(MPI_FLOAT) , �����index=0Ϊԭ��
	MPI_Type_commit(&columntype);


	MPI_Type_get_extent(columntype, &lb, &extent);
	cout << "lb " << lb << ",extent " << extent << endl;

	if (numtasks == SIZE) {
		if (rank == 0) {
			Sleep(100);
			for (i = 1; i < numtasks; i++)
				MPI_Send(&a[0][i], 1, columntype, i, tag, MPI_COMM_WORLD);
		}
		else
		{
			MPI_Recv(b, SIZE, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &stat);
			printf("rank= %d b= %3.1f %3.1f %3.1f %3.1f\n",
				rank, b[0], b[1], b[2], b[3]);
		}
	}
	else
		printf("Must specify %d processors. Terminating.\n", SIZE);
}

// �����������Ͳ���
void indexed_test(int rank)
{
	int numtasks, source = 0, dest, tag = 1, i;
	int count = 2;
	int blocklengths[2] = { 4,2 }, displacements[2] = { 5,12 };
	float a[16] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0 };

	// ��a��ȡ 6��7��8��9 �� 13��14

	float b[NELEMENTS];
	MPI_Status stat;
	MPI_Datatype indextype;
	MPI_Aint extent = 0;
	MPI_Aint lb = 0;

	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	MPI_Type_indexed(count, blocklengths, displacements, MPI_FLOAT, &indextype);
	MPI_Type_commit(&indextype);

	MPI_Type_get_extent(indextype, &lb, &extent);
	cout << "lb " << lb << ",extent " << extent << endl;

	if (rank == 0) {
		for (i = 1; i < numtasks; i++)

			// ��ȡ�����ж�Ӧλ������
			MPI_Send(a, 1, indextype, i, tag, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Recv(b, NELEMENTS, MPI_FLOAT, source, tag,
			MPI_COMM_WORLD, &stat);
		printf("rank= %d b= %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f\n",
			rank, b[0], b[1], b[2], b[3], b[4], b[5]);
	}
}

// �ṹ����������
void struct_test(int rank1)
{
	int numtasks, rank, source = 0, dest, tag = 1, i;
	rectangle a[SIZE];
	for (size_t i = 0; i < SIZE; i++)
	{
		a[i].x = i;
		a[i].y = i;
		a[i].height = i;
		a[i].width = i;
	}

	rectangle b;
	MPI_Status stat;
	MPI_Datatype rowtype;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	// �ڴ��ṹ �� FLOAT_FLOAT_INT_INT ��16��byte
	int array_of_blocklengths[2] = { 2,2 };	// �������͸���
	MPI_Aint array_of_displacements[2] = { 0,0 };	// ���ݼ��
	array_of_displacements[1] = 2 * sizeof(MPI_FLOAT);	// ����index=0��λ�ã��ڶ����0��λ��Ϊ 2 * sizeof(MPI_FLOAT)

	MPI_Datatype array_of_types[2] = { MPI_FLOAT, MPI_INT };	// ������������

	//MPI_Type_struct(1, array_of_blocklengths, array_of_displacements, array_of_types, &rowtype);	// ������
	MPI_Type_create_struct(2, array_of_blocklengths, array_of_displacements, array_of_types, &rowtype);
	MPI_Type_commit(&rowtype);

	MPI_Aint extent = 0;
	MPI_Aint lb = 0;
	MPI_Type_get_extent(rowtype, &lb, &extent);
	cout << "lb " << lb << ",extent " << extent << endl;

	if (numtasks == SIZE) {
		if (rank == 0) {
			Sleep(1000);
			for (i = 1; i < numtasks; i++)
				MPI_Send(&a[i], 1, rowtype, i, tag, MPI_COMM_WORLD);
		}
		else
		{
			MPI_Recv(&b, 1, rowtype, source, tag, MPI_COMM_WORLD, &stat);
			printf("rank= %d b= %3.1f %3.1f %d %d\n",
				rank, b.x, b.y, b.width, b.height);
		}
	}
	else
		printf("Must specify %d processors. Terminating.\n", SIZE);

}




int main_dt(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//contiguous_test(rank);

	//struct_test(rank);

	//vector_test(rank);

	indexed_test(rank);

	MPI_Finalize();
	return 0;
}
