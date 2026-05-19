#include "image_transform.h"

#define SW_BREAKPOINT asm(" SWBP 0 ");

int main(void)
{
	bmp inImg;
	char outFileName[100];
	int Fa_contrast = 2;
	int Fb_contrast = -128;
	int Fa_brightness = 1;
	int Fb_brightness = 50;

	printf("\r\nImage LinerTrans Application.\r\n");

	printf("Reading the image......\n");

	inImg = ReadBmp("../Image/TL_C6748_Gray.bmp");
	printf("Read successfully.\n");

	sprintf(outFileName, "../Image/Out_LinerTrans_Contrast_Fa%d_Fb%d.bmp", Fa_contrast, Fb_contrast);
	LinerTrans(&inImg, Fa_contrast, Fb_contrast, outFileName);
	printf("LinerTrans Contrast processing is completed.\n");
	printf("Write successfully.\n");

	sprintf(outFileName, "../Image/Out_LinerTrans_Brightness_Fa%d_Fb%d.bmp", Fa_brightness, Fb_brightness);
	LinerTrans(&inImg, Fa_brightness, Fb_brightness, outFileName);
	printf("LinerTrans Brightness processing is completed.\n");
	printf("Write successfully.\n");

	sprintf(outFileName, "../Image/Out_ImageReverse.bmp");
	ImageReverse(&inImg, outFileName);
	printf("ImageReverse processing is completed.\n");
	printf("Write successfully.\n");

	sprintf(outFileName, "../Image/Out_RGB2Gray.bmp");
	RGB2Gray("../Image/LCD.bmp", outFileName);
	printf("RGB2Gray processing is completed.\n");
	printf("Write successfully.\n");

	sprintf(outFileName, "../Image/Out_Rotate_30deg.bmp");
	Rotate("../Image/LCD.bmp", 3.1415927f / 6, outFileName);
	printf("Rotate processing is completed.\n");
	printf("Write successfully.\n");

	SW_BREAKPOINT;

	while (1)
		;
}
