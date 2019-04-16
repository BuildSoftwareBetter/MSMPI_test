/*
1. MPI 环境
	头文件： mpi.h
	调用格式：	rc = MPI_XXX(parameter,...)
				如果调用成功，rc 返回 MPI_SUCCESS

	程序结构：
		// 包含头文件
		#include "mpi.h"

		// 初始化环境
		MPI_Init(...)

		// 消息交换并处理
		MPI_XXX(...)

		// 退出MPI环境
		MPI_Finalize()

	通信因子和组：
		MPI_COMM_WORLD 所有进程都在MPI_COMM_WORLD通信因子组
		MPI通过指定通信因子和组来完成各个进程间的通信
	秩：
		进程所在通信组中的唯一标识符


	int MPI_Get_my_id() 获取进程ID号
	int MPI_Get_log_id() 获取当前进程所处的逻辑PE号
	void MPI_Get_longtimed(double *dt) 获取系统长时钟(以ms为单位)
	char *MPI_Init_node_shared_mem(int argc,char **argv, long StartAddr,unsigned long MemLen) 初始化节点的共享空间
	char *MPI_Get_node_shared_mem() 获取节点共享空间
	char *MPI_Init_pe_shared_mem(int argc,char **argv, long StartAddr,unsigned long MemLen) 初始化分步共享空间
	char *MPI_Get_pe_shared_mem() 获取分步共享空间


*/

#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <windows.h>

using namespace std;

int main_1(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);	//根据输入参数初始化环境

	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	// 获取当前进程在MPI_COMM_WORLD通信因子组的秩

	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);	// 获取MPI_COMM_WORLD通信因子组包含的进程数

	// MPI_Abort(MPI_COMM_WORLD, 0);	// 结束MPI_COMM_WORLD通信因子组的所有进程

	//Sleep(rank * 1000);

	// 获取当前进程所在处理器的名称
	char process_name[300] = { 0 };
	int process_name_len = 0;
	MPI_Get_processor_name(process_name, &process_name_len);

	double wtime = MPI_Wtime();	// 获取运行时间
	double wtick = MPI_Wtick();	// 获取时间单位(s),如果是毫秒则为0.001

	cout << "current process name: " << process_name << "; process rank: " << rank << "/" << size << "; exec time: " << wtick * wtime << " s" << endl;

	MPI_Finalize();	// 结束MPI执行环境

	return 0;
}