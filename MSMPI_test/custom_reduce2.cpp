/*×Ô¶¨Òåreduce2*/

#include <iostream>
#include "mpi.h"
#include <Windows.h>

using namespace std;

/* the user-defined function
 */
void MPIAPI mySum(int *in, int* inout, int *len,
	MPI_Datatype *dptr)
{
	for (size_t i = 0; i < *len; i++)
	{
		cout << in[i] << " ";
	}
	cout << endl;

	int i;
	int sum = 0;
	for (i = 0; i < *len; ++i) {
		sum += in[i];
	}
	inout[0] += sum;
}


int main_red2(int argc, char *argv[]) {

	MPI_Init(&argc, &argv);

	/* each process has an array of 100 Complexes
		 */

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int a[100];

	for (size_t i = 0; i < 100; i++)
	{
		a[i] = 10 * rank + i;
	}

	int answer[100];
	MPI_Op myOp;
	MPI_Datatype ctype;

	/* explain to MPI how type Complex is defined
		 */
		 //MPI_Type_contiguous(2, MPI_DOUBLE, &ctype);
		 //MPI_Type_commit(&ctype);
		 /* create the complex-product user-op
		  */

	MPI_Op_create((MPI_User_function *)mySum, TRUE, &myOp);

	int root = 0;
	MPI_Reduce(a, answer, 100, MPI_INT, myOp, root, MPI_COMM_WORLD);

	if (rank == root) {
		cout << "answer = " << answer[0] << endl;
	}

	/* At this point, the answer, which consists of 100 Complexes,
	 * resides on process root
	 */

	MPI_Finalize();


	return 0;
}