#include "image_transform.h"

unsigned char *newBmpBuf;
unsigned char newpColorTable[256 * 4];

bmp ReadBmp(char *bmpName)
{
	bmp m;
	FILE *fp;
	unsigned char *BmpBuf;
	int i;
	int bmpWidth;
	int bmpHeight;
	int biBitCount;
	int lineByte;

	if ((fp = fopen(bmpName, "rb")) == NULL)
	{
		printf("can't open the bmp imgae.\n ");
		exit(0);
	}
	else
	{
		fread(&m.file.bfType, sizeof(char), 2, fp);
		fread(&m.file.bfSize, sizeof(int), 1, fp);
		fread(&m.file.bfReserverd1, sizeof(short int), 1, fp);
		fread(&m.file.bfReserverd2, sizeof(short int), 1, fp);
		fread(&m.file.bfbfOffBits, sizeof(int), 1, fp);
		fread(&m.info.biSize, sizeof(int), 1, fp);
		fread(&m.info.biWidth, sizeof(int), 1, fp);
		fread(&m.info.biHeight, sizeof(int), 1, fp);
		fread(&m.info.biPlanes, sizeof(short), 1, fp);
		fread(&m.info.biBitcount, sizeof(short), 1, fp);
		fread(&m.info.biCompression, sizeof(int), 1, fp);
		fread(&m.info.biSizeImage, sizeof(int), 1, fp);
		fread(&m.info.biXPelsPermeter, sizeof(int), 1, fp);
		fread(&m.info.biYPelsPermeter, sizeof(int), 1, fp);
		fread(&m.info.biClrUsed, sizeof(int), 1, fp);
		fread(&m.info.biClrImportant, sizeof(int), 1, fp);
	}

	bmpWidth = m.info.biWidth;
	bmpHeight = m.info.biHeight;
	biBitCount = m.info.biBitcount;

	lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;

	if (biBitCount == 8)
	{
		m.pColorTable = (RGBQUAD *)malloc(256 * 4);
		fread(m.pColorTable, sizeof(RGBQUAD), 256, fp);
	}

	m.imgBuf = (unsigned char *)malloc(m.info.biSizeImage);
	BmpBuf = (unsigned char *)malloc(m.info.biSizeImage);

	fread(BmpBuf, lineByte * bmpHeight, 1, fp);

	for (i = 0; i < bmpHeight; i++)
		memcpy((void *)(m.imgBuf + lineByte * i),
			   (void *)(BmpBuf + lineByte * (bmpHeight - i - 1)), lineByte);
	fclose(fp);

	return m;
}

int SaveBmp(char *bmpName, bmp *m)
{
	unsigned char *BmpBuf;
	int i;
	int bmpWidth;
	int bmpHeight;
	int biBitCount;
	RGBQUAD *pColorTable;

	bmpWidth = m->info.biWidth;
	bmpHeight = m->info.biHeight;
	biBitCount = m->info.biBitcount;
	pColorTable = m->pColorTable;

	if (!m->imgBuf)
		return 0;

	int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;
	FILE *fp = fopen(bmpName, "wb");
	if (fp == 0)
		return 0;

	fwrite(&m->file.bfType, sizeof(short), 1, fp);
	fwrite(&m->file.bfSize, sizeof(int), 1, fp);
	fwrite(&m->file.bfReserverd1, sizeof(short int), 1, fp);
	fwrite(&m->file.bfReserverd2, sizeof(short int), 1, fp);
	fwrite(&m->file.bfbfOffBits, sizeof(int), 1, fp);
	fwrite(&m->info.biSize, sizeof(int), 1, fp);
	fwrite(&m->info.biWidth, sizeof(int), 1, fp);
	fwrite(&m->info.biHeight, sizeof(int), 1, fp);
	fwrite(&m->info.biPlanes, sizeof(short), 1, fp);
	fwrite(&m->info.biBitcount, sizeof(short), 1, fp);
	fwrite(&m->info.biCompression, sizeof(int), 1, fp);
	fwrite(&m->info.biSizeImage, sizeof(int), 1, fp);
	fwrite(&m->info.biXPelsPermeter, sizeof(int), 1, fp);
	fwrite(&m->info.biYPelsPermeter, sizeof(int), 1, fp);
	fwrite(&m->info.biClrUsed, sizeof(int), 1, fp);
	fwrite(&m->info.biClrImportant, sizeof(int), 1, fp);

	if (biBitCount == 8)
		fwrite(pColorTable, sizeof(RGBQUAD) * 256, 1, fp);

	BmpBuf = (unsigned char *)malloc(m->info.biSizeImage);
	for (i = 0; i < bmpHeight; i++)
		memcpy((void *)(BmpBuf + lineByte * i),
			   (void *)(m->imgBuf + lineByte * (bmpHeight - i - 1)), lineByte);

	fwrite(BmpBuf, bmpHeight * lineByte, 1, fp);

	fclose(fp);

	return 1;
}

int LinerTrans(bmp *m, int Fa, int Fb, char *outBmpName)
{
	int i, j;
	bmp newm;
	int bmpWidth;
	int bmpHeight;
	int biBitCount;
	int lineByte;
	int newBmpWidth;
	int newBmpHeight;
	int newLineByte;
	int temp;

	bmpWidth = m->info.biWidth;
	bmpHeight = m->info.biHeight;
	biBitCount = m->info.biBitcount;

	lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;

	newBmpWidth = bmpWidth;
	newBmpHeight = bmpHeight;
	newLineByte = (newBmpWidth * biBitCount / 8 + 3) / 4 * 4;
	newBmpBuf = (unsigned char *)malloc(newLineByte * newBmpHeight);

	for (i = 0; i < newBmpHeight; i++)
	{
		for (j = 0; j < newBmpWidth; j++)
		{
			temp = (*(unsigned char *)(m->imgBuf + i * lineByte + j)) * Fa + Fb;

			if (temp > 255)
			{
				*(unsigned char *)(newBmpBuf + i * newLineByte + j) = 0xFF;
			}
			else if (temp < 0)
			{
				*(unsigned char *)(newBmpBuf + i * newLineByte + j) = 0x00;
			}
			else
			{
				*(unsigned char *)(newBmpBuf + i * newLineByte + j) = temp;
			}
		}
	}

	newm.file.bfType = 0x4d42;
	newm.file.bfSize = 54 + 246 * 4 +
					   newLineByte * newBmpHeight;
	newm.file.bfReserverd1 = 0;
	newm.file.bfReserverd2 = 0;
	newm.file.bfbfOffBits = 54 + 256 * 4;
	newm.info.biSize = 40;
	newm.info.biWidth = newBmpWidth;
	newm.info.biHeight = newBmpHeight;
	newm.info.biPlanes = 1;
	newm.info.biBitcount = 8;
	newm.info.biCompression = 0;
	newm.info.biSizeImage = newLineByte * newBmpHeight;
	newm.info.biXPelsPermeter = 0;
	newm.info.biYPelsPermeter = 0;
	newm.info.biClrUsed = 256;
	newm.info.biClrImportant = 256;

	memcpy((void *)(newpColorTable), (void *)(m->pColorTable), 256 * 4);
	newm.pColorTable = (RGBQUAD *)newpColorTable;

	newm.imgBuf = newBmpBuf;

	printf("Writing the image......\n");
	return SaveBmp(outBmpName, &newm);
}

int Rotate(char *inBmpName, float fAngle, char *outBmpName)
{
	FILE *fp;
	bmp newm;
	unsigned char *rgbBuf;
	unsigned char *newRgbBuf;
	int i, j, k;
	int bmpWidth;
	int bmpHeight;
	int lineByte;
	int newBmpWidth;
	int newBmpHeight;
	int newLineByte;
	float cosAngle;
	float sinAngle;
	float f1, f2;
	int x1, x2, x3, x4;
	int y1, y2, y3, y4;
	int xmin, xmax, ymin, ymax;
	int intCapX, intCapY;
	int oldX0, oldY0;
	int newX0, newY0;

	if ((fp = fopen(inBmpName, "rb")) == NULL)
	{
		printf("can't open the bmp image.\n");
		return 0;
	}

	fread(&newm.file.bfType, sizeof(char), 2, fp);
	fread(&newm.file.bfSize, sizeof(int), 1, fp);
	fread(&newm.file.bfReserverd1, sizeof(short int), 1, fp);
	fread(&newm.file.bfReserverd2, sizeof(short int), 1, fp);
	fread(&newm.file.bfbfOffBits, sizeof(int), 1, fp);
	fread(&newm.info.biSize, sizeof(int), 1, fp);
	fread(&newm.info.biWidth, sizeof(int), 1, fp);
	fread(&newm.info.biHeight, sizeof(int), 1, fp);
	fread(&newm.info.biPlanes, sizeof(short), 1, fp);
	fread(&newm.info.biBitcount, sizeof(short), 1, fp);
	fread(&newm.info.biCompression, sizeof(int), 1, fp);
	fread(&newm.info.biSizeImage, sizeof(int), 1, fp);
	fread(&newm.info.biXPelsPermeter, sizeof(int), 1, fp);
	fread(&newm.info.biYPelsPermeter, sizeof(int), 1, fp);
	fread(&newm.info.biClrUsed, sizeof(int), 1, fp);
	fread(&newm.info.biClrImportant, sizeof(int), 1, fp);

	bmpWidth = newm.info.biWidth;
	bmpHeight = newm.info.biHeight;
	lineByte = (bmpWidth * 3 + 3) / 4 * 4;

	rgbBuf = (unsigned char *)malloc(lineByte * bmpHeight);
	fseek(fp, newm.file.bfbfOffBits, SEEK_SET);
	fread(rgbBuf, lineByte * bmpHeight, 1, fp);
	fclose(fp);

	oldX0 = (bmpWidth - 1) / 2;
	oldY0 = (bmpHeight - 1) / 2;

	cosAngle = (float)cos(fAngle);
	sinAngle = (float)sin(fAngle);

	f1 = oldX0 * (1 - cosAngle) + oldY0 * sinAngle;
	f2 = -oldX0 * sinAngle + oldY0 * (1 - cosAngle);

	x1 = (int)f1;
	y1 = (int)f2;
	x2 = (int)(bmpWidth * cosAngle + f1);
	y2 = (int)(bmpWidth * sinAngle + f2);
	x3 = (int)(-bmpHeight * sinAngle + f1);
	y3 = (int)(bmpHeight * cosAngle + f2);
	x4 = (int)(bmpWidth * cosAngle - bmpHeight * sinAngle + f1);
	y4 = (int)(bmpWidth * sinAngle + bmpHeight * cosAngle + f2);

	xmax = x1 > x2 ? (x1 > x3 ? (x1 > x4 ? x1 : x4) : (x3 > x4 ? x3 : x4)) : (x2 > x3 ? (x2 > x4 ? x2 : x4) : (x3 > x4 ? x3 : x4));
	xmin = x1 < x2 ? (x1 < x3 ? (x1 < x4 ? x1 : x4) : (x3 < x4 ? x3 : x4)) : (x2 < x3 ? (x2 < x4 ? x2 : x4) : (x3 < x4 ? x3 : x4));
	ymax = y1 > y2 ? (y1 > y3 ? (y1 > y4 ? y1 : y4) : (y3 > y4 ? y3 : y4)) : (y2 > y3 ? (y2 > y4 ? y2 : y4) : (y3 > y4 ? y3 : y4));
	ymin = y1 < y2 ? (y1 < y3 ? (y1 < y4 ? y1 : y4) : (y3 < y4 ? y3 : y4)) : (y2 < y3 ? (y2 < y4 ? y2 : y4) : (y3 < y4 ? y3 : y4));

	newBmpWidth = xmax - xmin;
	newBmpHeight = ymax - ymin;
	newLineByte = (newBmpWidth * 3 + 3) / 4 * 4;
	newRgbBuf = (unsigned char *)malloc(newLineByte * newBmpHeight);

	newX0 = (newBmpWidth - 1) / 2;
	newY0 = (newBmpHeight - 1) / 2;

	f1 = -newX0 * cosAngle - newY0 * sinAngle + oldX0;
	f2 = newX0 * sinAngle - newY0 * cosAngle + oldY0;

	for (i = 0; i < newBmpHeight; i++)
	{
		for (j = 0; j < newBmpWidth; j++)
		{
			intCapX = (int)(j * cosAngle + i * sinAngle + f1 + 0.5);
			intCapY = (int)(i * cosAngle - j * sinAngle + f2 + 0.5);

			if ((intCapX >= 0) && (intCapX < bmpWidth) && (intCapY >= 0) && (intCapY < bmpHeight))
			{
				int srcIdx = (bmpHeight - 1 - intCapY) * lineByte + intCapX * 3;
				int dstIdx = i * newLineByte + j * 3;
				for (k = 0; k < 3; k++)
					newRgbBuf[dstIdx + k] = rgbBuf[srcIdx + k];
			}
			else
			{
				int dstIdx = i * newLineByte + j * 3;
				for (k = 0; k < 3; k++)
					newRgbBuf[dstIdx + k] = 0xFF;
			}
		}
	}

	newm.file.bfType = 0x4d42;
	newm.file.bfSize = 54 + newLineByte * newBmpHeight;
	newm.file.bfReserverd1 = 0;
	newm.file.bfReserverd2 = 0;
	newm.file.bfbfOffBits = 54;
	newm.info.biSize = 40;
	newm.info.biWidth = newBmpWidth;
	newm.info.biHeight = newBmpHeight;
	newm.info.biPlanes = 1;
	newm.info.biBitcount = 24;
	newm.info.biCompression = 0;
	newm.info.biSizeImage = newLineByte * newBmpHeight;
	newm.info.biXPelsPermeter = 0;
	newm.info.biYPelsPermeter = 0;
	newm.info.biClrUsed = 0;
	newm.info.biClrImportant = 0;

	newm.pColorTable = NULL;
	newm.imgBuf = newRgbBuf;

	printf("Writing the image......\n");
	return SaveBmp(outBmpName, &newm);
}

int RGB2Gray(char *inBmpName, char *outBmpName)
{
	FILE *fp;
	bmp newm;
	unsigned char *rgbBuf;
	unsigned char *grayBuf;
	int i, j;
	int bmpWidth;
	int bmpHeight;
	int rgbLineByte;
	int grayLineByte;

	if ((fp = fopen(inBmpName, "rb")) == NULL)
	{
		printf("can't open the bmp image.\n");
		return 0;
	}

	fread(&newm.file.bfType, sizeof(char), 2, fp);
	fread(&newm.file.bfSize, sizeof(int), 1, fp);
	fread(&newm.file.bfReserverd1, sizeof(short int), 1, fp);
	fread(&newm.file.bfReserverd2, sizeof(short int), 1, fp);
	fread(&newm.file.bfbfOffBits, sizeof(int), 1, fp);
	fread(&newm.info.biSize, sizeof(int), 1, fp);
	fread(&newm.info.biWidth, sizeof(int), 1, fp);
	fread(&newm.info.biHeight, sizeof(int), 1, fp);
	fread(&newm.info.biPlanes, sizeof(short), 1, fp);
	fread(&newm.info.biBitcount, sizeof(short), 1, fp);
	fread(&newm.info.biCompression, sizeof(int), 1, fp);
	fread(&newm.info.biSizeImage, sizeof(int), 1, fp);
	fread(&newm.info.biXPelsPermeter, sizeof(int), 1, fp);
	fread(&newm.info.biYPelsPermeter, sizeof(int), 1, fp);
	fread(&newm.info.biClrUsed, sizeof(int), 1, fp);
	fread(&newm.info.biClrImportant, sizeof(int), 1, fp);

	bmpWidth = newm.info.biWidth;
	bmpHeight = newm.info.biHeight;

	rgbLineByte = (bmpWidth * 3 + 3) / 4 * 4;
	grayLineByte = (bmpWidth + 3) / 4 * 4;

	rgbBuf = (unsigned char *)malloc(rgbLineByte * bmpHeight);
	grayBuf = (unsigned char *)malloc(grayLineByte * bmpHeight);

	fseek(fp, newm.file.bfbfOffBits, SEEK_SET);
	fread(rgbBuf, rgbLineByte * bmpHeight, 1, fp);
	fclose(fp);

	for (i = 0; i < bmpHeight; i++)
	{
		for (j = 0; j < bmpWidth; j++)
		{
			int idx = (bmpHeight - 1 - i) * rgbLineByte + j * 3;
			*(grayBuf + i * grayLineByte + j) =
				(rgbBuf[idx + 2] * 76 + rgbBuf[idx + 1] * 150 + rgbBuf[idx] * 30) >> 8;
		}
	}

	newm.file.bfType = 0x4d42;
	newm.file.bfSize = 54 + 256 * 4 + grayLineByte * bmpHeight;
	newm.file.bfReserverd1 = 0;
	newm.file.bfReserverd2 = 0;
	newm.file.bfbfOffBits = 54 + 256 * 4;
	newm.info.biSize = 40;
	newm.info.biWidth = bmpWidth;
	newm.info.biHeight = bmpHeight;
	newm.info.biPlanes = 1;
	newm.info.biBitcount = 8;
	newm.info.biCompression = 0;
	newm.info.biSizeImage = grayLineByte * bmpHeight;
	newm.info.biXPelsPermeter = 0;
	newm.info.biYPelsPermeter = 0;
	newm.info.biClrUsed = 256;
	newm.info.biClrImportant = 256;

	newm.pColorTable = (RGBQUAD *)malloc(256 * 4);
	for (i = 0; i < 256; i++)
	{
		newm.pColorTable[i].rgbBlue = i;
		newm.pColorTable[i].rgbGreen = i;
		newm.pColorTable[i].rgbRed = i;
		newm.pColorTable[i].rgbReserved = 0;
	}

	newm.imgBuf = grayBuf;

	printf("Writing the image......\n");
	return SaveBmp(outBmpName, &newm);
}

int ImageReverse(bmp *m, char *outBmpName)
{
	int i;
	bmp newm;
	int bmpWidth;
	int bmpHeight;
	int biBitCount;
	int lineByte;

	bmpWidth = m->info.biWidth;
	bmpHeight = m->info.biHeight;
	biBitCount = m->info.biBitcount;
	lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;

	newBmpBuf = (unsigned char *)malloc(m->info.biSizeImage);

	for (i = 0; i < bmpWidth * bmpHeight; i++)
		*(newBmpBuf + i) = 0xff - *(m->imgBuf + i);

	newm.file.bfType = 0x4d42;
	newm.file.bfSize = 54 + 246 * 4 + lineByte * bmpHeight;
	newm.file.bfReserverd1 = 0;
	newm.file.bfReserverd2 = 0;
	newm.file.bfbfOffBits = 54 + 256 * 4;
	newm.info.biSize = 40;
	newm.info.biWidth = bmpWidth;
	newm.info.biHeight = bmpHeight;
	newm.info.biPlanes = 1;
	newm.info.biBitcount = 8;
	newm.info.biCompression = 0;
	newm.info.biSizeImage = lineByte * bmpHeight;
	newm.info.biXPelsPermeter = 0;
	newm.info.biYPelsPermeter = 0;
	newm.info.biClrUsed = 256;
	newm.info.biClrImportant = 256;

	memcpy((void *)(newpColorTable), (void *)(m->pColorTable), 256 * 4);
	newm.pColorTable = (RGBQUAD *)newpColorTable;

	newm.imgBuf = newBmpBuf;

	printf("Writing the image......\n");
	return SaveBmp(outBmpName, &newm);
}
