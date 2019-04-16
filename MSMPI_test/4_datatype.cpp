/*
派生数据类型：
	1. 连续数据类型
		MPI_Type_contiguous
			MPI_Type_contiguous (count,oldtype,*newtype) : 相同基础类型构成的新连续数据类型

	2. 向量数据类型
		MPI_Type_vector 、MPI_Type_hvector、MPI_Type_create_hvector
			MPI_Type_vector (count,blocklength,stride,oldtype,*newtype) : 相同基础数据类型构成的新的不连续数据类型，两个函数跨度有原点不同的区别
			MPI_Type_create_hvector(count,blocklength,stride,oldtype,*newtype) ; 相同基础数据类型构成的新的向量数据类型，跨度以块内存原点为起点

	3. 索引数据类型
		MPI_Type_indexed 、MPI_Type_hindexed
			MPI_Type_indexed (count,blocklens[],offsets[],old_type,*newtype) : 与MPI_Type_vector类似，但是可以单独定义每一小块的长度和跨度

	4. 结构体数据类型
		MPI_Type_struct (已弃用)
			MPI_Type_struct (count,blocklens[],offsets[],old_types,*newtype) : 最通用的类型构造函数，多种类型的MPI_Type_indexed
		MPI_Type_create_struct(新)
			MPI_Type_create_struct(count, array_of_blocklengths[], array_of_displacements[], array_of_types[],*rowtype);

	5. 数据类型长度
		MPI_Type_extent (已弃用)
			MPI_Type_extent (datatype,*extent) : extent返回数据类型长度
		MPI_Type_get_extent(新)
			MPI_Type_get_extent(rowtype, *lb, *extent) : lb返回最小数据类型长度，extent返回数据类型长度

	6. 提交数据类型到MPI
		MPI_Type_commit
			MPI_Type_commit (datatype) : 提交数据类型，使MPI认可该数据类型

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

// 连续数据类型类型
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

	MPI_Type_contiguous(SIZE, MPI_FLOAT, &rowtype);	// 创建由4个连续的MPI_FLOAT构成的新的数据类型
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

// 向量数据类型类型
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


	MPI_Type_vector(1, SIZE, 0, MPI_FLOAT, &columntype);	 // 计数 1；块长度 4；跨度 0 , 跨度以当前块起始点为原点
	//MPI_Type_vector(SIZE, 1, 0, MPI_FLOAT, &columntype);	 // 计数 4；块长度 1；跨度 0 , 跨度以当前块起始点为原点
	//MPI_Type_create_hvector(SIZE, 1, sizeof(MPI_FLOAT), MPI_FLOAT, &columntype);	// 计数 4；块长度 1；跨度 sizeof(MPI_FLOAT) , 跨度以index=0为原点
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

// 索引数据类型测试
void indexed_test(int rank)
{
	int numtasks, source = 0, dest, tag = 1, i;
	int count = 2;
	int blocklengths[2] = { 4,2 }, displacements[2] = { 5,12 };
	float a[16] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0 };

	// 从a中取 6、7、8、9 和 13、14

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

			// 抽取矩阵中对应位置数据
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

// 结构体数据类型
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

	// 内存块结构 ： FLOAT_FLOAT_INT_INT 共16个byte
	int array_of_blocklengths[2] = { 2,2 };	// 数据类型个数
	MPI_Aint array_of_displacements[2] = { 0,0 };	// 数据间隔
	array_of_displacements[1] = 2 * sizeof(MPI_FLOAT);	// 距离index=0的位置，第二块据0的位置为 2 * sizeof(MPI_FLOAT)

	MPI_Datatype array_of_types[2] = { MPI_FLOAT, MPI_INT };	// 基础数据类型

	//MPI_Type_struct(1, array_of_blocklengths, array_of_displacements, array_of_types, &rowtype);	// 已弃用
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
