#ifndef _TSK_H_
#define _TSK_H_

/**************************************************************

Tasks

Introducion

	This library operate BMP images in a consisten way optimized for hardware implementation in sistem C
	All the aplication resorces are in this file and its source
	The programa can be dividen in th following funcional segments
		- Transformation and array management
			# Blocker -> Image array 2 block array
			# Imager -> Block array 2 image array
			# Copy -> Copy arrays
		- Blockwise operations
			# block_mask
			# block_invt
			# block_rotd
			# block_roti
		- Matrixwise operations

Block array teory of operation

	using the blocker funcion the image is segmented in B*B matrix segments ordered as arry of rows in the data vector being the block operation also in the same way
	to access a particualr block (i,j) of the segmented matrix an offset of (B*B*d), being 'd' equal to (j*w + i) and 'w' the width of the original image

TASKS is free and open source software, distributed
under the MIT licence.

Copyright (c) 2016 Carlos Pastor, Miguel Angel Alfonso

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

**************************************************************/

#include"ext\qdbmp.h"
#include<math.h>

#define B 8

//const float dct8[64] =	  {	0.3536F, 0.3536F, 0.3536F, 0.3536F, 0.3536F, 0.3536F, 0.3536F, 0.3536F,
//							0.4904F, 0.4157F, 0.2778F, 0.0975F,-0.0975F,-0.2778F,-0.4157F,-0.4904F,
//							0.4619F, 0.1913F,-0.1913F,-0.4619F,-0.4619F,-0.1913F, 0.1913F, 0.4619F,
//							0.4157F,-0.0975F,-0.4904F,-0.2778F, 0.2778F, 0.4904F, 0.0975F,-0.4157F,
//							0.3536F,-0.3536F,-0.3536F, 0.3536F, 0.3536F,-0.3536F,-0.3536F, 0.3536F,
//							0.2778F,-0.4904F, 0.0975F, 0.4157F,-0.4157F,-0.0975F, 0.4904F,-0.2778F,
//							0.1913F,-0.4619F, 0.4619F,-0.1913F,-0.1913F, 0.4619F,-0.4916F, 0.1913F,
//							0.0975F,-0.2778F, 0.4157F,-0.4904F, 0.4904F,-0.4157F, 0.2778F,-0.0975F };
//
//const float dct8t[64] = {	0.3536F, 0.4904F, 0.4619F, 0.4157F, 0.3536F, 0.2778F, 0.1913F, 0.0975F,
//							0.3536F, 0.4157F, 0.1913F,-0.0975F,-0.3536F,-0.4904F,-0.4619F,-0.2778F,
//							0.3536F, 0.2778F,-0.1913F,-0.4904F,-0.3536F, 0.0975F, 0.4619F, 0.4157F,
//							0.3536F, 0.0975F,-0.4619F,-0.2778F, 0.3536F, 0.4157F,-0.1913F,-0.4904F,
//							0.3536F,-0.0975F,-0.4619F, 0.2778F, 0.3536F,-0.4157F,-0.1913F, 0.4904F,
//							0.3536F,-0.2778F,-0.1913F, 0.4904F,-0.3536F,-0.0975F, 0.4619F,-0.4157F,
//							0.3536F,-0.4157F, 0.1913F, 0.0975F,-0.3536F, 0.4904F,-0.4619F, 0.2778F,
//							0.3536F,-0.4904F, 0.4619F,-0.4157F, 0.3536F,-0.2778F, 0.1913F,-0.0975F };

const float T[64] = { 0.3536F, 0.3536F, 0.3536F, 0.3536F, 0.3536F, 0.3536F, 0.3536F, 0.3536F,
0.4904F, 0.4157F, 0.2778F, 0.0975F,-0.0975F,-0.2778F,-0.4157F,-0.4904F,
0.4619F, 0.1913F,-0.1913F,-0.4619F,-0.4619F,-0.1913F, 0.1913F, 0.4619F,
0.4157F,-0.0975F,-0.4904F,-0.2778F, 0.2778F, 0.4904F, 0.0975F,-0.4157F,
0.3536F,-0.3536F,-0.3536F, 0.3536F, 0.3536F,-0.3536F,-0.3536F, 0.3536F,
0.2778F,-0.4904F, 0.0975F, 0.4157F,-0.4157F,-0.0975F, 0.4904F,-0.2778F,
0.1913F,-0.4619F, 0.4619F,-0.1913F,-0.1913F, 0.4619F,-0.4916F, 0.1913F,
0.0975F,-0.2778F, 0.4157F,-0.4904F, 0.4904F,-0.4157F, 0.2778F,-0.0975F };

const float Tt[64] = { 0.3536F, 0.4904F, 0.4619F, 0.4157F, 0.3536F, 0.2778F, 0.1913F, 0.0975F,
0.3536F, 0.4157F, 0.1913F,-0.0975F,-0.3536F,-0.4904F,-0.4619F,-0.2778F,
0.3536F, 0.2778F,-0.1913F,-0.4904F,-0.3536F, 0.0975F, 0.4619F, 0.4157F,
0.3536F, 0.0975F,-0.4619F,-0.2778F, 0.3536F, 0.4157F,-0.1913F,-0.4904F,
0.3536F,-0.0975F,-0.4619F, 0.2778F, 0.3536F,-0.4157F,-0.1913F, 0.4904F,
0.3536F,-0.2778F,-0.1913F, 0.4904F,-0.3536F,-0.0975F, 0.4619F,-0.4157F,
0.3536F,-0.4157F, 0.1913F, 0.0975F,-0.3536F, 0.4904F,-0.4619F, 0.2778F,
0.3536F,-0.4904F, 0.4619F,-0.4157F, 0.3536F,-0.2778F, 0.1913F,-0.0975F };

//Transform operationss
int blocker_8_512_sq(UCHAR *cdata, UCHAR *bstring);
int imager_8_512_sq(UCHAR *bstring, UCHAR *cdata);

//Management operations
void copy_U(UCHAR *copied, UCHAR *copy, int d);
void copy_U_F(float *copied, UCHAR *copy, int d);
void copy_F_U(UCHAR *copied, float *copy, int d);
void copy_F(float *copied, float *copy, int d);

//Block operations
void block_mask_8_512_sq(UCHAR *mask, int d);
void block_invt_8_512_sq(UCHAR *bstring, int d);
void block_rotd_8_512_sq(UCHAR *bstring, int d);
void block_roti_8_512_sq(UCHAR *bstring, int d);

//Matrix operations
void vectorMult(double vector1[B*B], double vector2[B*B]);

UCHAR * AxB(UCHAR M1[B * B], UCHAR M2[B * B]);
UCHAR * BxA(UCHAR M1[B * B], UCHAR M2[B * B]);
float * AxB_F(float M1[B * B], const float M2[B * B]);
float * BxA_F(float M1[B * B], const float M2[B * B]);
void QnD_TATt(float A[B * B]);
void QnD_TtAT(float A[B * B]);
void MxC(UCHAR M1[B * B], float cns);
void Mask(float A[B*B], UCHAR mask[B*B]);
void transVector(double vector1[B*B]);
void createMask(int precision);
void applyMask(double vector1[B*B]);

#endif