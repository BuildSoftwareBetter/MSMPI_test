/*
2. 点对点通信
	阻塞发送：MPI_Send(buffer,count,type,dest,tag,comm)
	非阻塞发送：MPI_Isend(buffer,count,type,dest,tag,comm,request)
	阻塞接收：MPI_Recv(buffer,count,type,source,tag,comm,status)
	非阻塞接收：MPI_Irecv(buffer,count,type,source,tag,comm,request)


	参数说明：
		buffer： 数据
		count：数据(type)个数
		type： 数据类型,MPI_CHAR等
		dest： 指定接收进程的秩
		source：指定发送进程的秩，MPI_ANT_SOURCE代表接收任意源进程发过来的消息
		tag：标识符，（0-32767）唯一标识一个消息，发送和接收操作的标识符一定要匹配，MPI_ANY_TAG可与任何发送操作的tag匹配
		comm： 通信因子，可使用全局通信因子API_COMM_WORLD
		status： 包含了消息的源进程source和消息的标识符tag
		request： 请求，用于非阻塞发送和非阻塞接收操作


	阻塞消息通信：
	MPI_Send
		阻塞发送，进程发送缓冲区空闲时，函数返回，可继续发送
	MPI_Recv
		阻塞接收，进程接收缓冲区空闲时，函数返回
	MPI_Ssend
		同步阻塞发送，发送进程已空闲，并且接收进程已经开始接收时，函数返回
	MPI_Bsend
		缓冲区阻塞发送，应用程序首先应申请一个足够大的缓冲区， 然后用 MPI_Buffer_attach 函数加以确认，当 MPI_Bsend 函数返回时，消息数据已经从应用程序发送缓冲区拷贝到分配的缓冲区中。

	MPI_Buffer_attach 、MPI_Buffer_detach
		用于分配和释放用于 MPI_Bsend 函数的发送缓冲区， size 参数是以字节为计的缓冲区 大小：

	MPI_Rsend
		预备方式的阻塞发送。 如果能确认接收进程已经开始匹配接收时，可以使用该发送函数
	MPI_Sendrecv
		阻塞发送并阻塞接收一个消息。 只有当发送缓冲区空并接收缓冲区消息有效后该函数才返回

	MPI_Wait 、MPI_Waitany 、MPI_Waitall 、MPI_Waitsome
		函数只有当指定的消息发送或接收完成后才返回，对于多个非阻塞的操作， 程序员可以指定任何一个或一些或全部的消息发送或接收完成后再返回
		MPI_Wait (*request,*status)
			等待指定的非阻塞操作完成
		MPI_Waitany (count,*array_of_requests,*index,*status)
			等待任一非阻塞操作完成
		MPI_Waitall (count,*array_of_requests,*array_of_statuses)
			等待所有非阻塞操作完成
		MPI_Waitsome (incount,*array_of_requests,*outcount, array_of_offsets, *array_of_statuses)

	MPI_Probe
		MPI_Probe (source,tag,comm,*status)
			探测消息是否接收完成，只有当探测到消息接收完成才返回

	非阻塞消息通信：


*/

#include <iostream>
#include <Windows.h>
#include "mpi.h"

using namespace std;

// 定义默认接收信息的进程
#define MSG_RECV_RANK 0

#define MSG_SIZE 100

// 消息标识符
#define MSG_TAG 0

// 阻塞发送测试
void send_test();

// 同步阻塞发送测试
void ssend_test();

// 缓冲区阻塞发送
void bsend_test();

// 预备方式阻塞发送测试
void rsend_test();

// 阻塞接收测试
void recv_test(int source);

// 阻塞发送接收测试
void sendrecv_test();

void recvsend_test(int source);

void sendrecv_recvsend_test()
{
	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (MSG_RECV_RANK == rank)
	{
		// 接收数据
		for (size_t i = 1; i < size; i++)
		{
			// 接收rank顺序是 1 2 3 
			recvsend_test(i);
		}
	}
	else
	{
		Sleep(1000 * (rank - 1));
		sendrecv_test();
	}
}

int main_rs(int argc, char * argv[])
{
	MPI_Init(&argc, &argv);	// 初始化环境

	sendrecv_recvsend_test();

	MPI_Finalize();	// 销毁MPI环境

	return 0;
}

int main_ss(int argc, char * argv[])
{
	MPI_Init(&argc, &argv);	// 初始化环境

	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (MSG_RECV_RANK == rank)
	{
		// 接收数据
		for (size_t i = 1; i < size; i++)
		{
			//Sleep(1000);
			// 以阻塞方式接收
			recv_test(i);
		}
	}
	else
	{
		// 发送数据

		// 阻塞方式发送
		send_test();

		// 同步阻塞发送
		//ssend_test();

		// 缓冲区阻塞发送
		//bsend_test();

		// 就绪发送
		//Sleep(1000 * (rank - 1)); // 由于接收顺序是 1 2 3，Sleep是为了使发送顺序也是1 2 3
		//rsend_test();
	}

	MPI_Finalize();	// 销毁MPI环境

	return 0;
}


void send_test()
{
	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char str[MSG_SIZE] = { 0 };
	sprintf(str, "use send hello i'm rank %d", rank);
	int strsize = strlen(str);

	// 1. 未实现缓冲消息的情况：存在匹配的接收进程，则将消息传递至接收进程后返回；若不存在匹配的接收进程则等待，直到将消息传递至匹配的接收进程
	// 2. 实现了缓冲消息，存在匹配的接收进程，则将消息传递至接收进程后返回；若不存在匹配的接收进程则与Bsend类似
	int rc = MPI_Send(str, strlen(str), MPI_CHAR, MSG_RECV_RANK, MSG_TAG, MPI_COMM_WORLD);	// 发送完就返回,如果存在匹配的接收进程则返回，不存在匹配的接收进程则等待

	if (MPI_SUCCESS == rc)
	{
		double wtime = MPI_Wtime();	// 获取运行时间
		double wtick = MPI_Wtick();	// 获取时间单位(s),如果是毫秒则为0.001
		cout << "rank " << rank << " send success, times :" << wtick * wtime << endl;
	}
	else
	{
		cout << "rank " << rank << " send failed " << rc << endl;
	}
}

// 同步阻塞发送测试，等待接收完返回；send只是传递至接收进程即返回，ssend等待接收进程接收完后返回
void ssend_test()
{
	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char str[MSG_SIZE] = { 0 };
	sprintf(str, "use ssend hello i'm rank %d", rank);
	int strsize = strlen(str);


	MPI_Ssend(str, strlen(str), MPI_CHAR, MSG_RECV_RANK, MSG_TAG, MPI_COMM_WORLD);	// 必须接收后才返回


	double wtime = MPI_Wtime();	// 获取运行时间
	double wtick = MPI_Wtick();	// 获取时间单位(s),如果是毫秒则为0.001
	cout << "rank " << rank << " ssend success, times :" << wtick * wtime << endl;
}

// 缓冲区阻塞发送,数据放在缓冲区，发送完即返回，不等待接收
void bsend_test()
{
	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char str[MSG_SIZE] = { 0 };

	sprintf(str, "use bsend hello i'm rank %d", rank);
	int strsize = strlen(str);



	int bsize = 0;

	MPI_Pack_size(strsize, MPI_CHAR, MPI_COMM_WORLD, &bsize);

	char * tmpbuffer = nullptr;

	tmpbuffer = (char*)malloc(bsize + 2 * MPI_BSEND_OVERHEAD);

	if (!tmpbuffer) {
		MPI_Abort(MPI_COMM_WORLD, 1);
	}


	MPI_Buffer_attach(tmpbuffer, bsize + 2 * MPI_BSEND_OVERHEAD);

	MPI_Bsend(str, strlen(str), MPI_CHAR, MSG_RECV_RANK, MSG_TAG, MPI_COMM_WORLD);

	char * buf;
	int tsize = 0;
	MPI_Buffer_detach(&buf, &tsize);

	double wtime = MPI_Wtime();	// 获取运行时间
	double wtick = MPI_Wtick();	// 获取时间单位(s),如果是毫秒则为0.001
	cout << "rank " << rank << " bsend success, times :" << wtick * wtime << endl;
}

// 预备方式阻塞发送，接收进程必须已经处于recv状态，否则立即返回错误
void rsend_test()
{
	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char str[MSG_SIZE] = { 0 };

	sprintf(str, "use rsend hello i'm rank %d", rank);
	int strsize = strlen(str);


	int rc = MPI_Rsend(str, strsize, MPI_CHAR, MSG_RECV_RANK, MSG_TAG, MPI_COMM_WORLD);	// 必须接收端处于recv，发送失败会直接抛异常

	if (MPI_SUCCESS == rc) {
		double wtime = MPI_Wtime();	// 获取运行时间
		double wtick = MPI_Wtick();	// 获取时间单位(s),如果是毫秒则为0.001
		cout << "rank " << rank << " rsend success, times :" << wtick * wtime << endl;
	}
	else
	{
		cout << "rank " << rank << " rsend failed " << rc << endl;
	}
}

void recv_test(int source)
{
	char str[MSG_SIZE] = { 0 };

	MPI_Status status;

	MPI_Recv(str, MSG_SIZE, MPI_CHAR, source, MSG_TAG, MPI_COMM_WORLD, &status);

	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	cout << "rank " << rank << " recv from " << source << " message : " << str << endl;
}

void recvsend_test(int source)
{
	char str[MSG_SIZE] = { 0 };

	MPI_Status status;

	MPI_Recv(str, MSG_SIZE, MPI_CHAR, source, MSG_TAG, MPI_COMM_WORLD, &status);


	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	cout << "rank " << rank << " recvsend from " << source << " message : " << str << endl;


	char send_str[MSG_SIZE] = { 0 };
	sprintf(send_str, "in recvsend use send hello i'm rank %d", rank);
	int send_strsize = strlen(send_str);

	// 1. 未实现缓冲消息的情况：存在匹配的接收进程，则将消息传递至接收进程后返回；若不存在匹配的接收进程则等待，直到将消息传递至匹配的接收进程
	// 2. 实现了缓冲消息，存在匹配的接收进程，则将消息传递至接收进程后返回；若不存在匹配的接收进程则与Bsend类似
	MPI_Send(send_str, strlen(send_str), MPI_CHAR, source, MSG_TAG, MPI_COMM_WORLD);	// 发送完就返回,如果存在匹配的接收进程则返回，不存在匹配的接收进程则等待


	double wtime = MPI_Wtime();	// 获取运行时间
	double wtick = MPI_Wtick();	// 获取时间单位(s),如果是毫秒则为0.001
	cout << "rank " << rank << " recvsend success, times :" << wtick * wtime << endl;
}

void sendrecv_test()
{
	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char str[MSG_SIZE] = { 0 };

	sprintf(str, "use sendrecv hello i'm rank %d", rank);
	int strsize = strlen(str);

	char recv_buf[MSG_SIZE] = { 0 };

	MPI_Status status;
	MPI_Sendrecv(str, strsize, MPI_CHAR, MSG_RECV_RANK, MSG_TAG, recv_buf, MSG_SIZE, MPI_CHAR, MSG_RECV_RANK, MSG_TAG, MPI_COMM_WORLD, &status);

	cout << "rank " << rank << " sendrecv from " << MSG_RECV_RANK << " message : " << recv_buf << endl;
}