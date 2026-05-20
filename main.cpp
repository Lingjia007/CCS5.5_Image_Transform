#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cxcore.h"
#include "cv.h"

#define SW_BREAKPOINT asm(" SWBP 0 ");

static int readIntLE(unsigned char *p)
{
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

static short readShortLE(unsigned char *p)
{
    return (short)(p[0] | (p[1] << 8));
}

static void writeIntLE(unsigned char *p, int v)
{
    p[0] = v & 0xFF;
    p[1] = (v >> 8) & 0xFF;
    p[2] = (v >> 16) & 0xFF;
    p[3] = (v >> 24) & 0xFF;
}

static void writeShortLE(unsigned char *p, short v)
{
    p[0] = v & 0xFF;
    p[1] = (v >> 8) & 0xFF;
}

static IplImage* myLoadBmp(const char *filename)
{
    FILE *p;
    unsigned char header[54];
    int width, height, bitCount, dataOffset, channels;
    int step, idx;
    IplImage *image = NULL;

    p = fopen(filename, "rb");
    if (!p) return NULL;

    if (fread(header, 1, 54, p) != 54)
    {
        fclose(p);
        return NULL;
    }

    if (header[0] != 'B' || header[1] != 'M')
    {
        fclose(p);
        return NULL;
    }

    dataOffset = readIntLE(header + 10);
    width = readIntLE(header + 18);
    height = readIntLE(header + 22);
    bitCount = readShortLE(header + 28);
    channels = bitCount / 8;

    if (channels != 1 && channels != 3)
    {
        fclose(p);
        return NULL;
    }

    step = (width * channels + 3) & ~3;

    image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, channels);

    fseek(p, dataOffset, SEEK_SET);
    for (idx = height - 1; idx >= 0; idx--)
    {
        if (fread(image->imageData + image->widthStep * idx, 1, step, p) != step)
        {
            cvReleaseImage(&image);
            fclose(p);
            return NULL;
        }
    }

    fclose(p);
    return image;
}

static int mySaveBmp(const char *filename, IplImage *image)
{
    FILE *p;
    unsigned char header[54];
    int channels = image->nChannels;
    int width = image->width;
    int height = image->height;
    int step = (width * channels + 3) & ~3;
    int dataSize = step * height;
    int colorTableSize = (channels == 1) ? 1024 : 0;
    int dataOffset = 54 + colorTableSize;
    int fileSize = dataOffset + dataSize;
    int idx;

    p = fopen(filename, "wb");
    if (!p) return -1;

    memset(header, 0, 54);

    header[0] = 'B';
    header[1] = 'M';
    writeIntLE(header + 2, fileSize);
    writeIntLE(header + 10, dataOffset);

    writeIntLE(header + 14, 40);
    writeIntLE(header + 18, width);
    writeIntLE(header + 22, height);
    writeShortLE(header + 26, 1);
    writeShortLE(header + 28, (short)(channels * 8));
    writeIntLE(header + 34, dataSize);

    fwrite(header, 1, 54, p);

    if (channels == 1)
    {
        unsigned char colorTable[1024];
        for (idx = 0; idx < 256; idx++)
        {
            colorTable[idx * 4 + 0] = (unsigned char)idx;
            colorTable[idx * 4 + 1] = (unsigned char)idx;
            colorTable[idx * 4 + 2] = (unsigned char)idx;
            colorTable[idx * 4 + 3] = 0;
        }
        fwrite(colorTable, 1, 1024, p);
    }

    for (idx = height - 1; idx >= 0; idx--)
    {
        fwrite(image->imageData + image->widthStep * idx, 1, step, p);
    }

    fclose(p);
    return 0;
}

static void myRGB2Gray(IplImage *src, IplImage *dst)
{
    IplImage *chB = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage *chG = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage *chR = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

    cvSplit(src, chB, chG, chR, NULL);

    cvAddWeighted(chR, 0.299, chG, 0.587, 0.0, dst);
    cvAddWeighted(dst, 1.0, chB, 0.114, 0.0, dst);

    cvReleaseImage(&chB);
    cvReleaseImage(&chG);
    cvReleaseImage(&chR);
}

static void myRotate(IplImage *src, IplImage *dst, float angleDeg)
{
    int x, y, c;
    int srcW = src->width;
    int srcH = src->height;
    int dstW = dst->width;
    int dstH = dst->height;
    int srcStep = src->widthStep;
    int dstStep = dst->widthStep;
    int channels = src->nChannels;
    float cx = srcW / 2.0f;
    float cy = srcH / 2.0f;
    float angleRad = angleDeg * 3.14159265f / 180.0f;
    float cosA = cosf(angleRad);
    float sinA = sinf(angleRad);
    int dstRowBytes = dstW * channels;

    for (y = 0; y < dstH; y++)
    {
        unsigned char *pDst = (unsigned char *)dst->imageData + y * dstStep;
        memset(pDst, 0, dstRowBytes);
    }

    for (y = 0; y < dstH; y++)
    {
        unsigned char *pDst = (unsigned char *)dst->imageData + y * dstStep;
        for (x = 0; x < dstW; x++)
        {
            float dx = (float)x - cx;
            float dy = (float)y - cy;
            float srcXf = cosA * dx + sinA * dy + cx;
            float srcYf = -sinA * dx + cosA * dy + cy;
            int srcX0 = (int)srcXf;
            int srcY0 = (int)srcYf;
            int srcX1 = srcX0 + 1;
            int srcY1 = srcY0 + 1;

            if (srcXf >= 0 && srcX1 < srcW && srcYf >= 0 && srcY1 < srcH)
            {
                float fx = srcXf - srcX0;
                float fy = srcYf - srcY0;
                unsigned char *pSrc = (unsigned char *)src->imageData;
                for (c = 0; c < channels; c++)
                {
                    float v00 = pSrc[srcY0 * srcStep + srcX0 * channels + c];
                    float v01 = pSrc[srcY0 * srcStep + srcX1 * channels + c];
                    float v10 = pSrc[srcY1 * srcStep + srcX0 * channels + c];
                    float v11 = pSrc[srcY1 * srcStep + srcX1 * channels + c];
                    float val = v00 * (1 - fx) * (1 - fy) + v01 * fx * (1 - fy)
                              + v10 * (1 - fx) * fy + v11 * fx * fy;
                    pDst[x * channels + c] = (unsigned char)(val + 0.5f);
                }
            }
        }
    }
}

int main(void)
{
    IplImage *srcGray = NULL;
    IplImage *srcColor = NULL;
    IplImage *dst = NULL;
    char outFileName[100];
    int Fa_contrast = 2;
    int Fb_contrast = -128;
    int Fa_brightness = 1;
    int Fb_brightness = 50;
    float fAngleDeg = 30.0f;

    printf("\r\nImage Transform Application (OpenCV DSP).\r\n");

    printf("Reading the grayscale image......\n");
    srcGray = myLoadBmp("../Image/TL_C6748_Gray.bmp");
    if (!srcGray)
    {
        printf("Failed to read TL_C6748_Gray.bmp\n");
        return -1;
    }
    printf("Read successfully. w=%d h=%d ch=%d\n", srcGray->width, srcGray->height, srcGray->nChannels);

    dst = cvCreateImage(cvGetSize(srcGray), IPL_DEPTH_8U, srcGray->nChannels);

    sprintf(outFileName, "../Image/Out_LinerTrans_Contrast_Fa%d_Fb%d.bmp", Fa_contrast, Fb_contrast);
    cvConvertScale(srcGray, dst, (double)Fa_contrast, (double)Fb_contrast);
    mySaveBmp(outFileName, dst);
    printf("LinerTrans Contrast processing is completed.\n");

    sprintf(outFileName, "../Image/Out_LinerTrans_Brightness_Fa%d_Fb%d.bmp", Fa_brightness, Fb_brightness);
    cvConvertScale(srcGray, dst, (double)Fa_brightness, (double)Fb_brightness);
    mySaveBmp(outFileName, dst);
    printf("LinerTrans Brightness processing is completed.\n");

    sprintf(outFileName, "../Image/Out_ImageReverse.bmp");
    cvSubRS(srcGray, cvScalarAll(255), dst, NULL);
    mySaveBmp(outFileName, dst);
    printf("ImageReverse processing is completed.\n");

    cvReleaseImage(&dst);
    cvReleaseImage(&srcGray);

    printf("Reading the color image......\n");
    srcColor = myLoadBmp("../Image/LCD.bmp");
    if (!srcColor)
    {
        printf("Failed to read LCD.bmp\n");
        return -1;
    }
    printf("Read successfully. w=%d h=%d ch=%d\n", srcColor->width, srcColor->height, srcColor->nChannels);

    dst = cvCreateImage(cvGetSize(srcColor), IPL_DEPTH_8U, 1);
    sprintf(outFileName, "../Image/Out_RGB2Gray.bmp");
    myRGB2Gray(srcColor, dst);
    mySaveBmp(outFileName, dst);
    printf("RGB2Gray processing is completed.\n");

    cvReleaseImage(&dst);

    dst = cvCreateImage(cvGetSize(srcColor), IPL_DEPTH_8U, srcColor->nChannels);
    sprintf(outFileName, "../Image/Out_Rotate_30deg.bmp");
    myRotate(srcColor, dst, fAngleDeg);
    mySaveBmp(outFileName, dst);
    printf("Rotate processing is completed.\n");

    cvReleaseImage(&dst);
    cvReleaseImage(&srcColor);

    SW_BREAKPOINT;

    while (1)
        ;
}
