/*
2. ��Ե�ͨ��
	�������ͣ�MPI_Send(buffer,count,type,dest,tag,comm)
	���������ͣ�MPI_Isend(buffer,count,type,dest,tag,comm,request)
	�������գ�MPI_Recv(buffer,count,type,source,tag,comm,status)
	���������գ�MPI_Irecv(buffer,count,type,source,tag,comm,request)


	����˵����
		buffer�� ����
		count������(type)����
		type�� ��������,MPI_CHAR��
		dest�� ָ�����ս��̵���
		source��ָ�����ͽ��̵��ȣ�MPI_ANT_SOURCE�����������Դ���̷���������Ϣ
		tag����ʶ������0-32767��Ψһ��ʶһ����Ϣ�����ͺͽ��ղ����ı�ʶ��һ��Ҫƥ�䣬MPI_ANY_TAG�����κη��Ͳ�����tagƥ��
		comm�� ͨ�����ӣ���ʹ��ȫ��ͨ������API_COMM_WORLD
		status�� ��������Ϣ��Դ����source����Ϣ�ı�ʶ��tag
		request�� �������ڷ��������ͺͷ��������ղ���


	������Ϣͨ�ţ�
	MPI_Send
		�������ͣ����̷��ͻ���������ʱ���������أ��ɼ�������
	MPI_Recv
		�������գ����̽��ջ���������ʱ����������
	MPI_Ssend
		ͬ���������ͣ����ͽ����ѿ��У����ҽ��ս����Ѿ���ʼ����ʱ����������
	MPI_Bsend
		�������������ͣ�Ӧ�ó�������Ӧ����һ���㹻��Ļ������� Ȼ���� MPI_Buffer_attach ��������ȷ�ϣ��� MPI_Bsend ��������ʱ����Ϣ�����Ѿ���Ӧ�ó����ͻ���������������Ļ������С�

	MPI_Buffer_attach ��MPI_Buffer_detach
		���ڷ�����ͷ����� MPI_Bsend �����ķ��ͻ������� size ���������ֽ�Ϊ�ƵĻ����� ��С��

	MPI_Rsend
		Ԥ����ʽ���������͡� �����ȷ�Ͻ��ս����Ѿ���ʼƥ�����ʱ������ʹ�ø÷��ͺ���
	MPI_Sendrecv
		�������Ͳ���������һ����Ϣ�� ֻ�е����ͻ������ղ����ջ�������Ϣ��Ч��ú����ŷ���

	MPI_Wait ��MPI_Waitany ��MPI_Waitall ��MPI_Waitsome
		����ֻ�е�ָ������Ϣ���ͻ������ɺ�ŷ��أ����ڶ���������Ĳ����� ����Ա����ָ���κ�һ����һЩ��ȫ������Ϣ���ͻ������ɺ��ٷ���
		MPI_Wait (*request,*status)
			�ȴ�ָ���ķ������������
		MPI_Waitany (count,*array_of_requests,*index,*status)
			�ȴ���һ�������������
		MPI_Waitall (count,*array_of_requests,*array_of_statuses)
			�ȴ����з������������
		MPI_Waitsome (incount,*array_of_requests,*outcount, array_of_offsets, *array_of_statuses)

	MPI_Probe
		MPI_Probe (source,tag,comm,*status)
			̽����Ϣ�Ƿ������ɣ�ֻ�е�̽�⵽��Ϣ������ɲŷ���

	��������Ϣͨ�ţ�


*/

#include <iostream>
#include <Windows.h>
#include "mpi.h"

using namespace std;

// ����Ĭ�Ͻ�����Ϣ�Ľ���
#define MSG_RECV_RANK 0

#define MSG_SIZE 100

// ��Ϣ��ʶ��
#define MSG_TAG 0

// �������Ͳ���
void send_test();

// ͬ���������Ͳ���
void ssend_test();

// ��������������
void bsend_test();

// Ԥ����ʽ�������Ͳ���
void rsend_test();

// �������ղ���
void recv_test(int source);

// �������ͽ��ղ���
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
		// ��������
		for (size_t i = 1; i < size; i++)
		{
			// ����rank˳���� 1 2 3 
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
	MPI_Init(&argc, &argv);	// ��ʼ������

	sendrecv_recvsend_test();

	MPI_Finalize();	// ����MPI����

	return 0;
}

int main_ss(int argc, char * argv[])
{
	MPI_Init(&argc, &argv);	// ��ʼ������

	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (MSG_RECV_RANK == rank)
	{
		// ��������
		for (size_t i = 1; i < size; i++)
		{
			//Sleep(1000);
			// ��������ʽ����
			recv_test(i);
		}
	}
	else
	{
		// ��������

		// ������ʽ����
		send_test();

		// ͬ����������
		//ssend_test();

		// ��������������
		//bsend_test();

		// ��������
		//Sleep(1000 * (rank - 1)); // ���ڽ���˳���� 1 2 3��Sleep��Ϊ��ʹ����˳��Ҳ��1 2 3
		//rsend_test();
	}

	MPI_Finalize();	// ����MPI����

	return 0;
}


void send_test()
{
	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char str[MSG_SIZE] = { 0 };
	sprintf(str, "use send hello i'm rank %d", rank);
	int strsize = strlen(str);

	// 1. δʵ�ֻ�����Ϣ�����������ƥ��Ľ��ս��̣�����Ϣ���������ս��̺󷵻أ���������ƥ��Ľ��ս�����ȴ���ֱ������Ϣ������ƥ��Ľ��ս���
	// 2. ʵ���˻�����Ϣ������ƥ��Ľ��ս��̣�����Ϣ���������ս��̺󷵻أ���������ƥ��Ľ��ս�������Bsend����
	int rc = MPI_Send(str, strlen(str), MPI_CHAR, MSG_RECV_RANK, MSG_TAG, MPI_COMM_WORLD);	// ������ͷ���,�������ƥ��Ľ��ս����򷵻أ�������ƥ��Ľ��ս�����ȴ�

	if (MPI_SUCCESS == rc)
	{
		double wtime = MPI_Wtime();	// ��ȡ����ʱ��
		double wtick = MPI_Wtick();	// ��ȡʱ�䵥λ(s),����Ǻ�����Ϊ0.001
		cout << "rank " << rank << " send success, times :" << wtick * wtime << endl;
	}
	else
	{
		cout << "rank " << rank << " send failed " << rc << endl;
	}
}

// ͬ���������Ͳ��ԣ��ȴ������귵�أ�sendֻ�Ǵ��������ս��̼����أ�ssend�ȴ����ս��̽�����󷵻�
void ssend_test()
{
	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char str[MSG_SIZE] = { 0 };
	sprintf(str, "use ssend hello i'm rank %d", rank);
	int strsize = strlen(str);


	MPI_Ssend(str, strlen(str), MPI_CHAR, MSG_RECV_RANK, MSG_TAG, MPI_COMM_WORLD);	// ������պ�ŷ���


	double wtime = MPI_Wtime();	// ��ȡ����ʱ��
	double wtick = MPI_Wtick();	// ��ȡʱ�䵥λ(s),����Ǻ�����Ϊ0.001
	cout << "rank " << rank << " ssend success, times :" << wtick * wtime << endl;
}

// ��������������,���ݷ��ڻ������������꼴���أ����ȴ�����
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

	double wtime = MPI_Wtime();	// ��ȡ����ʱ��
	double wtick = MPI_Wtick();	// ��ȡʱ�䵥λ(s),����Ǻ�����Ϊ0.001
	cout << "rank " << rank << " bsend success, times :" << wtick * wtime << endl;
}

// Ԥ����ʽ�������ͣ����ս��̱����Ѿ�����recv״̬�������������ش���
void rsend_test()
{
	int rank = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char str[MSG_SIZE] = { 0 };

	sprintf(str, "use rsend hello i'm rank %d", rank);
	int strsize = strlen(str);


	int rc = MPI_Rsend(str, strsize, MPI_CHAR, MSG_RECV_RANK, MSG_TAG, MPI_COMM_WORLD);	// ������ն˴���recv������ʧ�ܻ�ֱ�����쳣

	if (MPI_SUCCESS == rc) {
		double wtime = MPI_Wtime();	// ��ȡ����ʱ��
		double wtick = MPI_Wtick();	// ��ȡʱ�䵥λ(s),����Ǻ�����Ϊ0.001
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

	// 1. δʵ�ֻ�����Ϣ�����������ƥ��Ľ��ս��̣�����Ϣ���������ս��̺󷵻أ���������ƥ��Ľ��ս�����ȴ���ֱ������Ϣ������ƥ��Ľ��ս���
	// 2. ʵ���˻�����Ϣ������ƥ��Ľ��ս��̣�����Ϣ���������ս��̺󷵻أ���������ƥ��Ľ��ս�������Bsend����
	MPI_Send(send_str, strlen(send_str), MPI_CHAR, source, MSG_TAG, MPI_COMM_WORLD);	// ������ͷ���,�������ƥ��Ľ��ս����򷵻أ�������ƥ��Ľ��ս�����ȴ�


	double wtime = MPI_Wtime();	// ��ȡ����ʱ��
	double wtick = MPI_Wtick();	// ��ȡʱ�䵥λ(s),����Ǻ�����Ϊ0.001
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