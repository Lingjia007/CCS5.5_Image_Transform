#ifndef _IMAGE_TRANSFORM_H_
#define _IMAGE_TRANSFORM_H_

#include <stdio.h>
#include <stdlib.h>
#include "mathlib.h"

typedef struct tagRGBQUAD
{
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPFILEHEADER
{
	unsigned short int bfType;
	unsigned int bfSize;
	unsigned short int bfReserverd1;
	unsigned short int bfReserverd2;
	unsigned int bfbfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	int biSize;
	int biWidth;
	int biHeight;
	short int biPlanes;
	short int biBitcount;
	int biCompression;
	int biSizeImage;
	int biXPelsPermeter;
	int biYPelsPermeter;
	int biClrUsed;
	int biClrImportant;
} BITMAPINFOHEADER;

typedef struct
{
	BITMAPFILEHEADER file;
	BITMAPINFOHEADER info;
	RGBQUAD *pColorTable;
	unsigned char *imgBuf;
} bmp;

bmp ReadBmp(char *bmpName);
int SaveBmp(char *bmpName, bmp *m);
int LinerTrans(bmp *m, int Fa, int Fb, char *outBmpName);
int ImageReverse(bmp *m, char *outBmpName);
int RGB2Gray(char *inBmpName, char *outBmpName);
int Rotate(char *inBmpName, float fAngle, char *outBmpName);

#endif
