#define SIZE 3
#ifdef DEBUG
	#define PDEBUG(X) printf(X)
	#define PDEBUG1(X, X1) printf(X, X1)
	#define PDEBUG2(X, X1, X2) printf(X, X1, X2)
	#define PDEBUG3(X, X1, X2) printf(X, X1, X2, X3)
#else
	#define PDEBUG(X) 
	#define PDEBUG1(X, X1)
	#define PDEBUG2(X, X1, X2)
	#define PDEBUG3(X, X1, X2)
#endif

#ifdef VECTORIZE
	#define INC 4
	#define VECTORIZE_THIS #pragma vector aligned
#else
	#define INC 1
	#define VECTORIZE_THIS
#endif

/*
float A[SIZE*SIZE] = {
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8
};

float B[SIZE*SIZE] = {
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8,
	1, 2, 3, 4, 5, 6, 7, 8
};*/

float A[SIZE*SIZE] = {
	1, 2, 3,
	1, 2, 3,
	1, 2, 3,
};

float B[SIZE*SIZE] = { // easy solution: create a new transposed m-bugga
	1, 2, 3, // 1, 1, 1
	1, 2, 3, // 2, 2, 2  
	1, 2, 3  // 3, 3, 3
};

float TRANS_B[SIZE*SIZE];// t([i,j], max) == [j,max-i]

float C[SIZE*SIZE];

float EXPECT_C[SIZE*SIZE] = {
	/*0,0*/ (1*1 + 2*1+ 3*1), /*0,1*/ (1*2+2*2+3*2), /*0,2*/ (1*3+2*3+3*3),
	/*1,0*/ (1*1 + 2*1+ 3*1), /*1,1*/ (1*2+2*2+3*2), /*1,2*/ (1*3+2*3+3*3),
	/*2,0*/ (1*1 + 2*1+ 3*1), /*2,1*/ (1*2+2*2+3*2), /*2,2*/ (1*3+2*3+3*3)
};
