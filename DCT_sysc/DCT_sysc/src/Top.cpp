#include<stdio.h>
#include<math.h>
#include"ext\qdbmp.h"


//Development glogal dependecies
BMP*    bmp;
UCHAR   r, g, b;
UINT    width, height;
UINT    h, w;
//--

int main(int argc, char* argv[])
{
	
	char buff[100];
	sprintf(buff, "res/lena512.bmp");		//target img

	UCHAR image_tonal_data_pre[263000];		//8bit...+250kByte
	UCHAR image_tonal_data_blk[263000];		//8bit...+250kByte
	UCHAR image_tonal_data_pst[263000];		//8bit...+250kByte
	UCHAR cmp_mask[64];		//8bit...+250kByte

	int nblocks = 0;
 
	/* Read an image file */
	bmp = BMP_ReadFile(buff);
	BMP_CHECK_ERROR(stderr, -1); /* If an error has occurred, notify and exit */

								 /* Get image's dimensions */
	width = BMP_GetWidth(bmp);
	height = BMP_GetHeight(bmp);

	int blocker_8_512_sq(UCHAR *cdata, UCHAR *bstring);
	int imager_8_512_sq(UCHAR *bstring, UCHAR *cdata);
	void copy(UCHAR *copied, UCHAR *copy);
	void block_mask_8_512_sq(UCHAR *mask, int d);
	void block_invt_8_512_sq(UCHAR *bstring, int d);
	void block_rotd_8_512_sq(UCHAR *bstring, int d);
	void block_roti_8_512_sq(UCHAR *bstring, int d);
	int debug_outputBPM(UCHAR * image_tonal_data, const char * app);
	

	/////////////////////////////////////////////////////////////////////////////////////// Load file and extract data

	/* Iterate through all the image's pixels */
	for (h = 0; h < height; ++h)
	{
		for (w = 0; w < width; ++w)
		{
			/* Get pixel's RGB values */
			BMP_GetPixelIndex(bmp, w, h, &image_tonal_data_pre[w + width*h]);

		}
	}

	/////////////////////////////////////////////////////////////////////////////////////// Make blocks

	//blocker operation
	blocker_8_512_sq(image_tonal_data_pre,image_tonal_data_blk);

	debug_outputBPM(image_tonal_data_blk, "blk");

	//blocker operation
	nblocks = blocker_8_512_sq(image_tonal_data_pre, image_tonal_data_blk);



	/////////////////////////////////////////////////////////////////////////////////////// MOD

	//stripes and mask 
	block_mask_8_512_sq(cmp_mask, 6);

	for (int n = 0; n < nblocks; n++)
		for (int p = 0; p < 64; p++)
			if (n % 2)
				image_tonal_data_blk[p + n * 64] /= 2; //image striper
			else
				image_tonal_data_blk[p + n * 64] *= cmp_mask[p]; //image striper

	//imager operation
	imager_8_512_sq(image_tonal_data_blk, image_tonal_data_pst);

	debug_outputBPM(image_tonal_data_pst, "pst");

	////////////////////////////////////////////////////////////////////////////////////////

	for (int n = 0; n < nblocks; n++)
		block_invt_8_512_sq(image_tonal_data_blk, n);
																
	imager_8_512_sq(image_tonal_data_blk, image_tonal_data_pst);

	debug_outputBPM(image_tonal_data_pst, "psti");

	////////////////////////////////////////////////////////////////////////////////////////

	for (int n = 0; n < nblocks; n++)
		block_rotd_8_512_sq(image_tonal_data_blk, n);
	for (int n = 0; n < nblocks; n++)
		block_roti_8_512_sq(image_tonal_data_blk, n);

	imager_8_512_sq(image_tonal_data_blk, image_tonal_data_pst);

	debug_outputBPM(image_tonal_data_pst, "pstr");

		////////////////////////////////////////////////////////////////////////////////////////


	/* Free all memory allocated for the image */
	BMP_Free(bmp);

	return 0;
}

//Function deff
int blocker_8_512_sq(UCHAR *cdata, UCHAR *bstring)
{
	const int n = 512, m = 512, b = 8;
	int block_pattern[b*b], c = 0;
	//Generates a vector that describes the ubication of a 8x8 section on the image having its data distributed in a unidimensional vector ordered in rows
	for (int i = 0; i < 8; i++)
		for (int j = i*n; j - i*n < 8; j++)		//Using n here is dependent of the oder chosen for the vector, in this case is rows so correspond to n
			block_pattern[c++] = j;		
	//calculates the ofsets to extract the blocks from the vector and reorder the data in a block wise way of substrings of b^2 bits
	for (int i = 0; i < (m / b); i++)
		for (int j = 0; j < (n / b); j++)
			for (int k = 0; k < b*b; k++)
				bstring[k + j*(b*b) + i*(n / b)*(b*b)] = cdata[block_pattern[k] + b*j + (n / b)*b*b*i];

	return (m / b)*(m / b);
	//Retunrs the number of blocks to ease operation
}

int imager_8_512_sq(UCHAR *bstring, UCHAR *cdata)
{
	const int n = 512, m = 512, b = 8;
	int block_pattern[b*b], c = 0;
	//Generates a vector that describes the ubication of a 8x8 section on the image having its data distributed in a unidimensional vector ordered in rows
	for (int i = 0; i < 8; i++)
		for (int j = i*n; j - i*n < 8; j++)		//Using n here is dependent of the oder chosen for the vector, in this case is rows so correspond to n
			block_pattern[c++] = j;
	//calculates the ofsets to extract the blocks from the vector and reorder the data in a block wise way of substrings of b^2 bits
	for (int i = 0; i < (m / b); i++)
		for (int j = 0; j < (n / b); j++)
			for (int k = 0; k < b*b; k++)
				cdata[block_pattern[k] + b*j + (n / b)*b*b*i] = bstring[k + j*(b*b) + i*(n / b)*(b*b)];

	return (m / b)*(m / b);
	//Retunrs the number of blocks to ease operation
}

void copy(UCHAR *copied, UCHAR *copy, int d)
{
	for (int i = 0; i < d; i++)
		copy[i] = copied[i];
}

void block_mask_8_512_sq(UCHAR *mask, int d)
{
	const int b = 8;
	int c = 0;

	for (int i = 0; i < b; i++)
		for (int j = 0; j < b; j++)
			if (d - i > j)
				mask[c++] = 1;
			else
				mask[c++] = 0;
}

void block_invt_8_512_sq(UCHAR *bstring, int d) //diagonal primaria
{
	const int b = 8;
	UCHAR TMP[b*b];

	copy(&bstring[b*b*d], TMP, b*b);

	for (int i = 0; i < b; i++)
		for (int j = 0; j < b; j++)
			bstring[i*b + j + (b*b*d)] = TMP[i + j*b];

}

void block_rotd_8_512_sq(UCHAR *bstring, int d)
{
	const int b = 8;
	UCHAR TMP[b*b];

	copy(&bstring[b*b*d], TMP, b*b);

	for (int i = 0; i < b; i++)
		for (int j = 0; j < b; j++)
			bstring[j*b + 7 - i + (b*b*d)] = TMP[j + i*b];

}

void block_roti_8_512_sq(UCHAR *bstring, int d)
{
	const int b = 8;
	UCHAR TMP[b*b];

	copy(&bstring[b*b*d], TMP, b*b);

	for (int i = 0; i < b; i++)
		for (int j = 0; j < b; j++)
			bstring[(7 - j)*b + i + (b*b*d)] = TMP[j + i*b];

}

int debug_outputBPM(UCHAR * image_tonal_data, const char * app)
{

	for (h = 0; h < height; ++h)
	{
		for (w = 0; w < width; ++w)
		{
			/* Set pixel's RGB values */
			BMP_SetPixelIndex(bmp, w, h, image_tonal_data[w + width*h]);

		}
	}

	/* Save result */
	char buff[50];
	sprintf(buff, "res/lena512_%s.bmp", app);
	BMP_WriteFile(bmp, buff);
	BMP_CHECK_ERROR(stderr, -2);
	return 0;

}