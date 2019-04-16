/*×Ô¶¨Òåreduce*/

#include <iostream>
#include "mpi.h"
#include <Windows.h>

using namespace std;

typedef struct {
	double real, imag;
} Complex;
/* the user-defined function
 */
void MPIAPI myProd(Complex *in, Complex *inout, int *len,
	MPI_Datatype *dptr)
{
	int i;
	Complex c;
	for (i = 0; i < *len; ++i) {
		c.real = inout->real*in->real -
			inout->imag*in->imag;
		c.imag = inout->real*in->imag +
			inout->imag*in->real;
		*inout = c;
		in++; inout++;
	}
}
/* and, to call it...
 */

int main_red(int argc, char *argv[]) {

	MPI_Init(&argc, &argv);

	/* each process has an array of 100 Complexes
		 */
	Complex a[100], answer[100];
	MPI_Op myOp;
	MPI_Datatype ctype;
	/* explain to MPI how type Complex is defined
		 */
	MPI_Type_contiguous(2, MPI_DOUBLE, &ctype);
	MPI_Type_commit(&ctype);
	/* create the complex-product user-op
	 */
	MPI_Op_create((MPI_User_function *)myProd, TRUE, &myOp);

	int root = 0;
	MPI_Reduce(a, answer, 100, ctype, myOp, root, MPI_COMM_WORLD);

	cout << "answer " << endl;
	for (size_t i = 0; i < 100; i++)
	{
		cout << answer[i].real << " " << answer[i].imag << " ";
	}
	cout << endl;

	/* At this point, the answer, which consists of 100 Complexes,
	 * resides on process root
	 */

	MPI_Finalize();

	return 0;
}