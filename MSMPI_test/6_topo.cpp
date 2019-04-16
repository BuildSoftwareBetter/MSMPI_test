/*
�������ˣ�
	1. ��Ҫ�����˽ṹ���ѿ�����ͼ
	2. �������˽��������ͨ������֮��

	MPI_Cart_coords (comm,rank,maxdims,*coords[]) �� �ѿ���ת���������ѽ�����ת��������
	MPI_Cart_create (comm_old,ndims,*dims[],*periods, reorder,*comm_cart) �� ���ݵѿ���������Ϣ����һ���µ�ͨ������
	MPI_Cart_get (comm,maxdims,*dims,*periods,*coords[]) �� ������ͨ��������صĵѿ���������Ϣ
	MPI_Cart_map (comm_old,ndims,*dims[],*periods[],*newrank) �� ������MPIϵͳ�Ƽ��ĵѿ���ӳ��ͼ
	MPI_Cart_rank (comm,*coords[],*rank) �� ���߼���������ת���ɽ�����
	MPI_Cart_shift (comm,direction,displ,*source,*dest) �� ��ɵѿ�����λ������������λ�������λ�����Сȷ����λ����
	MPI_Cart_sub (comm,*remain_dims[],*comm_new) �� �ú���������ͨ��������ֳɽϵ�ά�ѿ�������������飬 ����Ϊÿ�����鹹��һ���� �����������ѿ������˵�ͨ������
	MPI_Cartdim_get (comm,*ndims) �� ������ͨ��������صĵѿ����ṹ��ά��
	MPI_Dims_create (nnodes,ndims,*dims[]) �� �ú��������û���ÿ�����귽��ѡ��һ�ֽ��̵ľ���ֲ��� ��ȡ���ڱ���������еĽ� �� �� �� �� �� �� �� ָ �� �� �� �� �� �� �� ʹ �� �� �� �� �� һ �� �� �� �� �� �� �� �� �� �� �� ��MPI_COMM_WORLD�����С���ֳ�һ�� n ά������
	MPI_Graph_create (comm_old,nnodes,*index[],*edges[], reorder,*comm_graph) ������һ����ͼ������Ϣ��������ͨ������
	MPI_Graph_get (comm,maxindex,maxedges,*index[],*edges[]) �� ������ͨ�������������ͼ������Ϣ
	MPI_Graph_map (comm_old,nnodes,*index[],*edges[],*newrank) �� ������ͼ��������Ϣӡ��
	MPI_Graph_neighbors (comm,rank,maxneighbors,*neighbors[]) �� ������ָ��������ص��ٽ��ڵ�����
	MPI_Graphdims_get (comm,*nnodes,*nedges) �� ����ͼ�еĽڵ����ͱ���
	MPI_Topo_test (comm,*top_type) �� ���ظ���ͨ�����ӵ��������ͣ�MPI_GRAPH ͼ���ˣ�MPI_CART �ѿ������ˣ�MPI_UNDEFINED û������



*/


#include "mpi.h" 
#include <stdio.h> 
#include <iostream>

using namespace std;

#define SIZE 16 
#define UP 0 
#define DOWN 1 
#define LEFT 2 
#define RIGHT 3

int main_topo(int argc, char *argv[])
{
	int numtasks, rank, source, dest, outbuf, i, tag = 1;
	int	inbuf[4] = { MPI_PROC_NULL,MPI_PROC_NULL,MPI_PROC_NULL,MPI_PROC_NULL, };
	int nbrs[4];
	int dims[2] = { 4,4 };
	int periods[2] = { 0,0 };
	int reorder = 0;
	int coords[2];

	MPI_Request reqs[8];
	MPI_Status stats[8];
	MPI_Comm cartcomm;
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	if (numtasks == SIZE) {

		MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cartcomm);	// Ϊ�ѿ������˴���һ���µ�ͨ����

		MPI_Comm_rank(cartcomm, &rank);	// ��ȡ��ǰ������ͨ�����е���

		MPI_Cart_coords(cartcomm, rank, 2, coords);	// �ѿ���ת������������ת��������

		//cout << "rank " << rank << " coords : " << coords[0] << "," << coords[1] << endl;

		MPI_Cart_shift(cartcomm, 0, 1, &nbrs[UP], &nbrs[DOWN]);	// �ѿ�����λ����,��ȡ��ǰ����λ�õ�����λ����

		//cout << "rank " << rank << " vertical shift coords : " << coords[0] << "," << coords[1] << endl;

		MPI_Cart_shift(cartcomm, 1, 1, &nbrs[LEFT], &nbrs[RIGHT]);	// �ѿ�����λ����,��ȡ��ǰ����λ�õ�����λ����

		//cout << "rank " << rank << " horizen shift coords : " << coords[0] << "," << coords[1] << endl;

		cout << "rank " << rank << " coords : " << coords[0] << "," << coords[1] << " nbrs : " << nbrs[UP] << "," << nbrs[DOWN] << "," << nbrs[LEFT] << "," << nbrs[RIGHT] << endl;

		/*
		�ȵľ���
		| 0 | 1 | 2 | 3 |
		| 4 | 5 | 6 | 7 |
		| 8 | 9 | 10| 11|
		| 12| 13| 14| 15|

		�ȵ����꼰�ڽ����ȣ�
		| �� | ���� | �� | �� | �� | �� |
		| 0  |(0,0) | -1 | 4  | -1 | 1  |
		| 1  |(0,1) | -1 | 5  | 0  | 2  |
		| 2  |(0,2) | -1 | 6  | 1  | 3  |
		| 3  |(0,3) | -1 | 7  | 2  | -1 |
		| 4  |(1,0) | 0  | 8  | -1 | 5  |
		| 5  |(1,1) | 1  | 9  | 4  | 6  |
		| 6  |(1,2) | 2  | 10 | 5  | 7  |
		| 7  |(1,3) | 3  | 11 | 6  | -1 |
		| 8  |(2,0) | 4  | 12 | -1 | 9  |
		| 9  |(2,1) | 5  | 13 | 8  | 10 |
		| 10 |(2,2) | 6  | 14 | 9  | 11 |
		| 11 |(2,3) | 7  | 15 | 10 | -1 |
		| 12 |(3,0) | 8  | -1 | -1 | 13 |
		| 13 |(3,1) | 9  | -1 | 12 | 14 |
		| 14 |(3,2) | 10 | -1 | 13 | 15 |
		| 15 |(3,3) | 11 | -1 | 14 | -1 |


		*/
		outbuf = rank;
		for (i = 0; i < 4; i++) {

			// �����ݷֱ��͸���������4���ھ�
			// ����������4���ھӽ�������

			dest = nbrs[i];
			source = nbrs[i];

			if (-1 == dest) continue;

			// ����
			MPI_Isend(&outbuf, 1, MPI_INT, dest, tag,
				MPI_COMM_WORLD, &reqs[i]);

			// ����
			MPI_Irecv(&inbuf[i], 1, MPI_INT, source, tag,
				MPI_COMM_WORLD, &reqs[i + 4]);

		}
		MPI_Waitall(8, reqs, stats);

		printf("rank= %d coords= %d %d neighbors(u,d,l,r)= %d %d %d %d\n", rank, coords[0], coords[1], nbrs[UP], nbrs[DOWN], nbrs[LEFT], nbrs[RIGHT]);
		printf("rank= %d inbuf(u,d,l,r)= %d %d %d %d\n", rank, inbuf[UP], inbuf[DOWN], inbuf[LEFT], inbuf[RIGHT]);

	}
	else
		printf("Must specify %d processors. Terminating.\n", SIZE);

	MPI_Finalize();

	return 0;
}