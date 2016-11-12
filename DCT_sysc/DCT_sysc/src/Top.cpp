#include<stdio.h>
#include<math.h>
#include"ext\qdbmp.h"
#include"Tasks.h"


//Development glogal dependecies
BMP*    bmp;
UCHAR   r, g, b;
UINT    width, height;
UINT    h, w;
//--

//special
int debug_outputBPM(UCHAR * image_tonal_data, const char * app);

//too specialized!!
UCHAR image_tonal_data_bss[512 * 512];		//8bit...+250kByte
UCHAR image_tonal_data_blk[512 * 512];		//8bit...+250kByte
float image_tonal_data_csn[512 * 512];		//8bit...+250kByte

int main(int argc, char* argv[])
{
	
	char buff[100];
	sprintf(buff, "res/lena512.bmp");		//target img

	UCHAR cmp_mask[64];							//Mask vetor (pre-generated?)

	int nblocks = 0;

	// Load file and extract data /////////////////////////////////////////////////////////////////////////////////////// 

	/* Read an image file */
	bmp = BMP_ReadFile(buff);
	/* If an error has occurred, notify and exit */
	BMP_CHECK_ERROR(stderr, -1);
	/* Get image's dimensions */
	width = BMP_GetWidth(bmp);
	height = BMP_GetHeight(bmp);
	/* Iterate through all the image's pixels */
	for (h = 0; h < height; ++h)
	{
		for (w = 0; w < width; ++w)
		{
			/* Get pixel's RGB values */
			BMP_GetPixelIndex(bmp, w, h, &image_tonal_data_bss[w + width*h]);

		}
	}

	//  Make blocks ///////////////////////////////////////////////////////////////////////////////////////

	//blocker operation
	blocker_8_512_sq(image_tonal_data_bss,image_tonal_data_blk);

	debug_outputBPM(image_tonal_data_blk, "blk");

	//blocker operation
	nblocks = blocker_8_512_sq(image_tonal_data_bss, image_tonal_data_blk);
	copy_F_U(image_tonal_data_blk, image_tonal_data_csn, 512*512);

	// Input modifications ///////////////////////////////////////////////////////////////////////////////////////

	//stripes and mask 
	block_mask_8_512_sq(cmp_mask, 6);

	for (int n = 0; n < nblocks; n++)
	{
		if (n % 2)
			MxC(&image_tonal_data_blk[n * B * B], -1); //image striper
		else
			BxA(&image_tonal_data_blk[n * B * B], cmp_mask); //image striper
	}

	//imager operation
	imager_8_512_sq(image_tonal_data_blk, image_tonal_data_bss);
	debug_outputBPM(image_tonal_data_bss, "pst");

	////////////////////////////////////////////////////////////////////////////////////////

	//

	////////////////////////////////////////////////////////////////////////////////////////

	copy_U_F(image_tonal_data_csn, image_tonal_data_blk, 512 * 512);
	imager_8_512_sq(image_tonal_data_blk, image_tonal_data_bss);
	debug_outputBPM(image_tonal_data_bss, "pre");

	for (int n = 0; n < nblocks; n++)
	{
		QnD_TATt(&image_tonal_data_csn[n * B * B]);
	}

	copy_U_F(image_tonal_data_csn, image_tonal_data_blk, 512 * 512);
	imager_8_512_sq(image_tonal_data_blk, image_tonal_data_bss);
	debug_outputBPM(image_tonal_data_bss, "csnt");

	for (int n = 0; n < nblocks; n++)
	{
		QnD_TATt(&image_tonal_data_csn[n * B * B]);
	}
	
	copy_U_F(image_tonal_data_csn, image_tonal_data_blk, 512 * 512);
	imager_8_512_sq(image_tonal_data_blk, image_tonal_data_bss);
	debug_outputBPM(image_tonal_data_bss, "csnti");

	////////////////////////////////////////////////////////////////////////////////////////

	/* Free all memory allocated for the image */
	BMP_Free(bmp);

	return 0;
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