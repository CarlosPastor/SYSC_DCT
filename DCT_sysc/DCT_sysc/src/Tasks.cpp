#include "Tasks.h"

//Function deff

/*
	Takes the array with the image pixel values ordered in consecutive rows and transform it into and array of matrix sub-blocks of dimension B from the original image
*/
int blocker_8_512_sq(UCHAR *cdata, UCHAR *bstring)
{
	const int n = 512, m = 512;
	int block_pattern[B*B], c = 0;
	//Generates a vector that describes the ubication of a 8x8 section on the image having its data distributed in a unidimensional vector ordered in rows
	for (int i = 0; i < 8; i++)
		for (int j = i*n; j - i*n < 8; j++)		//Using n here is dependent of the oder chosen for the vector, in this case is rows so correspond to n
			block_pattern[c++] = j;
	//calculates the ofsets to extract the blocks from the vector and reorder the data in a block wise way of substrings of B^2 bits
	for (int i = 0; i < (m / B); i++)
		for (int j = 0; j < (n / B); j++)
			for (int k = 0; k < B*B; k++)
				bstring[k + j*(B*B) + i*(n / B)*(B*B)] = cdata[block_pattern[k] + B*j + (n / B)*B*B*i];

	return (m / B)*(m / B);
	//Retunrs the number of blocks to ease operation
}

/*
	Takes an array of row arrays of matrix sub-blocks of dimension B from the original image matrix and transform it into an array with the pixel values ordered in consecutive rows  
*/
int imager_8_512_sq(UCHAR *bstring, UCHAR *cdata)
{
	const int n = 512, m = 512;
	int block_pattern[B*B], c = 0;

	//Generates a vector that describes the ubication of a 8x8 section on the image having its data distributed in a unidimensional vector ordered in rows
	for (int i = 0; i < 8; i++)
		for (int j = i*n; j - i*n < 8; j++)		//Using n here is dependent of the oder chosen for the vector, in this case is rows so correspond to n
			block_pattern[c++] = j;

	//calculates the ofsets to extract the blocks from the vector and reorder the data in a block wise way of substrings of B^2 bits
	for (int i = 0; i < (m / B); i++)
		for (int j = 0; j < (n / B); j++)
			for (int k = 0; k < B*B; k++)
				cdata[block_pattern[k] + B*j + (n / B)*B*B*i] = bstring[k + j*(B*B) + i*(n / B)*(B*B)];

	return (m / B)*(m / B);
	//Retunrs the number of blocks to ease operation
}

/*
	copies d values of the first parameter on to the second parameter being the parameters UCHAR vector pointers with dimension abobe d
*/
void copy_U(UCHAR *copied, UCHAR *copy, int d)
{
	for (int i = 0; i < d; i++)
		copy[i] = copied[i];
}
void copy_U_F(float *copied, UCHAR *copy, int d)
{
	for (int i = 0; i < d; i++)
		copy[i] = (UCHAR)round(copied[i]);
}
void copy_F_U(UCHAR *copied, float *copy, int d)
{
	for (int i = 0; i < d; i++)
		copy[i] = (float)(copied[i]);
}
void copy_F(float *copied, float *copy, int d)
{
	for (int i = 0; i < d; i++)
		copy[i] = copied[i];
}

/*
	generates a mask consisting in a matrix of dimension B with ones in the superior corner untill the diagonal d, the generated matrix is then copied into the UCHAR vector pointer passed as first parameter
*/
void block_mask_8_512_sq(UCHAR *mask, int d)
{
	int c = 0;

	for (int i = 0; i < B; i++)
		for (int j = 0; j < B; j++)
			if (d - i > j)
				mask[c++] = 1;
			else
				mask[c++] = 0;
}

/*
	Inverts the block content on the main diagonal, d selects the block of the array of row arrays of matrix sub-blocks of dimension B from the original image 
*/
void block_invt_8_512_sq(UCHAR *bstring, int d) //diagonal primaria
{
	UCHAR TMP[B*B];

	copy_U(&bstring[B*B*d], TMP, B*B);

	for (int i = 0; i < B; i++)
		for (int j = 0; j < B; j++)
			bstring[i*B + j + (B*B*d)] = TMP[i + j*B];

}

/*
	Rotates the d block of the array of row arrays of matrix sub-blocks of dimension B from the original image, clockwise rotation
*/
void block_rotd_8_512_sq(UCHAR *bstring, int d)
{
	UCHAR TMP[B*B];

	copy_U(&bstring[B*B*d], TMP, B*B);

	for (int i = 0; i < B; i++)
		for (int j = 0; j < B; j++)
			bstring[j*B + 7 - i + (B*B*d)] = TMP[j + i*B];

}

/*
	Rotates the d block of the array of row arrays of matrix sub-blocks of dimension B from the original image, counterclockwise rotation
*/
void block_roti_8_512_sq(UCHAR *bstring, int d)
{

	UCHAR TMP[B*B];

	copy_U(&bstring[B*B*d], TMP, B*B);

	for (int i = 0; i < B; i++)
		for (int j = 0; j < B; j++)
			bstring[(7 - j)*B + i + (B*B*d)] = TMP[j + i*B];

}


// TEMP /////////////////////////////////////////////////////////////////////////////////////////////////

	double vectorResult[B*B];
	double vectorMask[B*B];
	double vectorTrans[B*B];
	double mask[B*B];

/*
Función que va a multiplicar las dos funciones que le pases y las guarda en matrixResult
*/
void vectorMult(double vector1[B*B], double vector2[B*B]) {
	//double matrixResult[B][B];
	for (int k = 0; k < B; k++) {
		for (int i = 0; i < B; i++) {
			vectorResult[i + k*B] = 0;
			for (int j = 0; j < B; j++) {
				vectorResult[k*B + i] += vector1[j + k*B] * vector2[i + j*B];
			}
		}
	}
}
//Modifies ther first parameter pushing in the result, correspond to A
UCHAR * AxB(UCHAR M1[B * B], UCHAR M2[B * B])
{
	UCHAR TMP[B * B];
	for (int k = 0; k < B; k++) {
		for (int i = 0; i < B; i++) {
			TMP[i + k*B] = 0;
			for (int j = 0; j < B; j++) {
				TMP[k*B + i] += M1[j + k*B] * M2[i + j*B];
			}
		}
	}
	copy_U(TMP, M1, B * B);
	return M1; //Returns the pointer of the firts parameter variable that also contains the result to allow recursive calls
}
//Modifies ther first parameter pushing in the result, correspond to A
UCHAR * BxA(UCHAR M1[B * B], UCHAR M2[B * B])
{
	UCHAR TMP[B * B];
	for (int k = 0; k < B; k++) {
		for (int i = 0; i < B; i++) {
			TMP[i + k*B] = 0;
			for (int j = 0; j < B; j++) {
				TMP[k*B + i] += M2[j + k*B] * M1[i + j*B];
			}
		}
	}
	copy_U(TMP, M1, B * B);
	return M1; //Returns the pointer of the firts parameter variable that also contains the result to allow recursive calls
}
//F variant accepts float as second matrix
//Modifies ther first parameter pushing in the result, correspond to A
float * AxB_F(float M1[B * B], const float M2[B * B])
{
	float TMP[B * B];
	for (int k = 0; k < B; k++) {
		for (int i = 0; i < B; i++) {
			TMP[i + k*B] = 0;
			for (int j = 0; j < B; j++) {
				TMP[k*B + i] += ((float)M1[j + k*B]) * M2[i + j*B];
			}
		}
	}
	copy_F(TMP, M1, B * B);
	return M1; //Returns the pointer of the firts parameter variable that also contains the result to allow recursive calls
}
// F variant accepts float as second matrix
//Modifies ther first parameter pushing in the result, correspond to A
float * BxA_F(float M1[B * B], const float M2[B * B])
{
	float TMP[B * B];
	for (int k = 0; k < B; k++) {
		for (int i = 0; i < B; i++) {
			TMP[i + k*B] = 0;
			for (int j = 0; j < B; j++) {
				TMP[k*B + i] += (M2[j + k*B]) * M1[i + j*B];
			}
		}
	}
	copy_F(TMP, M1, B * B);
	return M1; //Returns the pointer of the firts parameter variable that also contains the result to allow recursive calls
}

void MxC(UCHAR M1[B * B], float cns)
{
	UCHAR TMP[B * B];
	for (int i = 0; i < B; i++) {
		for (int j = 0; j < B; j++) {
			TMP[i + j*B] = (UCHAR)floor((float)M1[i + j*B] * cns);
		}
	}
	copy_U(TMP, M1, B * B);
}

void QnD_TATt(float A[B * B])
{
	BxA_F(A, T);
}
void QnD_TtAT(float A[B * B])
{
	BxA_F(AxB_F(A, T), Tt);
}

/*
Función que va a transponer el vector que le pases y lo va a guardar en matrixTrans
*/
void transVector(double vector1[B*B]) {
	//double matrixTrans[B][B];

	for (int i = 0; i < B; i++) {
		for (int j = 0; j < B; j++) {
			vectorTrans[i*B + j] = vector1[i + j*B];
		}
	}
}

/*
Función que te crea la matriz máscara. Con la variable precisión dices el número de "filas" que coges:
- Con valor 0, pones a 1 el valor 0,0 y el resto a 0
- Con valor 1, pones a 1 los valores 0,0; 0,1; 1,0 y el resto a 0
- ...
*/
void createMask(int precision) {
	//Para inicializar todos los valores de mask a 0
	for (int i = 0; i < B; i++) {
		for (int j = 0; j < B; j++) {
			mask[i*B + j] = 0;
		}
	}

	for (int i = 0; i < precision; i++) {
		for (int j = 0; j < precision; j++) {
			mask[i*B + j] = 1;
		}
	}
	mask[precision] = 1;
	mask[B*precision] = 1;
}

/*
Función que aplica la máscara y se guarda en matrixMask. Es tan tonta como comprobar si el valor de la máscara
es 1 o 0 y si es 1 respetas el valor y si es 0 pones la matriz resultante a 0.
*/
void applyMask(double vector1[B*B]) {
	for (int i = 0; i < B; i++) {
		for (int j = 0; j < B; j++) {
			if (mask[i*B + j] == 1) vectorMask[i*B + j] = vector1[i*B + j];
			else vectorMask[i*B + j] = 0;
		}
	}
}

void Mask(float A[B*B], UCHAR mask[B*B]) {
	for (int p = 0; p < B*B; p++)
		A[p] *= mask[p];
}