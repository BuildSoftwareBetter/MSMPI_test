/*
1. MPI ����
	ͷ�ļ��� mpi.h
	���ø�ʽ��	rc = MPI_XXX(parameter,...)
				������óɹ���rc ���� MPI_SUCCESS

	����ṹ��
		// ����ͷ�ļ�
		#include "mpi.h"

		// ��ʼ������
		MPI_Init(...)

		// ��Ϣ����������
		MPI_XXX(...)

		// �˳�MPI����
		MPI_Finalize()

	ͨ�����Ӻ��飺
		MPI_COMM_WORLD ���н��̶���MPI_COMM_WORLDͨ��������
		MPIͨ��ָ��ͨ�����Ӻ�������ɸ������̼��ͨ��
	�ȣ�
		��������ͨ�����е�Ψһ��ʶ��


	int MPI_Get_my_id() ��ȡ����ID��
	int MPI_Get_log_id() ��ȡ��ǰ�����������߼�PE��
	void MPI_Get_longtimed(double *dt) ��ȡϵͳ��ʱ��(��msΪ��λ)
	char *MPI_Init_node_shared_mem(int argc,char **argv, long StartAddr,unsigned long MemLen) ��ʼ���ڵ�Ĺ���ռ�
	char *MPI_Get_node_shared_mem() ��ȡ�ڵ㹲��ռ�
	char *MPI_Init_pe_shared_mem(int argc,char **argv, long StartAddr,unsigned long MemLen) ��ʼ���ֲ�����ռ�
	char *MPI_Get_pe_shared_mem() ��ȡ�ֲ�����ռ�


*/

#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <windows.h>

using namespace std;

int main_1(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);	//�������������ʼ������

	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	// ��ȡ��ǰ������MPI_COMM_WORLDͨ�����������

	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);	// ��ȡMPI_COMM_WORLDͨ������������Ľ�����

	// MPI_Abort(MPI_COMM_WORLD, 0);	// ����MPI_COMM_WORLDͨ������������н���

	//Sleep(rank * 1000);

	// ��ȡ��ǰ�������ڴ�����������
	char process_name[300] = { 0 };
	int process_name_len = 0;
	MPI_Get_processor_name(process_name, &process_name_len);

	double wtime = MPI_Wtime();	// ��ȡ����ʱ��
	double wtick = MPI_Wtick();	// ��ȡʱ�䵥λ(s),����Ǻ�����Ϊ0.001

	cout << "current process name: " << process_name << "; process rank: " << rank << "/" << size << "; exec time: " << wtick * wtime << " s" << endl;

	MPI_Finalize();	// ����MPIִ�л���

	return 0;
}