#include "stdafx.h"
#include "ShapeMatch.h"

#define	MAXTARGETNUM	64

MatchResultA	resultsPerDeg[MAXTARGETNUM];	//ÿ���Ƕȶ�Ӧ��ƥ��������
MatchResultA	totalResultsTemp[MAXTARGETNUM];	//����ƥ��������

CShapeMatch::CShapeMatch(void)
{
}

CShapeMatch::~CShapeMatch(void)
{
}

void CShapeMatch::gaussian_filter(uint8_t* corrupted, uint8_t* smooth, int width, int height)
{
	int templates[25] = 
	{ 1,   4,   7,   4, 1,   
	  4, 16, 26, 16, 4,   
	  7, 26, 41, 26, 7,  
	  4, 16, 26, 16, 4,   
	  1,   4,   7,   4, 1 };        

	memcpy ( smooth, corrupted, width*height*sizeof(uint8_t) );  
	for (int j = 2; j < height - 2; j++)  
	{  
		for (int i = 2; i < width -2; i++)  
		{  
			int sum = 0;  
			int index = 0;  
			for ( int m=j-2; m<j+3; m++)  
			{  
				for (int n=i-2; n<i+3; n++)  
				{  
					sum += corrupted [ m*width + n] * templates[index++] ;  
				}  
			}  
			sum /= 273;  
			if (sum > 255)  
				sum = 255;  
			smooth [ j*width+i ] = (uint8_t)sum;  
		}  
	}  
}

void CShapeMatch::gen_rectangle(IplImage *Image, IplImage *ModelRegion, int Row1, int Column1)
{
	//const int startX = Row1;
	//const int startY = Column1;
	//const int width  = ModelRegion->width;
	//const int height = ModelRegion->height;
	//const int imgWidth = Image->width;

	//memset(ModelRegion->imageData, 0, ModelRegion->widthStep * ModelRegion->height);

	//int i;
	//uint8_t* imageResource = (uint8_t*)malloc(width*height);
	//for(i=0;i<height;i++)
	//{
	//	memcpy((imageResource + i*width), (uint8_t*)(Image->imageData + (startY+i)*imgWidth) + startX, width);
	//}
	//memcpy((uint8_t*)ModelRegion->imageData, imageResource, width*height);
	//free(imageResource);

	cvSetImageROI(Image,cvRect(Row1,Column1,ModelRegion->width, ModelRegion->height));//����Դͼ��ROI
	cvCopy(Image,ModelRegion); //����ͼ��
	//cvSaveImage("..\\SaveImage\ModelRegion.bmp", ModelRegion);
	cvResetImageROI(ModelRegion);//Դͼ����������ROI

	return;
}

void CShapeMatch::board_image(IplImage *SrcImg, IplImage *ImgBordered, int32_t xOffset, int32_t yOffset)
{
	int32_t ImgBorderedWStep = ImgBordered->widthStep / sizeof(char);
	int32_t ImgFilteredWStep = SrcImg->widthStep / sizeof(char);

	//������ͼ��������
	memset(ImgBordered->imageData, *(SrcImg->imageData), ImgBordered->widthStep * ImgBordered->height);	

	int32_t row, col;
	//����Ч�������ԭʼͼ������
	for(row = yOffset; row < SrcImg->height + yOffset; row++)								
		memcpy(ImgBordered->imageData + row * ImgBorderedWStep + xOffset, SrcImg->imageData + (row - yOffset)*ImgFilteredWStep, SrcImg->width);
	//����ϲ��߽粿��
	for(row = yOffset; row >=0; row--)														
		memcpy(ImgBordered->imageData + row * ImgBorderedWStep + xOffset, SrcImg->imageData, SrcImg->width);
	//����²��߽粿��
	for(row = yOffset + SrcImg->height ; row < ImgBordered->height; row++)					
		memcpy(ImgBordered->imageData + row * ImgBorderedWStep + xOffset, SrcImg->imageData + (SrcImg->height-1)*ImgFilteredWStep, SrcImg->width);
	//���߽粿��
	for(col = 0; col < xOffset; col++)															
	{
		for(row = 0; row < ImgBordered->height; row++)
		{
			*(ImgBordered->imageData + row * ImgBorderedWStep + col) = *(ImgBordered->imageData + row * ImgBorderedWStep + xOffset);
		}
	}
	//���߽粿��
	for(col = xOffset + SrcImg->width; col < ImgBordered->width; col++)
	{
		for(row = 0; row < ImgBordered->height; row++)
		{
			*(ImgBordered->imageData + row * ImgBorderedWStep + col) = *(ImgBordered->imageData + row * ImgBorderedWStep + xOffset + SrcImg->width - 1);
		}
	}
}

void CShapeMatch::rotate_image(uint8_t *SrcImgData, uint8_t *MaskImgData, int srcWidth, int srcHeight, uint8_t *DstImgData, uint8_t *MaskRotData, int dstWidth, int dstHeight, int Angle)
{
	int xcenter = srcWidth >> 1;
	int ycenter = srcHeight >> 1;
	int xnew = dstWidth >> 1;
	int ynew = dstHeight >> 1;

	int SinA = ShiftSin(Angle);
	int CosA = ShiftCos(Angle);
	int nSinA= (-1) * ShiftSin(Angle);

	const int xFix = ( xcenter << 8 ) - ((ynew * SinA) >> 5 ) - ((xnew * CosA) >> 5) ;
	const int yFix = ( ycenter << 8 ) + ((xnew * SinA) >> 5 ) - ((ynew * CosA) >> 5) ;

	int ox;
	int oy;
	int x;
	int y;
	int kx;
	int ky;

	int j;
	int i;
	uint8_t value [2][2];

	for (j = 0; j < dstHeight; j++)
	{
		for (i = 0; i < dstWidth; i++)
		{
			ox = ((i * CosA  + j * SinA) >> 5) + xFix;
			oy = ((i * nSinA + j * CosA) >> 5) + yFix;   
			if ((ox >> 8) < (srcWidth - 1) && (ox >> 8) > 1 && (oy >> 8) < (srcHeight - 1) && (oy >> 8) > 1)
			{
				kx = ox >> 8;
				ky = oy >> 8;
				x = ox & 0xFF;
				y = oy & 0xFF;
				value[0][0] = *(SrcImgData + ky*srcWidth + kx);
				value[1][0] = *(SrcImgData + ky*srcWidth + kx + 1);
				value[0][1] = *(SrcImgData + (ky+1)*srcWidth + kx);
				value[1][1] = *(SrcImgData + (ky+1)*srcWidth + kx + 1);
				int result 	= (0x100 - x)*(0x100 - y)*value[0][0] + x*(0x100 - y)*value[1][0] + (0x100-x)*y*value[0][1] + x*y*value[1][1];
				result = result >> 16;
				result = (result > 255) ? 255 : result;
				result = (result < 0	) ? 0	: result;
				*(DstImgData + j*dstWidth + i) = (uint8_t)result;

				value[0][0] = *(MaskImgData + ky*srcWidth + kx);
				value[1][0] = *(MaskImgData + ky*srcWidth + kx + 1);
				value[0][1] = *(MaskImgData + (ky+1)*srcWidth + kx);
				value[1][1] = *(MaskImgData + (ky+1)*srcWidth + kx + 1);
				int mask	= (0x100 - x)*(0x100 - y)*value[0][0] + x*(0x100 - y)*value[1][0] + (0x100-x)*y*value[0][1] + x*y*value[1][1];
				mask = mask >> 16;
				mask = (mask > 255) ? 255 : mask;
				mask = (mask < 0	) ? 0 : mask;
				*(MaskRotData + j*dstWidth + i) = (uint8_t)mask;
			}
			else
			{
				kx = ox >> 8;
				ky = oy >> 8;
				*(DstImgData + j*dstWidth + i) = 0x00;
			}
		}
	}
}

void CShapeMatch::rotateImage(IplImage* srcImage, IplImage* dstImage, int Angle)
{
	float m[6];  
	m[0] = (float) cos(Angle * CV_PI / 180.);  
	m[1] = (float) sin(Angle * CV_PI / 180.);  
	m[3] = -m[1];  
	m[4] = m[0];  
	m[2] = srcImage->width * 0.5f;  
	m[5] = srcImage->width * 0.5f;  

	CvMat M = cvMat(2, 3, CV_32F, m);  
	cvGetQuadrangleSubPix(srcImage, dstImage, &M);
}

void CShapeMatch::image_pyramid(uint8_t *SrcImgData, int srcWidth, int srcHeight, uint8_t *OutImgData)
{
	int w = srcWidth >> 1;  
	int h = srcHeight >> 1;   
	int in_size = srcWidth * srcHeight;
	int offset = 0;
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			*(OutImgData + j * w + i) = *(SrcImgData + 2 * j * srcWidth + i * 2);
		}
	}

	//IplImage *PyImage = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	//memcpy((uint8_t*)PyImage->imageData, OutImgData, w*h * sizeof(uint8_t));
	//cvNamedWindow("PyImage",CV_WINDOW_AUTOSIZE );
	//cvShowImage("PyImage",PyImage);

	w = srcWidth >> 2;  
	h = srcHeight >> 2;  
	offset = in_size / 4;
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			*(OutImgData + offset + j * w + i) = *(SrcImgData + 4 * j * srcWidth + i * 4);
		}
	}

	//IplImage *Py2Image = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	//memcpy((uint8_t*)Py2Image->imageData, OutImgData + in_size/4, w*h * sizeof(uint8_t));
	//cvNamedWindow("Py2Image",CV_WINDOW_AUTOSIZE );
	//cvShowImage("Py2Image",Py2Image);

	w = srcWidth >> 3;  
	h = srcHeight >> 3;  
	offset += in_size / 16;
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			*(OutImgData + offset + j * w + i) = *(SrcImgData + 8 * j * srcWidth + i * 8);
		}
	}

	//IplImage *Py3Image = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	//memcpy((uint8_t*)Py3Image->imageData, OutImgData + in_size*5/16, w*h * sizeof(uint8_t));
	//cvNamedWindow("Py3Image",CV_WINDOW_AUTOSIZE );
	//cvShowImage("Py3Image",Py3Image);
}

void CShapeMatch::imagePyramid(uint8_t *SrcImgData, int srcWidth, int srcHeight, uint8_t *OutImgData)
{
	int srcImgHeight = srcWidth;  
	int srcImgWidth = srcHeight;  
	int dstImgHeight = srcImgHeight >> 1;   
	int dstImgWidth = srcImgWidth >> 1;  
	int srcImgWidthStep = srcImgWidth;  
	int dstImgWidthStep = dstImgWidth;  

	float heightRatio = (float)dstImgHeight/(float)srcImgHeight;  
	float widthRatio = (float)dstImgWidth/(float)srcImgWidth;  

	for (int i=0;i<dstImgHeight;i++)  
	{  
		float fx = (float)i/heightRatio;  
		int nx = (int)fx;  
		int nxa1 = nx+1;  
		float p = fx-nx;   
		if (nxa1>=srcImgHeight) //���һ��  
		{  
			nxa1 = srcImgHeight-1;  
		}  
		for (int j=0;j<dstImgWidth;j++)  
		{  
			float fy = (float)j/widthRatio;  
			int ny = (int)fy;  
			int nya1 = ny+1;  
			float q = fy - ny;  
			if (nya1>=srcImgWidth) //�������һ��Ԫ��  
			{  
				nya1 = srcImgWidth -1;  
			}  

			float b = (1-p)*(1-q)*SrcImgData[nx*srcImgWidthStep+ny];  
			b += (1-p)*q*SrcImgData[nx*srcImgWidthStep+nya1];  
			b += p*(1-q)*SrcImgData[nxa1*srcImgWidthStep+ny];  
			b += p*q*SrcImgData[nxa1*srcImgWidthStep+nya1];  

			OutImgData[i*dstImgWidthStep+j]=(int)b;  

			float g = (1-p)*(1-q)*SrcImgData[nx*srcImgWidthStep+ny+1];  
			g += (1-p)*q*SrcImgData[nx*srcImgWidthStep+nya1+1];  
			g += p*(1-q)*SrcImgData[nxa1*srcImgWidthStep+ny+1];  
			g += p*q*SrcImgData[nxa1*srcImgWidthStep+nya1+1];  

			OutImgData[i*dstImgWidthStep+j+1]=(int)g;  

			float r = (1-p)*(1-q)*SrcImgData[nx*srcImgWidthStep+ny+2];  
			r += (1-p)*q*SrcImgData[nx*srcImgWidthStep+nya1+2];  
			r += p*(1-q)*SrcImgData[nxa1*srcImgWidthStep+ny+2];  
			r += p*q*SrcImgData[nxa1*srcImgWidthStep+nya1+2];  

			OutImgData[i*dstImgWidthStep+j+2]=(int)r;  
		}  
	} 
}

void CShapeMatch::initial_shape_model(shape_model *ModelID, int Width, int Height, int EdgeSize)
{
	ModelID->m_pShapeInfoPyd1Vec = NULL;
	ModelID->m_pShapeInfoPyd2Vec = NULL;
	ModelID->m_pShapeInfoPyd3Vec = NULL;
	ModelID->m_pShapeInfoTmpVec  = NULL;

	int Length = ConvertLength(MAX(Width, Height));

	int AngleStart = ModelID->m_AngleStart;
	int AngleStop = ModelID->m_AngleStop;
	int AngleStep = ModelID->m_AngleStep;

	switch(ModelID->m_NumLevels)
	{
		int Width, Height;
	case 3:
		{
			/* Initial pyd3 image model list */
			Width = Height =  Length >> 3;
			//int ShapeSize = EdgeSize >> 2;
			int ShapeSize = Width * Height;

			AngleStep = ModelID->m_AngleStep << 3;

			if (ModelID->m_pShapeInfoPyd3Vec != NULL)
			{
				free(ModelID->m_pShapeInfoPyd3Vec);
				ModelID->m_pShapeInfoPyd3Vec = NULL;
			}

			int AngleNum = 0;
			for (int iAngle = AngleStart; iAngle < AngleStop; iAngle += AngleStep)
			{
				if (iAngle == 0)
					continue;
				AngleNum++;
			}
			if(AngleStart == AngleStop)
			{
				AngleNum += 1;
			}
			else
				AngleNum += 2;
			
			ModelID->m_pShapeInfoPyd3Vec = (ShapeInfo*)malloc(AngleNum *sizeof(ShapeInfo));
			memset(ModelID->m_pShapeInfoPyd3Vec, 0, AngleNum * sizeof(ShapeInfo));

			if (AngleStart <= 0)
			{
				ModelID->m_pShapeInfoPyd3Vec[0].Angel = AngleStart;
				ModelID->m_pShapeInfoPyd3Vec[AngleNum - 1].Angel = AngleStop;
			}
			else
			{
				ModelID->m_pShapeInfoPyd3Vec[0].Angel = 0;
				ModelID->m_pShapeInfoPyd3Vec[1].Angel = AngleStart;
				ModelID->m_pShapeInfoPyd3Vec[AngleNum - 1].Angel = AngleStop;
			}
			int Angle = AngleStart + AngleStep;
			bool isFilled = false;
			for (int i = 1; i < AngleNum - 1; i++)
			{
				if ((!isFilled) && (Angle >= 0 && (Angle - AngleStep) < 0))
				{
					ModelID->m_pShapeInfoPyd3Vec[i].Angel = 0;
					if (Angle == 0)
					{
						Angle += AngleStep;
					}
					isFilled = true;
				}
				else if (Angle < AngleStop && Angle != 0)
				{
					ModelID->m_pShapeInfoPyd3Vec[i].Angel = Angle;
					Angle += AngleStep;
				}
			}
			for (int i = 0; i < AngleNum; i++) 
			{
				ModelID->m_pShapeInfoPyd3Vec[i].PyLevel = 3;
				ModelID->m_pShapeInfoPyd3Vec[i].AngleNum		  = AngleNum;
				ModelID->m_pShapeInfoPyd3Vec[i].Coordinates	      = (CvPoint *) malloc(ShapeSize * sizeof(CvPoint));
				ModelID->m_pShapeInfoPyd3Vec[i].EdgeDerivativeX = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoPyd3Vec[i].EdgeDerivativeY = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoPyd3Vec[i].EdgeMagnitude = (float *  )   malloc(ShapeSize * sizeof(float));

				memset(ModelID->m_pShapeInfoPyd3Vec[i].Coordinates,  	    0, ShapeSize * sizeof(CvPoint));
				memset(ModelID->m_pShapeInfoPyd3Vec[i].EdgeDerivativeX, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoPyd3Vec[i].EdgeDerivativeY, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoPyd3Vec[i].EdgeMagnitude, 0, ShapeSize * sizeof(float));
			}

			/* Initial pyd2 image model list */
			Width = Height =  Length >> 2;
			//ShapeSize = EdgeSize >> 1;
			ShapeSize = Width * Height;
			AngleStep = ModelID->m_AngleStep << 2;

			if (ModelID->m_pShapeInfoPyd2Vec != NULL)
			{
				free(ModelID->m_pShapeInfoPyd2Vec);
				ModelID->m_pShapeInfoPyd2Vec = NULL;
			}

			AngleNum = 0;
			for (int iAngle = AngleStart; iAngle < AngleStop; iAngle += AngleStep)
			{
				if (iAngle == 0)
					continue;
				AngleNum++;
			}

			if(AngleStart == AngleStop)
			{
				AngleNum += 1;
			}
			else
				AngleNum += 2;

			ModelID->m_pShapeInfoPyd2Vec = (ShapeInfo*)malloc(AngleNum *sizeof(ShapeInfo));
			memset(ModelID->m_pShapeInfoPyd2Vec, 0, AngleNum * sizeof(ShapeInfo));

			if (AngleStart <= 0)
			{
				ModelID->m_pShapeInfoPyd2Vec[0].Angel = AngleStart;
				ModelID->m_pShapeInfoPyd2Vec[AngleNum - 1].Angel = AngleStop;
			}
			else
			{
				ModelID->m_pShapeInfoPyd2Vec[0].Angel = 0;
				ModelID->m_pShapeInfoPyd2Vec[1].Angel = AngleStart;
				ModelID->m_pShapeInfoPyd2Vec[AngleNum - 1].Angel = AngleStop;
			}
			Angle = AngleStart + AngleStep;
			isFilled = false;
			for (int i = 1; i < AngleNum - 1; i++)
			{
				if ((!isFilled) && (Angle >= 0 && (Angle - AngleStep) < 0))
				{
					ModelID->m_pShapeInfoPyd2Vec[i].Angel = 0;
					if (Angle == 0)
					{
						Angle += AngleStep;
					}
					isFilled = true;
				}
				else if (Angle < AngleStop && Angle != 0)
				{
					ModelID->m_pShapeInfoPyd2Vec[i].Angel = Angle;
					Angle += AngleStep;
				}
			}
			for (int i = 0; i < AngleNum; i++) 
			{
				ModelID->m_pShapeInfoPyd2Vec[i].PyLevel = 2;
				ModelID->m_pShapeInfoPyd2Vec[i].AngleNum		  = AngleNum;
				ModelID->m_pShapeInfoPyd2Vec[i].Coordinates	      = (CvPoint *) malloc(ShapeSize * sizeof(CvPoint));
				ModelID->m_pShapeInfoPyd2Vec[i].EdgeDerivativeX = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoPyd2Vec[i].EdgeDerivativeY = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoPyd2Vec[i].EdgeMagnitude = (float *  )   malloc(ShapeSize * sizeof(float));

				memset(ModelID->m_pShapeInfoPyd2Vec[i].Coordinates,  	    0, ShapeSize * sizeof(CvPoint));
				memset(ModelID->m_pShapeInfoPyd2Vec[i].EdgeDerivativeX, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoPyd2Vec[i].EdgeDerivativeY, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoPyd2Vec[i].EdgeMagnitude, 0, ShapeSize * sizeof(float));
			}

			/* Initial pyd1 image model list */
			Width = Height =  Length >> 1;
			//ShapeSize = EdgeSize;
			ShapeSize = Width * Height;
			AngleStep = ModelID->m_AngleStep << 1;

			if (ModelID->m_pShapeInfoPyd1Vec != NULL)
			{
				free(ModelID->m_pShapeInfoPyd1Vec);
				ModelID->m_pShapeInfoPyd1Vec = NULL;
			}

			AngleNum = 0;
			for (int iAngle = AngleStart; iAngle < AngleStop; iAngle += AngleStep)
			{
				if (iAngle == 0)
					continue;
				AngleNum++;
			}

			if(AngleStart == AngleStop)
			{
				AngleNum += 1;
			}
			else
				AngleNum += 2;

			ModelID->m_pShapeInfoPyd1Vec = (ShapeInfo*)malloc(AngleNum *sizeof(ShapeInfo));
			memset(ModelID->m_pShapeInfoPyd1Vec, 0, AngleNum * sizeof(ShapeInfo));

			if (AngleStart <= 0)
			{
				ModelID->m_pShapeInfoPyd1Vec[0].Angel = AngleStart;
				ModelID->m_pShapeInfoPyd1Vec[AngleNum - 1].Angel = AngleStop;
			}
			else
			{
				ModelID->m_pShapeInfoPyd1Vec[0].Angel = 0;
				ModelID->m_pShapeInfoPyd1Vec[1].Angel = AngleStart;
				ModelID->m_pShapeInfoPyd1Vec[AngleNum - 1].Angel = AngleStop;
			}
			Angle = AngleStart + AngleStep;
			isFilled = false;
			for (int i = 1; i < AngleNum - 1; i++)
			{
				if ((!isFilled) && (Angle >= 0 && (Angle - AngleStep) < 0))
				{
					ModelID->m_pShapeInfoPyd1Vec[i].Angel = 0;
					if (Angle == 0)
					{
						Angle += AngleStep;
					}
					isFilled = true;
				}
				else if (Angle < AngleStop && Angle != 0)
				{
					ModelID->m_pShapeInfoPyd1Vec[i].Angel = Angle;
					Angle += AngleStep;
				}
			}
			for (int i = 0; i < AngleNum; i++) 
			{
				ModelID->m_pShapeInfoPyd1Vec[i].PyLevel = 1;
				ModelID->m_pShapeInfoPyd1Vec[i].AngleNum		  = AngleNum;
				ModelID->m_pShapeInfoPyd1Vec[i].Coordinates	      = (CvPoint *) malloc(ShapeSize * sizeof(CvPoint));
				ModelID->m_pShapeInfoPyd1Vec[i].EdgeDerivativeX = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoPyd1Vec[i].EdgeDerivativeY = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoPyd1Vec[i].EdgeMagnitude = (float *  )   malloc(ShapeSize * sizeof(float));

				memset(ModelID->m_pShapeInfoPyd1Vec[i].Coordinates,  	    0, ShapeSize * sizeof(CvPoint));
				memset(ModelID->m_pShapeInfoPyd1Vec[i].EdgeDerivativeX, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoPyd1Vec[i].EdgeDerivativeY, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoPyd1Vec[i].EdgeMagnitude, 0, ShapeSize * sizeof(float));
			}

			/* Initial source image model list */
		    AngleNum = ModelID->m_AngleStop - ModelID->m_AngleStart + 1;
			if (ModelID->m_pShapeInfoTmpVec != NULL)
			{
				free(ModelID->m_pShapeInfoTmpVec);
				ModelID->m_pShapeInfoTmpVec = NULL;
			}

			ModelID->m_pShapeInfoTmpVec = (ShapeInfo*)malloc(AngleNum * sizeof(ShapeInfo));
			memset(ModelID->m_pShapeInfoTmpVec, 0, AngleNum * sizeof(ShapeInfo));
			//ShapeSize = EdgeSize * 2;
			ShapeSize = Length * Length;
			//ShapeSize = Length * Length;

			for (int i = 0; i < AngleNum; i++)
			{
				ModelID->m_pShapeInfoTmpVec[i].PyLevel = 0;
				ModelID->m_pShapeInfoTmpVec[i].Angel = AngleStart + i * ModelID->m_AngleStep;
				ModelID->m_pShapeInfoTmpVec[i].AngleNum		 = AngleNum;
				ModelID->m_pShapeInfoTmpVec[i].Coordinates	     = (CvPoint *) malloc(ShapeSize * sizeof(CvPoint));
				ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeX = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeY = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoTmpVec[i].EdgeMagnitude = (float *  ) malloc(ShapeSize * sizeof(float));

				memset(ModelID->m_pShapeInfoTmpVec[i].Coordinates,  	   0, ShapeSize * sizeof(CvPoint));
				memset(ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeX, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeY, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoTmpVec[i].EdgeMagnitude, 0, ShapeSize * sizeof(float));
			}

			break;
		}
	case 2:
		{
			/* Initial pyd2 image model list */
			Width = Height =  Length >> 2;
			//int ShapeSize = EdgeSize >> 1;
			int ShapeSize = Width * Height;
			AngleStep = ModelID->m_AngleStep << 2;

			if (ModelID->m_pShapeInfoPyd2Vec != NULL)
			{
				free(ModelID->m_pShapeInfoPyd2Vec);
				ModelID->m_pShapeInfoPyd2Vec = NULL;
			}

			int AngleNum = 0;
			for (int iAngle = AngleStart; iAngle < AngleStop; iAngle += AngleStep)
			{
				if (iAngle == 0)
					continue;
				AngleNum++;
			}

			if(AngleStart == AngleStop)
			{
				AngleNum += 1;
			}
			else
				AngleNum += 2;

			ModelID->m_pShapeInfoPyd2Vec = (ShapeInfo*)malloc(AngleNum *sizeof(ShapeInfo));
			memset(ModelID->m_pShapeInfoPyd2Vec, 0, AngleNum * sizeof(ShapeInfo));

			if (AngleStart <= 0)
			{
				ModelID->m_pShapeInfoPyd2Vec[0].Angel = AngleStart;
				ModelID->m_pShapeInfoPyd2Vec[AngleNum - 1].Angel = AngleStop;
			}
			else
			{
				ModelID->m_pShapeInfoPyd2Vec[0].Angel = 0;
				ModelID->m_pShapeInfoPyd2Vec[1].Angel = AngleStart;
				ModelID->m_pShapeInfoPyd2Vec[AngleNum - 1].Angel = AngleStop;
			}
			int Angle = AngleStart + AngleStep;
			bool isFilled = false;
			for (int i = 1; i < AngleNum - 1; i++)
			{
				if ((!isFilled) && (Angle >= 0 && (Angle - AngleStep) < 0))
				{
					ModelID->m_pShapeInfoPyd2Vec[i].Angel = 0;
					if (Angle == 0)
					{
						Angle += AngleStep;
					}
					isFilled = true;
				}
				else if (Angle < AngleStop && Angle != 0)
				{
					ModelID->m_pShapeInfoPyd2Vec[i].Angel = Angle;
					Angle += AngleStep;
				}
			}
			for (int i = 0; i < AngleNum; i++) 
			{
				ModelID->m_pShapeInfoPyd2Vec[i].PyLevel = 2;
				ModelID->m_pShapeInfoPyd2Vec[i].AngleNum		  = AngleNum;
				ModelID->m_pShapeInfoPyd2Vec[i].Coordinates	      = (CvPoint *) malloc(ShapeSize * sizeof(CvPoint));
				ModelID->m_pShapeInfoPyd2Vec[i].EdgeDerivativeX = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoPyd2Vec[i].EdgeDerivativeY = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoPyd2Vec[i].EdgeMagnitude = (float *  )   malloc(ShapeSize * sizeof(float));

				memset(ModelID->m_pShapeInfoPyd2Vec[i].Coordinates,  	    0, ShapeSize * sizeof(CvPoint));
				memset(ModelID->m_pShapeInfoPyd2Vec[i].EdgeDerivativeX, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoPyd2Vec[i].EdgeDerivativeY, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoPyd2Vec[i].EdgeMagnitude, 0, ShapeSize * sizeof(float));
			}

			/* Initial pyd1 image model list */
			Width = Height =  Length >> 1;
			//ShapeSize = EdgeSize;
			ShapeSize = Width * Height;
			AngleStep = ModelID->m_AngleStep << 1;

			if (ModelID->m_pShapeInfoPyd1Vec != NULL)
			{
				free(ModelID->m_pShapeInfoPyd1Vec);
				ModelID->m_pShapeInfoPyd1Vec = NULL;
			}

			AngleNum = 0;
			for (int iAngle = AngleStart; iAngle < AngleStop; iAngle += AngleStep)
			{
				if (iAngle == 0)
					continue;
				AngleNum++;
			}

			if(AngleStart == AngleStop)
			{
				AngleNum += 1;
			}
			else
				AngleNum += 2;

			ModelID->m_pShapeInfoPyd1Vec = (ShapeInfo*)malloc(AngleNum *sizeof(ShapeInfo));
			memset(ModelID->m_pShapeInfoPyd1Vec, 0, AngleNum * sizeof(ShapeInfo));

			if (AngleStart <= 0)
			{
				ModelID->m_pShapeInfoPyd1Vec[0].Angel = AngleStart;
				ModelID->m_pShapeInfoPyd1Vec[AngleNum - 1].Angel = AngleStop;
			}
			else
			{
				ModelID->m_pShapeInfoPyd1Vec[0].Angel = 0;
				ModelID->m_pShapeInfoPyd1Vec[1].Angel = AngleStart;
				ModelID->m_pShapeInfoPyd1Vec[AngleNum - 1].Angel = AngleStop;
			}
			Angle = AngleStart + AngleStep;
			isFilled = false;
			for (int i = 1; i < AngleNum - 1; i++)
			{
				if ((!isFilled) && (Angle >= 0 && (Angle - AngleStep) < 0))
				{
					ModelID->m_pShapeInfoPyd1Vec[i].Angel = 0;
					if (Angle == 0)
					{
						Angle += AngleStep;
					}
					isFilled = true;
				}
				else if (Angle < AngleStop && Angle != 0)
				{
					ModelID->m_pShapeInfoPyd1Vec[i].Angel = Angle;
					Angle += AngleStep;
				}
			}
			for (int i = 0; i < AngleNum; i++) 
			{
				ModelID->m_pShapeInfoPyd1Vec[i].PyLevel = 1;
				ModelID->m_pShapeInfoPyd1Vec[i].AngleNum		  = AngleNum;
				ModelID->m_pShapeInfoPyd1Vec[i].Coordinates	      = (CvPoint *) malloc(ShapeSize * sizeof(CvPoint));
				ModelID->m_pShapeInfoPyd1Vec[i].EdgeDerivativeX = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoPyd1Vec[i].EdgeDerivativeY = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoPyd1Vec[i].EdgeMagnitude = (float *  )   malloc(ShapeSize * sizeof(float));

				memset(ModelID->m_pShapeInfoPyd1Vec[i].Coordinates,  	    0, ShapeSize * sizeof(CvPoint));
				memset(ModelID->m_pShapeInfoPyd1Vec[i].EdgeDerivativeX, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoPyd1Vec[i].EdgeDerivativeY, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoPyd1Vec[i].EdgeMagnitude, 0, ShapeSize * sizeof(float));
			}

			/* Initial source image model list */
			AngleNum = ModelID->m_AngleStop - ModelID->m_AngleStart + 1;
			if (ModelID->m_pShapeInfoTmpVec != NULL)
			{
				free(ModelID->m_pShapeInfoTmpVec);
				ModelID->m_pShapeInfoTmpVec = NULL;
			}

			ModelID->m_pShapeInfoTmpVec = (ShapeInfo*)malloc(AngleNum * sizeof(ShapeInfo));
			memset(ModelID->m_pShapeInfoTmpVec, 0, AngleNum * sizeof(ShapeInfo));
			//ShapeSize = EdgeSize * 2;
			ShapeSize = Length * Length;

			for (int i = 0; i < AngleNum; i++)
			{
				ModelID->m_pShapeInfoTmpVec[i].PyLevel = 0;
				ModelID->m_pShapeInfoTmpVec[i].Angel = AngleStart + i * ModelID->m_AngleStep;
				ModelID->m_pShapeInfoTmpVec[i].AngleNum		 = AngleNum;
				ModelID->m_pShapeInfoTmpVec[i].Coordinates	     = (CvPoint *) malloc(ShapeSize * sizeof(CvPoint));
				ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeX = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeY = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoTmpVec[i].EdgeMagnitude = (float *  ) malloc(ShapeSize * sizeof(float));

				memset(ModelID->m_pShapeInfoTmpVec[i].Coordinates,  	   0, ShapeSize * sizeof(CvPoint));
				memset(ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeX, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeY, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoTmpVec[i].EdgeMagnitude, 0, ShapeSize * sizeof(float));
			}
			break;
		}
	case 1:
		{
			/* Initial pyd1 image model list */
			Width = Height =  Length >> 1;
			//int ShapeSize = EdgeSize;
			int ShapeSize = Width * Height;
			AngleStep = ModelID->m_AngleStep << 1;

			if (ModelID->m_pShapeInfoPyd1Vec != NULL)
			{
				free(ModelID->m_pShapeInfoPyd1Vec);
				ModelID->m_pShapeInfoPyd1Vec = NULL;
			}

			int AngleNum = 0;
			for (int iAngle = AngleStart; iAngle < AngleStop; iAngle += AngleStep)
			{
				if (iAngle == 0)
					continue;
				AngleNum++;
			}

			if(AngleStart == AngleStop)
			{
				AngleNum += 1;
			}
			else
				AngleNum += 2;

			ModelID->m_pShapeInfoPyd1Vec = (ShapeInfo*)malloc(AngleNum *sizeof(ShapeInfo));
			memset(ModelID->m_pShapeInfoPyd1Vec, 0, AngleNum * sizeof(ShapeInfo));

			if (AngleStart <= 0)
			{
				ModelID->m_pShapeInfoPyd1Vec[0].Angel = AngleStart;
				ModelID->m_pShapeInfoPyd1Vec[AngleNum - 1].Angel = AngleStop;
			}
			else
			{
				ModelID->m_pShapeInfoPyd1Vec[0].Angel = 0;
				ModelID->m_pShapeInfoPyd1Vec[1].Angel = AngleStart;
				ModelID->m_pShapeInfoPyd1Vec[AngleNum - 1].Angel = AngleStop;
			}
			int Angle = AngleStart + AngleStep;
			bool isFilled = false;
			for (int i = 1; i < AngleNum - 1; i++)
			{
				if ((!isFilled) && (Angle >= 0 && (Angle - AngleStep) < 0))
				{
					ModelID->m_pShapeInfoPyd1Vec[i].Angel = 0;
					if (Angle == 0)
					{
						Angle += AngleStep;
					}
					isFilled = true;
				}
				else if (Angle < AngleStop && Angle != 0)
				{
					ModelID->m_pShapeInfoPyd1Vec[i].Angel = Angle;
					Angle += AngleStep;
				}
			}
			for (int i = 0; i < AngleNum; i++) 
			{
				ModelID->m_pShapeInfoPyd1Vec[i].PyLevel = 1;
				ModelID->m_pShapeInfoPyd1Vec[i].AngleNum		  = AngleNum;
				ModelID->m_pShapeInfoPyd1Vec[i].Coordinates	      = (CvPoint *) malloc(ShapeSize * sizeof(CvPoint));
				ModelID->m_pShapeInfoPyd1Vec[i].EdgeDerivativeX = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoPyd1Vec[i].EdgeDerivativeY = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoPyd1Vec[i].EdgeMagnitude = (float *  )   malloc(ShapeSize * sizeof(float));

				memset(ModelID->m_pShapeInfoPyd1Vec[i].Coordinates,  	    0, ShapeSize * sizeof(CvPoint));
				memset(ModelID->m_pShapeInfoPyd1Vec[i].EdgeDerivativeX, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoPyd1Vec[i].EdgeDerivativeY, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoPyd1Vec[i].EdgeMagnitude, 0, ShapeSize * sizeof(float));
			}

			/* Initial source image model list */
			AngleNum = ModelID->m_AngleStop - ModelID->m_AngleStart + 1;
			if (ModelID->m_pShapeInfoTmpVec != NULL)
			{
				free(ModelID->m_pShapeInfoTmpVec);
				ModelID->m_pShapeInfoTmpVec = NULL;
			}

			ModelID->m_pShapeInfoTmpVec = (ShapeInfo*)malloc(AngleNum * sizeof(ShapeInfo));
			memset(ModelID->m_pShapeInfoTmpVec, 0, AngleNum * sizeof(ShapeInfo));
			//ShapeSize = EdgeSize * 2;
			ShapeSize = Length * Length;

			for (int i = 0; i < AngleNum; i++)
			{
				ModelID->m_pShapeInfoTmpVec[i].PyLevel = 0;
				ModelID->m_pShapeInfoTmpVec[i].Angel = AngleStart + i * ModelID->m_AngleStep;
				ModelID->m_pShapeInfoTmpVec[i].AngleNum		 = AngleNum;
				ModelID->m_pShapeInfoTmpVec[i].Coordinates	     = (CvPoint *) malloc(ShapeSize * sizeof(CvPoint));
				ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeX = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeY = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoTmpVec[i].EdgeMagnitude = (float *  ) malloc(ShapeSize * sizeof(float));

				memset(ModelID->m_pShapeInfoTmpVec[i].Coordinates,  	   0, ShapeSize * sizeof(CvPoint));
				memset(ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeX, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeY, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoTmpVec[i].EdgeMagnitude, 0, ShapeSize * sizeof(float));
			}

			break;
		}
	case 0:
		{
			/* Initial source image model list */
			int AngleNum = ModelID->m_AngleStop - ModelID->m_AngleStart + 1;
			if (ModelID->m_pShapeInfoTmpVec != NULL)
			{
				free(ModelID->m_pShapeInfoTmpVec);
				ModelID->m_pShapeInfoTmpVec = NULL;
			}

			ModelID->m_pShapeInfoTmpVec = (ShapeInfo*)malloc(AngleNum * sizeof(ShapeInfo));
			memset(ModelID->m_pShapeInfoTmpVec, 0, AngleNum * sizeof(ShapeInfo));
			//int ShapeSize = EdgeSize * 2;
			int ShapeSize = Length * Length;

			for (int i = 0; i < AngleNum; i++)
			{
				ModelID->m_pShapeInfoTmpVec[i].PyLevel = 0;
				ModelID->m_pShapeInfoTmpVec[i].Angel    = AngleStart + i * ModelID->m_AngleStep;
				ModelID->m_pShapeInfoTmpVec[i].AngleNum		 = AngleNum;		
				ModelID->m_pShapeInfoTmpVec[i].Coordinates	     = (CvPoint *) malloc(ShapeSize * sizeof(CvPoint));
				ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeX = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeY = (int16_t *) malloc(ShapeSize * sizeof(int16_t));
				ModelID->m_pShapeInfoTmpVec[i].EdgeMagnitude = (float *  ) malloc(ShapeSize * sizeof(float));

				memset(ModelID->m_pShapeInfoTmpVec[i].Coordinates,  	   0, ShapeSize * sizeof(CvPoint));
				memset(ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeX, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoTmpVec[i].EdgeDerivativeY, 0, ShapeSize * sizeof(int16_t));
				memset(ModelID->m_pShapeInfoTmpVec[i].EdgeMagnitude, 0, ShapeSize * sizeof(float));
			}
			break;
		}
	default:
		break;
	}
}

bool CShapeMatch::release_shape_model(shape_model *ModelID)
{
	if(!ModelID->m_IsInited)
		return false;
	switch(ModelID->m_NumLevels)
	{
	case 3:
		{
			ShapeInfo * pInfoPy3 = NULL;
			pInfoPy3 = ModelID->m_pShapeInfoPyd3Vec;
			for (int i = 0; i < pInfoPy3[0].AngleNum; i++)
			{
				free(pInfoPy3[i].EdgeMagnitude);
				free(pInfoPy3[i].EdgeDerivativeY);
				free(pInfoPy3[i].EdgeDerivativeX);
				free(pInfoPy3[i].Coordinates);
			}
			free(pInfoPy3);
			pInfoPy3 = NULL;

			ShapeInfo * pInfoPy2 = NULL;
			pInfoPy2 = ModelID->m_pShapeInfoPyd2Vec;
			for (int i = 0; i < pInfoPy2[0].AngleNum; i++)
			{
				free(pInfoPy2[i].EdgeMagnitude);
				free(pInfoPy2[i].EdgeDerivativeY);
				free(pInfoPy2[i].EdgeDerivativeX);
				free(pInfoPy2[i].Coordinates);
			}
			free(pInfoPy2);
			pInfoPy2 = NULL;

			ShapeInfo * pInfoPy1 = NULL;
			pInfoPy1 = ModelID->m_pShapeInfoPyd1Vec;
			for (int i = 0; i < pInfoPy1[0].AngleNum; i++)
			{
				free(pInfoPy1[i].EdgeMagnitude);
				free(pInfoPy1[i].EdgeDerivativeY);
				free(pInfoPy1[i].EdgeDerivativeX);
				free(pInfoPy1[i].Coordinates);
			}
			free(pInfoPy1);
			pInfoPy1 = NULL;

			ShapeInfo * pInfoSrc = NULL;
			pInfoSrc = ModelID->m_pShapeInfoTmpVec;
			for (int i = 0; i < pInfoSrc[0].AngleNum; i++)
			{
				free(pInfoSrc[i].EdgeMagnitude);
				free(pInfoSrc[i].EdgeDerivativeY);
				free(pInfoSrc[i].EdgeDerivativeX);
				free(pInfoSrc[i].Coordinates);
			}

			free(pInfoSrc);
			pInfoSrc = NULL;
			break;
		}
	case 2:
		{
			ShapeInfo * pInfoPy2 = NULL;
			pInfoPy2 = ModelID->m_pShapeInfoPyd2Vec;
			for (int i = 0; i < pInfoPy2[0].AngleNum; i++)
			{
				free(pInfoPy2[i].EdgeMagnitude);
				free(pInfoPy2[i].EdgeDerivativeY);
				free(pInfoPy2[i].EdgeDerivativeX);
				free(pInfoPy2[i].Coordinates);
			}
			free(pInfoPy2);
			pInfoPy2 = NULL;

			ShapeInfo * pInfoPy1 = NULL;
			pInfoPy1 = ModelID->m_pShapeInfoPyd1Vec;
			for (int i = 0; i < pInfoPy1[0].AngleNum; i++)
			{
				free(pInfoPy1[i].EdgeMagnitude);
				free(pInfoPy1[i].EdgeDerivativeY);
				free(pInfoPy1[i].EdgeDerivativeX);
				free(pInfoPy1[i].Coordinates);
			}
			free(pInfoPy1);
			pInfoPy1 = NULL;

			ShapeInfo * pInfoSrc = NULL;
			pInfoSrc = ModelID->m_pShapeInfoTmpVec;
			for (int i = 0; i < pInfoSrc[0].AngleNum; i++)
			{
				free(pInfoSrc[i].EdgeMagnitude);
				free(pInfoSrc[i].EdgeDerivativeY);
				free(pInfoSrc[i].EdgeDerivativeX);
				free(pInfoSrc[i].Coordinates);
			}

			free(pInfoSrc);
			pInfoSrc = NULL;

			break;
		}
		case 1:
		{
			ShapeInfo * pInfoPy1 = NULL;
			pInfoPy1 = ModelID->m_pShapeInfoPyd1Vec;
			for (int i = 0; i < pInfoPy1[0].AngleNum; i++)
			{
				free(pInfoPy1[i].EdgeMagnitude);
				free(pInfoPy1[i].EdgeDerivativeY);
				free(pInfoPy1[i].EdgeDerivativeX);
				free(pInfoPy1[i].Coordinates);
			}
			free(pInfoPy1);
			pInfoPy1 = NULL;

			ShapeInfo * pInfoSrc = NULL;
			pInfoSrc = ModelID->m_pShapeInfoTmpVec;
			for (int i = 0; i < pInfoSrc[0].AngleNum; i++)
			{
				free(pInfoSrc[i].EdgeMagnitude);
				free(pInfoSrc[i].EdgeDerivativeY);
				free(pInfoSrc[i].EdgeDerivativeX);
				free(pInfoSrc[i].Coordinates);
			}

			free(pInfoSrc);
			pInfoSrc = NULL;

			break;
		}
		case 0:
			{
				ShapeInfo * pInfo = NULL;
				pInfo = ModelID->m_pShapeInfoTmpVec;
				for (int i = 0; i < pInfo[0].AngleNum; i++)
				{
					free(pInfo[i].EdgeMagnitude);
					free(pInfo[i].EdgeDerivativeY);
					free(pInfo[i].EdgeDerivativeX);
					free(pInfo[i].Coordinates);
				}

				free(pInfo);
				pInfo = NULL;
				break;
			}
	default:
		break;
	}
	ModelID->m_IsInited = false;
	return true;
}

void CShapeMatch::extract_shape_info(uint8_t *ImageData, ShapeInfo *ShapeInfoData, int Contrast, int MinContrast, int PointReduction, uint8_t *MaskImgData)
{
	/* source image size */
	int width  = ShapeInfoData->ImgWidth;
	int height = ShapeInfoData->ImgHeight;

	/* Compute buffer sizes */
	uint32_t  bufferSize  = width * height;

	/* Allocate buffers for each vector */
	uint8_t  *pInput		  = (uint8_t *) malloc(bufferSize * sizeof(uint8_t));
	uint8_t  *pBufOut		  = (uint8_t *) malloc(bufferSize * sizeof(uint8_t));
	int16_t  *pBufGradX   = (int16_t *) malloc(bufferSize * sizeof(int16_t));
	int16_t  *pBufGradY   = (int16_t *) malloc(bufferSize * sizeof(int16_t));
	int32_t	*pBufOrien	 =  (int32_t *) malloc(bufferSize * sizeof(int32_t));
	float	    *pBufMag      = (float *)	 malloc(bufferSize * sizeof(float));


	if( pInput && pBufGradX && pBufGradY && pBufMag && pBufOrien && pBufOut)
	{
		gaussian_filter(ImageData, pInput, width, height);
		//memcpy(pInput, ImageData, bufferSize * sizeof(uint8_t));
		memset(pBufGradX,  0, bufferSize * sizeof(int16_t));
		memset(pBufGradY,  0, bufferSize * sizeof(int16_t));
		memset(pBufOrien,  0, bufferSize * sizeof(int32_t));
		memset(pBufOut,    0, bufferSize * sizeof(uint8_t));
		memset(pBufMag,   0, bufferSize * sizeof(float));

		float MaxGradient=-9999.99f;
		int count = 0,i,j; // count variable;

		for( i = 1; i < width-1; i++ )
		{
			for( j = 1; j < height-1; j++ )
			{ 	
				int16_t sdx =*(pInput + j*width + i + 1) - *(pInput + j*width + i - 1);
				int16_t sdy =*(pInput + (j+1)*width + i ) - *(pInput + (j-1)*width + i);

				if (*(MaskImgData + j*width + i) != 0xff)
				{
					*(pBufGradX + j*width + i) = 0;
					*(pBufGradY + j*width + i) = 0;
				}

				*(pBufGradX + j*width + i) = sdx;
				*(pBufGradY + j*width + i) = sdy;
				float MagG = sqrt((float)(sdx*sdx) + (float)(sdy*sdy));
				*(pBufMag + j*width + i) = MagG;

				// get maximum gradient value for normalizing.
				if(MagG>MaxGradient)
					MaxGradient=MagG; 
			}
		}

		for( i = 1; i < width-1; i++ )
		{
			for( j = 1; j < height-1; j++ )
			{ 		 
				int16_t fdx = *(pBufGradX + j*width + i);
				int16_t fdy = *(pBufGradY + j*width + i);

				float direction =cvFastArctan((float)fdy,(float)fdx);	 //Direction = invtan (Gy / Gx)

				// get closest angle from 0, 45, 90, 135 set
				if ( (direction>0 && direction < 22.5) || (direction >157.5 && direction < 202.5) || (direction>337.5 && direction<360)  )
					direction = 0;
				else if ( (direction>22.5 && direction < 67.5) || (direction >202.5 && direction <247.5)  )
					direction = 45;
				else if ( (direction >67.5 && direction < 112.5)||(direction>247.5 && direction<292.5) )
					direction = 90;
				else if ( (direction >112.5 && direction < 157.5)||(direction>292.5 && direction<337.5) )
					direction = 135;
				else 
					direction = 0;

				pBufOrien[count]  = (int32_t)direction;
				count++;
			}
		}

		count=0; // init count
		// non maximum suppression
		float leftPixel,rightPixel;

		for( i = 1; i < width-1; i++ )
		{
			for( j = 1; j < height-1; j++ )
			{
				switch ( pBufOrien[count] )
				{
				case 0:
					leftPixel   =  *(pBufMag + j*width + i - 1);
					rightPixel = *(pBufMag + j*width + i + 1);
					break;
				case 45:
					leftPixel   =  *(pBufMag + (j-1)*width + i - 1);
					rightPixel = *(pBufMag + (j+1)*width + i + 1);
					break;
				case 90:
					leftPixel   =  *(pBufMag + (j-1)*width + i);
					rightPixel = *(pBufMag + (j+1)*width + i);

					break;
				case 135:
					leftPixel   =  *(pBufMag + (j+1)*width + i - 1);
					rightPixel =  *(pBufMag + (j-1)*width + i + 1);
					break;
				}
				// compare current pixels value with adjacent pixels
				if (( *(pBufMag + j*width + i) < leftPixel ) || (*(pBufMag + j*width + i) < rightPixel ) || (*(MaskImgData + j*width + i) == 0x00))
					{
						*(pBufOut + j*width + i) = 0;
					}
				else
					*(pBufOut + j*width + i) = (uint8_t)(*(pBufMag + j*width + i) / MaxGradient * 255);

				count++;
			}
		}

		int RSum=0,CSum=0;
		int curX,curY;
		int flag=1;
		int n = 0;
		int iPr = PointReduction;
		//Hysteresis threshold
		for( i = 1; i < width-1; i+=iPr )
		{
			for( j = 1; j < height-1; j+=iPr )
			{
				int16_t fdx = *(pBufGradX + j*width + i);
				int16_t fdy = *(pBufGradY + j*width + i);
				float MagG =*(pBufMag   + j*width + i);

				flag=1;
				if((float)*(pBufOut + j*width + i) < Contrast)
				{
					if((float)*(pBufOut + j*width + i) < MinContrast)
					{
						*(pBufOut + j*width + i)=0;
						flag=0; // remove from edge
					}
					else
					{   // if any of 8 neighboring pixel is not greater than max contract remove from edge
						if( ((float)*(pBufOut + (j - 1)*width + i - 1) < Contrast) &&
							((float)*(pBufOut +     j     * width + i - 1) < Contrast)  &&
							((float)*(pBufOut + (j - 1) * width + i - 1) < Contrast) &&
							((float)*(pBufOut + (j - 1) * width + i     ) < Contrast) &&
							((float)*(pBufOut + (j + 1)* width + i     ) < Contrast) &&
							((float)*(pBufOut + (j - 1) * width + i +1) < Contrast) &&
							((float)*(pBufOut +    j     * width + i + 1) < Contrast) &&
							((float)*(pBufOut + (j+1)  * width + i + 1) < Contrast))
						{
							*(pBufOut + j*width + i)=0;
							flag=0;
						}
					}
				}

				// save selected edge information
				curX=i;	curY=j;
				if(flag!=0)
				{
					if(fdx!=0 || fdy!=0)
					{		
						RSum=RSum+curX;	
						CSum=CSum+curY; // Row sum and column sum for center of gravity

						ShapeInfoData->Coordinates[n].x = curX;
						ShapeInfoData->Coordinates[n].y = curY;
						ShapeInfoData->EdgeDerivativeX[n] = fdx;
						ShapeInfoData->EdgeDerivativeY[n] = fdy;

						//handle divide by zero
						if(MagG!=0)
							ShapeInfoData->EdgeMagnitude[n] = 1/MagG;  // gradient magnitude 
						else
							ShapeInfoData->EdgeMagnitude[n] = 0;
						n++;
					}
				}
			}
		}
		if (n != 0)
		{
			ShapeInfoData->NoOfCordinates = n;
			//ShapeInfoData->ReferPoint.x = RSum / n;			 // center of gravity
			//ShapeInfoData->ReferPoint.y = CSum / n;			 // center of gravity
			ShapeInfoData->ReferPoint.x = width / 2;			 // center of image
			ShapeInfoData->ReferPoint.y = height / 2;		     // center of image
		}

		//printf(" ���������%d �������꣺(%d,%d)\n", ShapeInfoData->NoOfCordinates, ShapeInfoData->ReferPoint.x, ShapeInfoData->ReferPoint.y );

		//IplImage* Test = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
		//memcpy((uint8_t*)Test->imageData, pBufOut, width*height * sizeof(uint8_t));
		//char buf[1024];  
		//sprintf_s(buf,"..\\SaveImage\\%d_%d.bmp", width, ShapeInfoData->Angel); 
		//cvSaveImage(buf, Test);
		//cvReleaseImage(&Test);

		// change coordinates to reflect center of reference
		int m, temp;
		for(m = 0; m < ShapeInfoData->NoOfCordinates; m++)
		{
			temp=(ShapeInfoData->Coordinates+m)->x;
			(ShapeInfoData->Coordinates+m)->x = temp - ShapeInfoData->ReferPoint.x ;
			temp=(ShapeInfoData->Coordinates+m)->y;
			(ShapeInfoData->Coordinates+m)->y = temp - ShapeInfoData->ReferPoint.y;
		}
	}

	free(pBufMag);
	free(pBufOrien); 
	free(pBufGradY);
	free(pBufGradX);
	free(pBufOut);
	free(pInput);
}

bool CShapeMatch::build_model_list(ShapeInfo *ShapeInfoVec, uint8_t *ImageData, uint8_t *MaskData, int Width, int Height, int Contrast, int MinContrast, int Granularity)
{
	int BufferSizeSrc = Width * Height;
	int tempLength = (int)(sqrt((float)BufferSizeSrc + (float)BufferSizeSrc) + 10);    //������ת��չͼ��ĳ���
	if ((tempLength & 1) != 0)
	{
		tempLength += 1;
	}
	int DstWidth = tempLength;
	int DstHeight = tempLength;
	int BufferSizeDst = DstWidth * DstHeight;

	uint8_t	*pDstImgData = (uint8_t *)malloc(BufferSizeDst * sizeof(uint8_t));
	memset(pDstImgData, 0, BufferSizeDst);

	uint8_t	*pMaskRotData = (uint8_t *)malloc(BufferSizeDst * sizeof(uint8_t));
	memset(pMaskRotData, 0, BufferSizeDst);

	IplImage* SrcImage = cvCreateImage(cvSize(Width, Height), IPL_DEPTH_8U, 1);
	memcpy((uint8_t*)SrcImage->imageData, ImageData, BufferSizeSrc * sizeof(uint8_t));
	IplImage* DstImage = cvCreateImage(cvSize(DstWidth, DstHeight), IPL_DEPTH_8U, 1);
	memcpy((uint8_t*)DstImage->imageData, pDstImgData, BufferSizeDst * sizeof(uint8_t));

	IplImage* SrcMask = cvCreateImage(cvSize(Width, Height), IPL_DEPTH_8U, 1);
	memcpy((uint8_t*)SrcMask->imageData, MaskData, BufferSizeSrc * sizeof(uint8_t));
	IplImage* DstMask = cvCreateImage(cvSize(DstWidth, DstHeight), IPL_DEPTH_8U, 1);
	memcpy((uint8_t*)DstMask->imageData, pMaskRotData, BufferSizeDst * sizeof(uint8_t));

	int AngleNum = ShapeInfoVec[0].AngleNum;
	for (int i = 0; i < AngleNum; i++)
	{
		ShapeInfoVec[i].ImgWidth  = DstImage->width;
		ShapeInfoVec[i].ImgHeight = DstImage->height;

		rotateImage(SrcImage, DstImage, ShapeInfoVec[i].Angel);
		rotateImage(SrcMask, DstMask, ShapeInfoVec[i].Angel);

		//char buf[1024];  
		//sprintf_s(buf,"..\\SaveImage\\Mask_%d_%d_%d.bmp", ShapeInfoVec[i].PyLevel, ShapeInfoVec[i].Angel, DstWidth); 
		//cvSaveImage(buf, DstImage);

		extract_shape_info((uint8_t*)DstImage->imageData, &ShapeInfoVec[i], Contrast, MinContrast, Granularity, (uint8_t*)DstMask->imageData);

		if(ShapeInfoVec[i].NoOfCordinates == 0)
			return false;
	}
	cvReleaseImage(&SrcImage);
	cvReleaseImage(&DstImage);
	cvReleaseImage(&SrcMask);
	cvReleaseImage(&DstMask);
	free(pMaskRotData);
	free(pDstImgData);

	return true;
}

void CShapeMatch::train_shape_model(IplImage *Image, int Contrast, int MinContrast, int PointReduction, edge_list *EdgeList)
{
	/* source image size */
	int width  = Image->width;
	int height = Image->height;

	/* Compute buffer sizes */
	uint32_t  bufferSize  = width * height;

	/* Allocate buffers for each vector */
	uint8_t  *pInput		  = (uint8_t *) malloc(bufferSize * sizeof(uint8_t));
	uint8_t  *pBufOut		  = (uint8_t *) malloc(bufferSize * sizeof(uint8_t));
	int16_t  *pBufGradX   = (int16_t *) malloc(bufferSize * sizeof(int16_t));
	int16_t  *pBufGradY   = (int16_t *) malloc(bufferSize * sizeof(int16_t));
	int32_t	*pBufOrien	 =  (int32_t *) malloc(bufferSize * sizeof(int32_t));
	float	    *pBufMag      = (float *) malloc(bufferSize * sizeof(float));
	CvPoint  *pEdgePiont  = (CvPoint *) malloc(bufferSize * sizeof(CvPoint));

	if( pInput && pBufGradX && pBufGradY && pBufMag && pBufOrien && pBufOut)
	{
		gaussian_filter((uint8_t*)Image->imageData, pInput, width, height);
		//memcpy(pInput, Image->imageData, bufferSize * sizeof(uint8_t));
		memset(pBufGradX,  0, bufferSize * sizeof(int16_t));
		memset(pBufGradY,  0, bufferSize * sizeof(int16_t));
		memset(pBufOrien,  0, bufferSize * sizeof(int32_t));
		memset(pBufOut,    0, bufferSize * sizeof(uint8_t));
		memset(pBufMag,   0, bufferSize * sizeof(float));
		memset(pEdgePiont,0, bufferSize *sizeof(CvPoint));

		float MaxGradient=-9999.99f;
		int count = 0,i,j; // count variable;
		for( i = 1; i < width-1; i++ )
		{
			for( j = 1; j < height-1; j++ )
			{ 	
				int16_t sdx =*(pInput + j*width + i + 1) - *(pInput + j*width + i - 1);
				int16_t sdy =*(pInput + (j+1)*width + i ) - *(pInput + (j-1)*width + i);
				*(pBufGradX + j*width + i) = sdx;
				*(pBufGradY + j*width + i) = sdy;
				float MagG = sqrt((float)(sdx*sdx) + (float)(sdy*sdy));
				*(pBufMag + j*width + i) = MagG;

				// get maximum gradient value for normalizing.
				if(MagG>MaxGradient)
					MaxGradient=MagG; 
			}
		}

		for( i = 1; i < width - 1; i++ )
		{
			for( j = 1; j < height - 1; j++ )
			{ 		 
				int16_t fdx = *(pBufGradX + j*width + i);
				int16_t fdy = *(pBufGradY + j*width + i);

				float direction =cvFastArctan((float)fdy,(float)fdx);	 //Direction = invtan (Gy / Gx)

				// get closest angle from 0, 45, 90, 135 set
				if ( (direction>0 && direction < 22.5) || (direction >157.5 && direction < 202.5) || (direction>337.5 && direction<360)  )
					direction = 0;
				else if ( (direction>22.5 && direction < 67.5) || (direction >202.5 && direction <247.5)  )
					direction = 45;
				else if ( (direction >67.5 && direction < 112.5) || (direction>247.5 && direction<292.5) )
					direction = 90;
				else if ( (direction >112.5 && direction < 157.5) || (direction>292.5 && direction<337.5) )
					direction = 135;
				else 
					direction = 0;

				pBufOrien[count]  = (int32_t)direction;
				count++;
			}
		}

		// non maximum suppression
		float leftPixel,rightPixel;
		count=0; // init count

		for( i = 1; i < width-1; i++ )
		{
			for( j = 1; j < height-1; j++ )
			{
				switch ( pBufOrien[count] )
				{
				case 0:
					leftPixel   =  *(pBufMag + j*width + i - 1);
					rightPixel = *(pBufMag + j*width + i + 1);
					break;
				case 45:
					leftPixel   =  *(pBufMag + (j-1)*width + i - 1);
					rightPixel = *(pBufMag + (j+1)*width + i + 1);
					break;
				case 90:
					leftPixel   =  *(pBufMag + (j-1)*width + i);
					rightPixel = *(pBufMag + (j+1)*width + i);
					break;
				case 135:
					leftPixel   =  *(pBufMag + (j+1)*width + i - 1);
					rightPixel =  *(pBufMag + (j-1)*width + i + 1);
					break;
				}
				// compare current pixels value with adjacent pixels
				if (( *(pBufMag + j*width + i) < leftPixel ) || (*(pBufMag + j*width + i) < rightPixel ))
					*(pBufOut + j*width + i) = 0;
				else
					*(pBufOut + j*width + i) = (uint8_t)(*(pBufMag + j*width + i) / MaxGradient * 255);

				count++;
			}
		}

		//Hysteresis threshold
		int RSum=0,CSum=0;
		int curX,curY;
		int flag=1;
		int n = 0;
		int iPr = PointReduction;
		for( i = 1; i < width-1; i+=iPr )
		{
			for( j = 1; j < height-1; j+=iPr )
			{
				int16_t fdx = *(pBufGradX + j*width + i);
				int16_t fdy = *(pBufGradY + j*width + i);
				float MagG =*(pBufMag   + j*width + i);
				flag=1;
				if((float)*(pBufOut + j*width + i) < Contrast)
				{
					if((float)*(pBufOut + j*width + i) < MinContrast)
					{
						*(pBufOut + j*width + i)=0;
						flag=0; // remove from edge
					}
					else
					{   // if any of 8 neighboring pixel is not greater than max contract remove from edge
						if( ((float)*(pBufOut + (j-1)   * width + i - 1) < Contrast) &&
							((float)*(pBufOut +     j     * width + i - 1) < Contrast)  &&
							((float)*(pBufOut + (j-1)   * width + i - 1) < Contrast) &&
							((float)*(pBufOut + (j-1)   * width + i     ) < Contrast) &&
							((float)*(pBufOut + (j+1)  * width + i     ) < Contrast) &&
							((float)*(pBufOut + (j-1)   * width + i +1) < Contrast) &&
							((float)*(pBufOut + j        * width + i + 1) < Contrast) &&
							((float)*(pBufOut + (j+1)  * width + i + 1) < Contrast))
						{
							*(pBufOut + j*width + i)=0;
							flag=0;
						}
					}
				}

				// save selected edge information
				curX=i;	curY=j;
				if(flag!=0)
				{
					if(fdx!=0 || fdy!=0)
					{		
						//RSum=RSum+curX;	
						//CSum=CSum+curY; 
						pEdgePiont[n].x = curX;
						pEdgePiont[n].y = curY;

						n++;
					}
				}
			}
		}
		EdgeList->ListSize = n;

		//IplImage* Test = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
		//memcpy((uint8_t*)Test->imageData, pInput, width*height * sizeof(uint8_t));
		//char buf[1024];  
		//sprintf_s(buf,"test.bmp"); 
		//cvSaveImage(buf, Test);
		//cvReleaseImage(&Test);

	   /* Store edge coordinates to struct */
	   memset(EdgeList->EdgePiont, 0, n * sizeof(CvPoint));
       memcpy(EdgeList->EdgePiont, pEdgePiont, n * sizeof(CvPoint));
	}

	/* Free buffers */
	free(pEdgePiont);
	free(pBufMag);
	free(pBufOrien);
	free(pBufGradY);
	free(pBufGradX);
	free(pBufOut);
	free(pInput);
}

bool CShapeMatch::create_shape_model(IplImage *Template, shape_model *ModelID)
{
	/* source image size */
	int ImgWidth  = Template->width;
	int ImgHeight = Template->height;

	/* Resize Image */
	if(ModelID->m_NumLevels >= 0)
	{
		int Length = ConvertLength(MAX(ImgWidth, ImgHeight));
		uint32_t  yOffset = (Length - ImgHeight) >> 1;
		uint32_t  xOffset = (Length - ImgWidth) >> 1;

		IplImage	*ImgBordered = cvCreateImage(cvSize(Length, Length), IPL_DEPTH_8U, 1);
		board_image(Template, ImgBordered, xOffset, yOffset);	//��չͼ��
		//cvCopyMakeBorder(Template, ImgBordered, cvPoint(xOffset, yOffset), IPL_BORDER_REPLICATE);

		IplImage	*ImgMask = cvCreateImage(cvSize(Length, Length), IPL_DEPTH_8U, 1);
		memset(ImgMask->imageData, 0, ImgMask->width * ImgMask->height);	//��ģͼ��

		uint32_t row;
		int ImgMaskWStep = ImgMask->widthStep / sizeof(char);
		for(row = yOffset; row < yOffset + Template->height; row++)
			memset(ImgMask->imageData + row * ImgMaskWStep + xOffset, 0xff, Template->width);

		int BorderedWidth  = ImgBordered->width;
		int BorderedHeight = ImgBordered->height;

		bool IsBuild  = false;
		int Contrast 	= ModelID->m_Contrast;
		int MinContrast = ModelID->m_MinContrast;
		int Granularity  = ModelID->m_Granularity;
		int AngleStart   = ModelID->m_AngleStart;
		int AngleStop   = ModelID->m_AngleStop;

		/* Compute buffer sizes */
		uint32_t  in_size  = BorderedWidth * BorderedHeight;
		uint32_t  out_size = (in_size * 21) >> 6; //in_size / 4 + in_size / 16 + in_size / 64; // same as in_size*21/64

		/* Allocate buffers */
		uint8_t   *pIn     	 = (uint8_t *) malloc(in_size  * sizeof(uint8_t));
		uint8_t   *pOut      = (uint8_t *) malloc(out_size * sizeof(uint8_t));
		uint8_t   *pOutMask  = (uint8_t *) malloc(out_size * sizeof(uint8_t));

		/* Image Pyramid */
		if(pIn && pOut && pOutMask)
		{
			memcpy(pIn, (uint8_t *)(ImgBordered->imageData), in_size * sizeof(uint8_t));
			image_pyramid(pIn, BorderedWidth, BorderedHeight, pOut);

			memcpy(pIn, (uint8_t *)(ImgMask->imageData), in_size * sizeof(uint8_t));
			image_pyramid(pIn, BorderedWidth, BorderedHeight, pOutMask);
		}
		else
			return false;

		/*Build shape model*/
		switch(ModelID->m_NumLevels)
		{
		int Width, Height, BufferSize, AngleStep;
		case 3:
			{
				/*Get pyrmid3 image shape info vector*/
				Width  = BorderedWidth >> 3;
				Height = BorderedHeight >> 3;
				BufferSize = Width*Height;
				AngleStep = ModelID->m_AngleStep << 3;

				uint8_t *pImageDataPy3 = (uint8_t *) malloc(BufferSize);
				memcpy(pImageDataPy3, pOut+in_size*5/16, BufferSize);

				uint8_t	*pMaskDataPy3 = (uint8_t *)malloc(BufferSize);
				memcpy(pMaskDataPy3, pOutMask+in_size*5/16, BufferSize);

				IsBuild = build_model_list(ModelID->m_pShapeInfoPyd3Vec, pImageDataPy3, pMaskDataPy3, Width, Height,
					Contrast, MinContrast, Granularity);

				free(pMaskDataPy3);
				free(pImageDataPy3);

				if(!IsBuild)
					return false;

	    		/*Get pyrmid2 image shape info vector*/
	    		Width  = BorderedWidth >> 2;
	    		Height = BorderedHeight >> 2;
	    		BufferSize = Width * Height;
	    		AngleStep  = ModelID->m_AngleStep << 2;

	    		uint8_t *pImageDataPy2 = (uint8_t *) malloc(BufferSize);
	        	memcpy(pImageDataPy2, pOut+in_size/4, BufferSize);

	        	uint8_t	*pMaskDataPy2 = (uint8_t *)malloc(BufferSize);
	    		memcpy(pMaskDataPy2, pOutMask+in_size/4, BufferSize);

	    		IsBuild = build_model_list(ModelID->m_pShapeInfoPyd2Vec, pImageDataPy2, pMaskDataPy2, Width, Height,
	    				Contrast, MinContrast, Granularity);
	    		if(!IsBuild)
	    			return false;

				free(pMaskDataPy2);
	    		free(pImageDataPy2);

	    		/*Get pyrmid1 image shape info vector*/
	    		Width  = BorderedWidth >> 1;
	    		Height = BorderedHeight >> 1;
	    		BufferSize = Width * Height;
	    		AngleStep  = ModelID->m_AngleStep << 1;

	    		uint8_t *pImageDataPy1 = (uint8_t *) malloc(BufferSize);
	        	memcpy(pImageDataPy1, pOut, BufferSize);

	        	uint8_t	*pMaskDataPy1 = (uint8_t *)malloc(BufferSize);
	    		memcpy(pMaskDataPy1, pOutMask, BufferSize);

	    		IsBuild = build_model_list(ModelID->m_pShapeInfoPyd1Vec, pImageDataPy1, pMaskDataPy1, Width, Height,
	    				Contrast, MinContrast, Granularity);
	    		if(!IsBuild)
	    			return false;

				free(pMaskDataPy1);
	    		free(pImageDataPy1);

				/*Get temple image shape info vector*/
				Width  = BorderedWidth;
				Height = BorderedHeight;
				AngleStep = ModelID->m_AngleStep;

				IsBuild = build_model_list(ModelID->m_pShapeInfoTmpVec, (uint8_t *)(ImgBordered->imageData), (uint8_t *)(ImgMask->imageData),
					Width, Height, Contrast, MinContrast, Granularity);
				if(!IsBuild)
					return false;

				break;
			}
	        case 2:
	        {
				/*Get pyrmid2 image shape info vector*/
				Width  = BorderedWidth >> 2;
				Height = BorderedHeight >> 2;
				BufferSize = Width * Height;
				AngleStep  = ModelID->m_AngleStep << 2;

				uint8_t *pImageDataPy2 = (uint8_t *) malloc(BufferSize);
				memcpy(pImageDataPy2, pOut+in_size/4, BufferSize);

				uint8_t	*pMaskDataPy2 = (uint8_t *)malloc(BufferSize);
				memcpy(pMaskDataPy2, pOutMask+in_size/4, BufferSize);

				IsBuild = build_model_list(ModelID->m_pShapeInfoPyd2Vec, pImageDataPy2, pMaskDataPy2, Width, Height,
					Contrast, MinContrast, Granularity);
				if(!IsBuild)
					return false;

				free(pMaskDataPy2);
				free(pImageDataPy2);

				/*Get pyrmid1 image shape info vector*/
				Width  = BorderedWidth >> 1;
				Height = BorderedHeight >> 1;
				BufferSize = Width * Height;
				AngleStep  = ModelID->m_AngleStep << 1;

				uint8_t *pImageDataPy1 = (uint8_t *) malloc(BufferSize);
				memcpy(pImageDataPy1, pOut, BufferSize);

				uint8_t	*pMaskDataPy1 = (uint8_t *)malloc(BufferSize);
				memcpy(pMaskDataPy1, pOutMask, BufferSize);

				IsBuild = build_model_list(ModelID->m_pShapeInfoPyd1Vec, pImageDataPy1, pMaskDataPy1, Width, Height,
					Contrast, MinContrast, Granularity);
				if(!IsBuild)
					return false;

				free(pMaskDataPy1);
				free(pImageDataPy1);

				/*Get temple image shape info vector*/
				Width  = BorderedWidth;
				Height = BorderedHeight;
				AngleStep = ModelID->m_AngleStep;

				IsBuild = build_model_list(ModelID->m_pShapeInfoTmpVec, (uint8_t *)(ImgBordered->imageData), (uint8_t *)(ImgMask->imageData),
					Width, Height, Contrast, MinContrast, Granularity);
	    		if(!IsBuild)
	    			return false;

	        	break;
	        }
	        case 1:
	        {
				/*Get pyrmid1 image shape info vector*/
				Width  = BorderedWidth >> 1;
				Height = BorderedHeight >> 1;
				BufferSize = Width * Height;
				AngleStep  = ModelID->m_AngleStep << 1;

				uint8_t *pImageDataPy1 = (uint8_t *) malloc(BufferSize);
				memcpy(pImageDataPy1, pOut, BufferSize);

				uint8_t	*pMaskDataPy1 = (uint8_t *)malloc(BufferSize);
				memcpy(pMaskDataPy1, pOutMask, BufferSize);

				IsBuild = build_model_list(ModelID->m_pShapeInfoPyd1Vec, pImageDataPy1, pMaskDataPy1, Width, Height,
					Contrast, MinContrast, Granularity);
				if(!IsBuild)
					return false;

				free(pMaskDataPy1);
				free(pImageDataPy1);

				/*Get temple image shape info vector*/
				Width  = BorderedWidth;
				Height = BorderedHeight;
				AngleStep = ModelID->m_AngleStep;

				IsBuild = build_model_list(ModelID->m_pShapeInfoTmpVec, (uint8_t *)(ImgBordered->imageData), (uint8_t *)(ImgMask->imageData),
					Width, Height, Contrast, MinContrast, Granularity);
				if(!IsBuild)
					return false;

	        	break;
	        }
	        case 0:
	        {
	    		/*Get temple image shape info vector*/
	    		Width  = BorderedWidth;
	    		Height = BorderedHeight;
	    		AngleStep = ModelID->m_AngleStep;

	    		IsBuild = build_model_list(ModelID->m_pShapeInfoTmpVec, (uint8_t *)(ImgBordered->imageData), (uint8_t *)(ImgMask->imageData),
	    				Width, Height, Contrast, MinContrast, Granularity);
	    		if(!IsBuild)
	    			return false;

	        	break;
	        }
		default:
			{
				break;
			}
		}

		free(pOutMask);
		free(pOut);
		free(pIn);
		//cvReleaseImage(&ImgMask);
		//cvReleaseImage(&ImgBordered);
	}
	ModelID->m_IsInited = true;
	return true;
}

void CShapeMatch::shape_match(uint8_t *SearchImage, ShapeInfo *ShapeInfoVec, int Width, int Height, int *NumMatches, int Contrast, int MinContrast, float MinScore, float Greediness, search_region *SearchRegion, MatchResultA *ResultList)
{
	/* source image size */
	int width  = Width;
	int height = Height;

	/* Compute buffer sizes */
	uint32_t  bufferSize  = Width * Height;

	/* Allocate buffers for each vector */
	uint8_t  *pInput		  = (uint8_t *) malloc(bufferSize * sizeof(uint8_t));
	int16_t  *pBufGradX   = (int16_t *) malloc(bufferSize * sizeof(int16_t));
	int16_t  *pBufGradY   = (int16_t *) malloc(bufferSize * sizeof(int16_t));
	float	    *pBufMag      = (float *) malloc(bufferSize * sizeof(float));

	if( pInput && pBufGradX && pBufGradY && pBufMag )
	{
		gaussian_filter(SearchImage, pInput, width, height);
		//memcpy(pInput, SearchImage, bufferSize * sizeof(uint8_t));
		memset(pBufGradX,  0, bufferSize * sizeof(int16_t));
		memset(pBufGradY,  0, bufferSize * sizeof(int16_t));
		memset(pBufMag,    0, bufferSize * sizeof(float));

		int i, j, m; // count variable;

		for( i = 1; i < width-1; i++ )
		{
			for( j = 1; j < height-1; j++ )
			{ 	
				int16_t sdx = *(pInput + j*width + i + 1) - *(pInput + j*width + i - 1);
				int16_t sdy = *(pInput + (j+1)*width + i ) - *(pInput + (j-1)*width + i);
				*(pBufGradX + j*width + i) = sdx;
				*(pBufGradY + j*width + i) = sdy; 
				*(pBufMag   + j*width + i) = new_rsqrt((float)(sdx*sdx) + (float)(sdy*sdy));
			}
		}

		/* Similarity calculation */
		int curX = 0;
		int curY = 0;

		int16_t iTx = 0;
		int16_t iTy = 0;
		int16_t iSx = 0;
		int16_t iSy = 0;
		float   iSm  = 0;
		float   iTm = 0;

		int startX =  SearchRegion->StartX;
		int startY =  SearchRegion->StartY;
		int endX   =  SearchRegion->EndX;
		int endY   =  SearchRegion->EndY;

		int AngleStep  = SearchRegion->AngleStep;
		int AngleStart = SearchRegion->AngleStart;
		int AngleStop = SearchRegion->AngleStop;
		int iAngle = ShapeInfoVec->Angel;

		int   n = 0;
		int   SumOfCoords = 0;
		int   TempPiontX   = 0;
		int   TempPiontY   = 0;
		float PartialSum   = 0;
		float PartialScore = 0;
		float ResultScore  = 0;
		float anMinScore	 = 1 - MinScore;
		float NormMinScore   = 0;
		float NormGreediness = Greediness;

		int resultsNumPerDegree;						//ÿ���Ƕȶ�Ӧ��ƥ������
		int	totalResultsNum = 0;						//���н������
		float    minScoreTemp = 0;
		int offSet = 0;

		for (int k = 0; k < ShapeInfoVec[0].AngleNum; k++)
		{
			if (ShapeInfoVec[k].Angel < AngleStart || ShapeInfoVec[k].Angel > AngleStop)
				continue;
			resultsNumPerDegree = 0;
			minScoreTemp = 0;
			ResultScore = 0;
			NormMinScore = MinScore / ShapeInfoVec[k].NoOfCordinates;
			NormGreediness = ((1- Greediness * MinScore)/(1-Greediness)) /ShapeInfoVec[k].NoOfCordinates;
			for(i = startX; i < endX; i++)
			{
				for(j = startY; j < endY; j++)
				{
					PartialSum = 0;
					for(m = 0; m < ShapeInfoVec[k].NoOfCordinates; m++)
					{
						curX = i + (ShapeInfoVec[k].Coordinates + m)->x ;		// template X coordinate
						curY = j + (ShapeInfoVec[k].Coordinates + m)->y ; 		// template Y coordinate
						iTx	= *(ShapeInfoVec[k].EdgeDerivativeX + m);		    // template X derivative
						iTy	= *(ShapeInfoVec[k].EdgeDerivativeY + m);    		// template Y derivative
						iTm   = *(ShapeInfoVec[k].EdgeMagnitude + m);			// template gradients magnitude

						if(curX < 0 ||curY < 0||curX > width-1 ||curY > height-1)
							continue;
						offSet = curY*width + curX;
						iSx = *(pBufGradX + offSet);			// get corresponding  X derivative from source image
						iSy = *(pBufGradY + offSet);			// get corresponding  Y derivative from source image
						iSm = *(pBufMag   + offSet);			// get gradients magnitude from source image

						if((iSx != 0 || iSy != 0) && (iTx != 0 || iTy != 0))
						{
							PartialSum = PartialSum + ((iSx * iTx) + (iSy * iTy)) * (iTm * iSm);		// calculate similarity
						}
						SumOfCoords = m + 1;
						PartialScore = PartialSum / SumOfCoords;															// Normalized
						if( PartialScore < (MIN(anMinScore + NormGreediness * SumOfCoords, NormMinScore * SumOfCoords)))
							break;
						//if( PartialScore <  NormMinScore * SumOfCoords)
						//	break;
					}

					if (PartialScore > MinScore)
					{
						int Angle = ShapeInfoVec[k].Angel;
						bool hasFlag = false;
						for(int n = 0; n < resultsNumPerDegree; n++)
						{		//�����Ѽ�¼�Ľ��
							if(abs(resultsPerDeg[n].CenterLocX - i) < 5 && abs(resultsPerDeg[n].CenterLocY - j) < 5)
							{	//���Ѽ�¼����뵱ǰ�������λ�����ʱ������Ϊ��ͬλ�ý�����ٸ���ƥ��ֵ��Сѡ�����滻
								hasFlag = true;
								if(resultsPerDeg[n].ResultScore < PartialScore)
								{	//����ǰ���ƥ��ֵ�����Ѽ�¼���ƥ��ֵʱ���滻�Ѽ�¼���
									resultsPerDeg[n].Angel = Angle;
									resultsPerDeg[n].CenterLocX = i;
									resultsPerDeg[n].CenterLocY = j;
									resultsPerDeg[n].ResultScore = PartialScore;
									break;
								}
							}
						}
						if(!hasFlag)
						{	//���Ѽ�¼�������������ƥ��Ľ��ʱ�����汾��ƥ����
							resultsPerDeg[resultsNumPerDegree].Angel = Angle;
							resultsPerDeg[resultsNumPerDegree].CenterLocX = i;
							resultsPerDeg[resultsNumPerDegree].CenterLocY = j;
							resultsPerDeg[resultsNumPerDegree].ResultScore = PartialScore;
							resultsNumPerDegree ++;
						}
						minScoreTemp = minScoreTemp < PartialScore ? PartialScore : minScoreTemp;	//������Сƥ��ֵ
					}
				}
			}

			//����ĳһ�Ƕȵ�ģ�壬ƥ������������������totalResultsTemp��
			for(int i = 0; i < resultsNumPerDegree; i++)
			{
				totalResultsTemp[totalResultsNum].Angel = resultsPerDeg[i].Angel;
				totalResultsTemp[totalResultsNum].CenterLocX = resultsPerDeg[i].CenterLocX;
				totalResultsTemp[totalResultsNum].CenterLocY = resultsPerDeg[i].CenterLocY;
				totalResultsTemp[totalResultsNum].ResultScore = resultsPerDeg[i].ResultScore;
				totalResultsNum++;
			}
			n++;
		}

		//�����н������ɸѡ��ɸѡ�ĳ��Ľ���洢��ResultList��
		int resultsCounter = 0;
		bool hasFlag = false;
		for(int i = 0; i < totalResultsNum; i++)
		{	//���������Ѽ�¼�Ľ��
			hasFlag = false;
			for(int j = 0; j < resultsCounter; j++)
			{	//����������ѡ��Ľ��
				if(abs((ResultList + j)->CenterLocX - totalResultsTemp[i].CenterLocX) < 5 && abs((ResultList + j)->CenterLocY - totalResultsTemp[i].CenterLocY) < 5)
				{	//�����λ�����ʱ����ѡ��һ���������
					hasFlag = true;
					if(totalResultsTemp[i].ResultScore > (ResultList + j)->ResultScore)
					{	//��δ����Ľ������ʱ�������滻
						(ResultList + j)->Angel = totalResultsTemp[i].Angel;
						(ResultList + j)->CenterLocX = totalResultsTemp[i].CenterLocX;
						(ResultList + j)->CenterLocY = totalResultsTemp[i].CenterLocY;
						(ResultList + j)->ResultScore = totalResultsTemp[i].ResultScore;
						break;
					}
				}
			}
			if(!hasFlag)
			{	//����ѡ����������û�����ƽ��ʱ��ѡ��˽������
				(ResultList + resultsCounter)->Angel = totalResultsTemp[i].Angel;
				(ResultList + resultsCounter)->CenterLocX = totalResultsTemp[i].CenterLocX;
				(ResultList + resultsCounter)->CenterLocY = totalResultsTemp[i].CenterLocY;
				(ResultList + resultsCounter)->ResultScore = totalResultsTemp[i].ResultScore;
				resultsCounter++;
			}
		}
		*NumMatches = resultsCounter;
	}
	free(pBufMag);
	free(pBufGradY);
	free(pBufGradX);
	free(pInput);
}

void CShapeMatch::shape_match_accurate(uint8_t *SearchImage, ShapeInfo *ShapeInfoVec, int Width, int Height, int Contrast, int MinContrast, float MinScore, float Greediness, search_region *SearchRegion, MatchResultA *ResultList)
{
	/* source image size */
	int width  = Width;
	int height = Height;

	/* Compute buffer sizes */
	uint32_t  bufferSize  = Width * Height;

	/* Allocate buffers for each vector */
	uint8_t  *pInput		  = (uint8_t *) malloc(bufferSize * sizeof(uint8_t));
	int16_t  *pBufGradX   = (int16_t *) malloc(bufferSize * sizeof(int16_t));
	int16_t  *pBufGradY   = (int16_t *) malloc(bufferSize * sizeof(int16_t));
	float	    *pBufMag      = (float *) malloc(bufferSize * sizeof(float));

	if( pInput && pBufGradX && pBufGradY && pBufMag )
	{
		//gaussian_filter(SearchImage, pInput, width, height);
		memcpy(pInput, SearchImage, bufferSize * sizeof(uint8_t));
		memset(pBufGradX,  0, bufferSize * sizeof(int16_t));
		memset(pBufGradY,  0, bufferSize * sizeof(int16_t));
		memset(pBufMag,    0, bufferSize * sizeof(float));

		int i, j, m; // count variable;

		for( i = 1; i < width-1; i++ )
		{
			for( j = 1; j < height-1; j++ )
			{ 	
				int16_t sdx = *(pInput + j*width + i + 1) - *(pInput + j*width + i - 1);
				int16_t sdy = *(pInput + (j+1)*width + i ) - *(pInput + (j-1)*width + i);
				*(pBufGradX + j*width + i) = sdx;
				*(pBufGradY + j*width + i) = sdy; 
				*(pBufMag   + j*width + i) = new_rsqrt((float)(sdx*sdx) + (float)(sdy*sdy));
			}
		}

		/* Similarity calculation */
		int curX = 0;
		int curY = 0;

		int16_t iTx = 0;
		int16_t iTy = 0;
		int16_t iSx = 0;
		int16_t iSy = 0;
		float   iSm  = 0;
		float   iTm = 0;

		int startX =  SearchRegion->StartX;
		int startY =  SearchRegion->StartY;
		int endX   =  SearchRegion->EndX;
		int endY   =  SearchRegion->EndY;

		int AngleStep  = SearchRegion->AngleStep;
		int AngleStart = SearchRegion->AngleStart;
		int AngleStop = SearchRegion->AngleStop;
		int iAngle = ShapeInfoVec->Angel;

		int   SumOfCoords  = 0;
		int   TempPiontX   = 0;
		int   TempPiontY   = 0;
		float PartialSum   = 0;
		float PartialScore = 0;
		float ResultScore  = 0;
		float TempScore   = 0;
		float anMinScore	 = 1 - MinScore;
		float NormMinScore   = 0;
		float NormGreediness = Greediness;

		for (int k = 0; k < ShapeInfoVec[0].AngleNum; k++)
		{
			if (ShapeInfoVec[k].Angel < AngleStart || ShapeInfoVec[k].Angel > AngleStop)
				continue;

			ResultScore = 0;
			NormMinScore = MinScore / ShapeInfoVec[k].NoOfCordinates;
			NormGreediness = ((1- Greediness * MinScore)/(1-Greediness)) /ShapeInfoVec[k].NoOfCordinates;
			for(i = startX; i < endX; i++)
			{
				for(j = startY; j < endY; j++)
				{
					PartialSum = 0;
					for(m = 0; m < ShapeInfoVec[k].NoOfCordinates; m++)
					{
						curX = i + (ShapeInfoVec[k].Coordinates + m)->x ;		// template X coordinate
						curY = j + (ShapeInfoVec[k].Coordinates + m)->y ; 		// template Y coordinate
						iTx	= *(ShapeInfoVec[k].EdgeDerivativeX + m);		    // template X derivative
						iTy	= *(ShapeInfoVec[k].EdgeDerivativeY + m);    		// template Y derivative
						iTm   = *(ShapeInfoVec[k].EdgeMagnitude + m);			// template gradients magnitude

						if(curX < 0 ||curY < 0||curX > width-1 ||curY > height-1)
							continue;

						iSx = *(pBufGradX + curY*width + curX);			// get corresponding  X derivative from source image
						iSy = *(pBufGradY + curY*width + curX);			// get corresponding  Y derivative from source image
						iSm= *(pBufMag   + curY*width + curX);			// get gradients magnitude from source image

						if((iSx != 0 || iSy != 0) && (iTx != 0 || iTy != 0))
						{
							PartialSum = PartialSum + ((iSx * iTx) + (iSy * iTy)) * (iTm * iSm);// calculate similarity
						}
						SumOfCoords = m + 1;
						PartialScore = PartialSum / SumOfCoords;															// Normalized
						if( PartialScore < (MIN(anMinScore + NormGreediness * SumOfCoords, NormMinScore * SumOfCoords)))
							break;
					}

					if(PartialScore > ResultScore)
					{
						ResultScore = PartialScore;		// Match score
						TempPiontX  = i;						// result coordinate X
						TempPiontY  = j;						// result coordinate Y
					}
				}
			}

			if (ResultScore > TempScore)
			{
				TempScore = ResultScore;
				ResultList->ResultScore = TempScore;
				ResultList->Angel = ShapeInfoVec[k].Angel;
				ResultList->CenterLocX = TempPiontX;
				ResultList->CenterLocY = TempPiontY;
			}
		}
	}
	free(pBufMag);
	free(pBufGradY);
	free(pBufGradX);
	free(pInput);
}

void CShapeMatch::find_shape_model(IplImage *Image, shape_model *ModelID, float MinScore, int NumMatches, float Greediness, MatchResultA *ResultList)
{
	/* source image size */
	int ImgWidth  = Image->width;
	int ImgHeight = Image->height;

	/* Resize Image */
	if(ModelID->m_NumLevels >= 0)
	{
		int width, height;
		int xOffset = 0;
		int yOffset = 0;
		uint8_t *pData = NULL;
		IplImage *ImgBordered;
		bool isBordred = false;

		if((ImgWidth % 16 != 0 ) && (ImgHeight % 16 != 0))
		{
			int BorderedWidth  = ConvertLength(ImgWidth);
			int BorderedHeight = ConvertLength(ImgHeight);

			xOffset = (BorderedWidth - ImgWidth) >> 1;
			yOffset = (BorderedHeight - ImgHeight) >> 1;

			ImgBordered = cvCreateImage(cvSize(BorderedWidth, BorderedHeight), IPL_DEPTH_8U, 1);
			board_image(Image, ImgBordered, xOffset, yOffset);
			isBordred = true;
			width  = BorderedWidth;
			height = BorderedHeight;
			pData  = (uint8_t *)ImgBordered->imageData;
		}
		else
		{
			width  = ImgWidth;
			height = ImgHeight;
			pData  = (uint8_t *)Image->imageData;
		}

		float SocoreMax = 0;
		int MatchAngle  = 0;
		int MatchPiontX = 0;
		int MatchPiontY = 0;
		int cropImgW	= 0;
		int cropImgH	= 0;
		int WidthPy		= 0; 
		int HeightPy     = 0;
		int Contrast	= ModelID->m_Contrast;
		int MinContrast = ModelID->m_MinContrast;
		int Row1, Col1, Row2, Col2, ResultPiontX, ResultPiontY, ReferPointX, ReferPointY;

		search_region *SearchRegion = (search_region*)malloc(sizeof(search_region));
		memset(SearchRegion, 0, sizeof(search_region));

		/* Compute buffer sizes */
		uint32_t   in_size  = width * height;
		uint32_t   out_size = (in_size*21)/64; //in_size / 4 + in_size / 16 + in_size / 64;

		/* Allocate buffers */
		uint8_t   *pIn    = (uint8_t *)malloc(in_size  * sizeof(uint8_t));
		uint8_t   *pOut = (uint8_t *)malloc(out_size * sizeof(uint8_t));

		/* Image Pyramid */
		if(pIn && pOut )
		{
			memcpy(pIn, pData, in_size  * sizeof(uint8_t));
			image_pyramid(pIn, width, height, pOut);
		}

		switch(ModelID->m_NumLevels)
		{
		case 3:
			{
				uint8_t	  *pImagePy1 = (uint8_t *) malloc((in_size/4) * sizeof(uint8_t));
				memcpy(pImagePy1, pOut, in_size/4);

				uint8_t	  *pImagePy2 = (uint8_t *) malloc((in_size/16) * sizeof(uint8_t));
				memcpy(pImagePy2, pOut + in_size/4, in_size/16);

				uint8_t   *pImagePy3 = (uint8_t *) malloc((in_size/64) * sizeof(uint8_t));
				memcpy(pImagePy3, pOut + in_size * 5/16, in_size/64);

				//IplImage	*PyImage = cvCreateImage(cvSize(WidthPy, HeightPy), IPL_DEPTH_8U, 1);
				//memcpy((uint8_t*)PyImage->imageData, pImagePy3, WidthPy*HeightPy);

				//if (isBordred)
				//{
				//	CvSize size1= cvSize(ImgWidth >> 3, ImgHeight >>3);//�����С
				//	int RoiRow = (WidthPy - (ImgWidth >> 3)) / 2;
				//	int RoiCol  = (HeightPy- (ImgHeight >>3)) / 2;
				//	IplImage	*ImageRoi = cvCreateImage(size1, IPL_DEPTH_8U, 1);
				//	gen_rectangle(PyImage, ImageRoi, RoiRow, RoiCol);
				//}

				WidthPy  = width >> 3;
				HeightPy = height >> 3;
				/* Set search region */
				SearchRegion->StartX = (ModelID->m_pShapeInfoPyd3Vec[0].ReferPoint.x >> 1) + (xOffset >> 3);
				SearchRegion->StartY = (ModelID->m_pShapeInfoPyd3Vec[0].ReferPoint.y >> 1) + (yOffset >> 3);
				SearchRegion->EndX   = WidthPy - SearchRegion->StartX;
				SearchRegion->EndY   = HeightPy - SearchRegion->StartY;

				SearchRegion->AngleRange = ModelID->m_pShapeInfoPyd3Vec[0].AngleNum;
				SearchRegion->AngleStart   = ModelID->m_AngleStart;
				SearchRegion->AngleStop   = ModelID->m_AngleStop;
				SearchRegion->AngleStep   = ModelID->m_AngleStep << 3;

				/* Find shape model in pyramid3 image */
				MatchResultA ResultListPy3[MAXTARGETNUM];
				memset(ResultListPy3, 0, MAXTARGETNUM * sizeof(MatchResultA));

				if(ModelID->m_pShapeInfoPyd3Vec == NULL)
					return;
				int TargetNum = 0;
				shape_match(pImagePy3, ModelID->m_pShapeInfoPyd3Vec, WidthPy, HeightPy, &TargetNum,
					Contrast, MinContrast, MinScore, Greediness, SearchRegion, ResultListPy3);

				 /*------------------------------------------------------------------ */
				int n = 0;
				int OffSet = 0;
				for (int i = 0; i < MAXTARGETNUM; i++)
				{
					MatchPiontX = ResultListPy3[i].CenterLocX;
					MatchPiontY = ResultListPy3[i].CenterLocY;
					MatchAngle  = ResultListPy3[i].Angel;
					SocoreMax	= ResultListPy3[i].ResultScore;

					if (SocoreMax == 0)
						break;
					if (SocoreMax <= MinScore)
						continue;
					IplImage	*SearchImage, *cropImage;

					//Search model in pyramid2 image
					//WidthPy  = width >> 2;
					//HeightPy = height >> 2;

					//ResultPiontX = ((MatchPiontX << 1) < 0) ? 0 : (MatchPiontX << 1);
					//ResultPiontY = ((MatchPiontY << 1 )< 0) ? 0 : (MatchPiontY << 1);

					////ReferPointX  = (ModelID->m_pShapeInfoPyd2Vec[0].ImgWidth >> 1);
					////ReferPointY  = (ModelID->m_pShapeInfoPyd2Vec[0].ImgHeight >> 1);

					//ReferPointX  = (ModelID->m_ImageWidth >> 3);
					//ReferPointY  = (ModelID->m_ImageHeight >> 3);

					//Row1 = ((ResultPiontX - ReferPointX - 2) < 0) ? 0 : (ResultPiontX - ReferPointX - 2);
					//Col1  = ((ResultPiontY - ReferPointY - 2) < 0) ? 0 : (ResultPiontY - ReferPointY - 2);
					//Row2 = ((ResultPiontX + ReferPointX + 2) > WidthPy) ? WidthPy : (ResultPiontX + ReferPointX + 2);
					//Col2  = ((ResultPiontY + ReferPointY + 2) > HeightPy) ? HeightPy : (ResultPiontY + ReferPointY + 2);
					//
					///* Set accurate match image */
					//cropImgW = abs(Row1 - Row2);
					//cropImgW = ((cropImgW & 1) == 0) ? cropImgW : (cropImgW + 1);
					//cropImgH = abs(Col1 - Col2);
					//cropImgH = ((cropImgH & 1) == 0) ? cropImgH : (cropImgH + 1);

					//SearchImage = cvCreateImage(cvSize(WidthPy, HeightPy), IPL_DEPTH_8U, 1);
					//memcpy((uint8_t*)SearchImage->imageData, pImagePy2, WidthPy*HeightPy);
					//cropImage = cvCreateImage(cvSize(cropImgW, cropImgH), IPL_DEPTH_8U, 1);
					//gen_rectangle(SearchImage, cropImage, Row1, Col1);

					//SearchRegion->StartX = ((ResultPiontX - Row1 - 2) < 0) ? 0 : (ResultPiontX - Row1 - 2);
					//SearchRegion->StartY = ((ResultPiontY - Col1 - 2) < 0) ? 0 : (ResultPiontY - Col1 - 2);
					//SearchRegion->EndX   = SearchRegion->StartX + 4;
					//SearchRegion->EndY   = SearchRegion->StartY + 4;

					//SearchRegion->AngleRange = ModelID->m_pShapeInfoPyd2Vec[0].AngleNum;
					//SearchRegion->AngleStart   = ((MatchAngle - OffSet) < ModelID->m_AngleStart) ?  ModelID->m_AngleStart : (MatchAngle - 4);
					//SearchRegion->AngleStop   = ((MatchAngle + OffSet) > ModelID->m_AngleStop) ?  ModelID->m_AngleStop : (MatchAngle + 4);
					//SearchRegion->AngleStep   = ModelID->m_AngleStep << 2;

					///* Find shape model in pyramid2 image */
					//if(ModelID->m_pShapeInfoPyd2Vec == NULL)
					//	return;

					//MatchResultA ResultPy2;
					//shape_match_accurate((uint8_t*)cropImage->imageData, ModelID->m_pShapeInfoPyd2Vec, cropImgW, cropImgH,
					//	Contrast, MinContrast, MinScore, Greediness, SearchRegion, &ResultPy2);

					//MatchPiontX = ResultPy2.CenterLocX + Row1;
					//MatchPiontY = ResultPy2.CenterLocY + Col1;
					//MatchAngle  = ResultPy2.Angel;
					//SocoreMax	= ResultPy2.ResultScore;
					//if (SocoreMax  < MinScore * 0.8)
					//	continue;

					//cvReleaseImage(&cropImage);
					//cvReleaseImage(&SearchImage);
					/*------------------------------------------------------------------ */
					//Search model in pyramid1 image
					WidthPy  = width >> 1;
					HeightPy = height >> 1;

					ResultPiontX = ((MatchPiontX << 2) < 0) ? 0 : (MatchPiontX << 2);
					ResultPiontY = ((MatchPiontY << 2 )< 0) ? 0 : (MatchPiontY << 2);

					ReferPointX  = ModelID->m_ImageWidth >> 2;
					ReferPointY  = ModelID->m_ImageHeight >> 2;

					//ReferPointX  = (ModelID->m_pShapeInfoPyd1Vec[0].ImgWidth >> 1);
					//ReferPointY  = (ModelID->m_pShapeInfoPyd1Vec[0].ImgHeight >> 1);

					Row1 = ((ResultPiontX - ReferPointX - 4) < 0) ? 0 : (ResultPiontX - ReferPointX - 4);
					Col1  = ((ResultPiontY - ReferPointY - 4) < 0) ? 0 : (ResultPiontY - ReferPointY - 4);
					Row2 = ((ResultPiontX + ReferPointX + 4) > WidthPy) ? WidthPy : (ResultPiontX + ReferPointX + 4);
					Col2  = ((ResultPiontY + ReferPointY + 4) > HeightPy) ? HeightPy : (ResultPiontY + ReferPointY + 4);

					/* Set accurate match image */
					cropImgW = abs(Row1 - Row2);
					cropImgW = ((cropImgW & 1) == 0) ? cropImgW : (cropImgW + 1);
					cropImgH = abs(Col1 - Col2);
					cropImgH = ((cropImgH & 1) == 0) ? cropImgH : (cropImgH + 1);

					SearchImage = cvCreateImage(cvSize(WidthPy, HeightPy), IPL_DEPTH_8U, 1);
					memcpy((uint8_t*)SearchImage->imageData, pImagePy1, WidthPy*HeightPy);
					cropImage = cvCreateImage(cvSize(cropImgW, cropImgH), IPL_DEPTH_8U, 1);
					gen_rectangle(SearchImage, cropImage, Row1, Col1);

					SearchRegion->StartX = ((ResultPiontX - Row1 - 4) < 0) ? 0 : (ResultPiontX - Row1 - 4);
					SearchRegion->StartY = ((ResultPiontY - Col1 - 4) < 0) ? 0 : (ResultPiontY - Col1 - 4);
					SearchRegion->EndX   = SearchRegion->StartX + 8;
					SearchRegion->EndY   = SearchRegion->StartY + 8;

					SearchRegion->AngleRange = ModelID->m_pShapeInfoPyd1Vec[0].AngleNum;
					SearchRegion->AngleStart   = ((MatchAngle - OffSet) < ModelID->m_AngleStart) ?  ModelID->m_AngleStart : (MatchAngle - 4);
					SearchRegion->AngleStop   = ((MatchAngle + OffSet) > ModelID->m_AngleStop) ?  ModelID->m_AngleStop : (MatchAngle + 4);
					SearchRegion->AngleStep   = ModelID->m_AngleStep << 1;

					/* Find shape model in pyramid1 image */
					if(ModelID->m_pShapeInfoPyd1Vec == NULL)
						return;

					MatchResultA ResultPy1;
					shape_match_accurate((uint8_t*)cropImage->imageData, ModelID->m_pShapeInfoPyd1Vec, cropImgW, cropImgH,
						Contrast, MinContrast, MinScore, Greediness, SearchRegion, &ResultPy1);

					MatchPiontX = ResultPy1.CenterLocX + Row1;
					MatchPiontY = ResultPy1.CenterLocY + Col1;
					MatchAngle  = ResultPy1.Angel;
					SocoreMax	= ResultPy1.ResultScore;
					if (SocoreMax  < MinScore)
						continue;

					cvReleaseImage(&cropImage);
					cvReleaseImage(&SearchImage);
					/*------------------------------------------------------------------ */

					//Search model in source image
					//ResultPiontX = ((MatchPiontX << 3) < 0) ? 0 : (MatchPiontX << 3);
					//ResultPiontY = ((MatchPiontY << 3 )< 0) ? 0 : (MatchPiontY << 3);

					ResultPiontX = ((MatchPiontX << 1) < 0) ? 0 : (MatchPiontX << 1);
					ResultPiontY = ((MatchPiontY << 1 )< 0) ? 0 : (MatchPiontY << 1);

					ReferPointX  = ModelID->m_ImageWidth >> 1;
					ReferPointY  = ModelID->m_ImageHeight >> 1;

					//ReferPointX  = (ModelID->m_pShapeInfoTmpVec[0].ImgWidth >> 1);
					//ReferPointY  = (ModelID->m_pShapeInfoTmpVec[0].ImgHeight >> 1);

					Row1 = ((ResultPiontX - ReferPointX - 2) < 0) ? 0 : (ResultPiontX - ReferPointX - 2);
					Col1  = ((ResultPiontY - ReferPointY - 2) < 0) ? 0 : (ResultPiontY - ReferPointY - 2);
					Row2 = ((ResultPiontX + ReferPointX + 2) > width) ? width : (ResultPiontX + ReferPointX + 2);
					Col2  = ((ResultPiontY + ReferPointY + 2) > height) ? height : (ResultPiontY + ReferPointY + 2);

					/* Set accurate match image */
					cropImgW = abs(Row1 - Row2);
					cropImgW = ((cropImgW & 1) == 0) ? cropImgW :  (cropImgW + 1);
					cropImgH = abs(Col1 - Col2);
					cropImgH = ((cropImgH & 1) == 0) ? cropImgH :  (cropImgH + 1);

					SearchImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
					memcpy((uint8_t*)SearchImage->imageData, pData, width*height);
					cropImage = cvCreateImage(cvSize(cropImgW, cropImgH), IPL_DEPTH_8U, 1);
					gen_rectangle(SearchImage, cropImage, Row1, Col1);
					//cvSaveImage("cropImage.bmp", cropImage);

					SearchRegion->StartX = ((ResultPiontX - Row1 - 2) < 0) ? 0 : (ResultPiontX - Row1 - 2);
					SearchRegion->StartY = ((ResultPiontY - Col1 - 2) < 0) ? 0 : (ResultPiontY - Col1 - 2);
					SearchRegion->EndX   = SearchRegion->StartX + 4;
					SearchRegion->EndY   = SearchRegion->StartY + 4;

					SearchRegion->AngleRange = ModelID->m_pShapeInfoTmpVec[0].AngleNum;
					SearchRegion->AngleStart   = ((MatchAngle - OffSet) < ModelID->m_AngleStart) ?  ModelID->m_AngleStart : (MatchAngle - 2);
					SearchRegion->AngleStop   = ((MatchAngle + OffSet) > ModelID->m_AngleStop) ?  ModelID->m_AngleStop : (MatchAngle + 2);
					SearchRegion->AngleStep   = ModelID->m_AngleStep;

					/* Find shape model in source image */
					if(ModelID->m_pShapeInfoTmpVec == NULL)
						return;

					MatchResultA Result;
					shape_match_accurate((uint8_t*)cropImage->imageData, ModelID->m_pShapeInfoTmpVec, cropImgW, cropImgH,
						Contrast, MinContrast, MinScore, Greediness, SearchRegion, &Result);

					if (Result.ResultScore > MinScore)
					{
						ResultList[n].Angel = Result.Angel;
						ResultList[n].ResultScore = Result.ResultScore;
						ResultList[n].CenterLocX = Result.CenterLocX + Row1 - xOffset;
						ResultList[n].CenterLocY = Result.CenterLocY + Col1 - yOffset;
						n++;
					}
					cvReleaseImage(&cropImage);
					cvReleaseImage(&SearchImage);
				}

				free(pImagePy3);
				free(pImagePy2);
				free(pImagePy1);
				break;
			}
		case 2:
			{
				uint8_t	  *pImagePy1 = (uint8_t *) malloc((in_size/4) * sizeof(uint8_t));
				memcpy(pImagePy1, pOut, in_size/4);

				uint8_t	  *pImagePy2 = (uint8_t *) malloc((in_size/16) * sizeof(uint8_t));
				memcpy(pImagePy2, pOut + in_size/4, in_size/16);

				WidthPy  = width >> 2;
				HeightPy = height >> 2;

				SearchRegion->StartX = (ModelID->m_pShapeInfoPyd2Vec[0].ReferPoint.x >> 1) + (xOffset >> 2);
				SearchRegion->StartY = (ModelID->m_pShapeInfoPyd2Vec[0].ReferPoint.y >> 1) + (yOffset >> 2);
				SearchRegion->EndX   = WidthPy - SearchRegion->StartX;
				SearchRegion->EndY   = HeightPy - SearchRegion->StartY;

				SearchRegion->AngleRange = ModelID->m_pShapeInfoPyd2Vec[0].AngleNum;
				SearchRegion->AngleStart   = ModelID->m_AngleStart;
				SearchRegion->AngleStop   = ModelID->m_AngleStop;
				SearchRegion->AngleStep   = ModelID->m_AngleStep << 2;

				/* Find shape model in pyramid2 image */
				MatchResultA ResultListPy2[MAXTARGETNUM];
				memset(ResultListPy2, 0, MAXTARGETNUM * sizeof(MatchResultA));

				if(ModelID->m_pShapeInfoPyd2Vec == NULL)
					return;

				int TargetNum = 0;
				shape_match(pImagePy2, ModelID->m_pShapeInfoPyd2Vec, WidthPy, HeightPy, &TargetNum,
					Contrast, MinContrast, MinScore, Greediness, SearchRegion, ResultListPy2);

				/*------------------------------------------------------------------ */
				int n = 0;
				int OffSet = 0;
				for (int i = 0; i < MAXTARGETNUM; i++)
				{
					MatchPiontX = ResultListPy2[i].CenterLocX;
					MatchPiontY = ResultListPy2[i].CenterLocY;
					MatchAngle  = ResultListPy2[i].Angel;
					SocoreMax	= ResultListPy2[i].ResultScore;

					if (SocoreMax == 0)
						break;
					if (SocoreMax <= MinScore)
						continue;
					IplImage	*SearchImage, *cropImage;

					/*------------------------------------------------------------------ */
					//Search model in pyramid1 image
					WidthPy  = width >> 1;
					HeightPy = height >> 1;

					ResultPiontX = ((MatchPiontX << 1) < 0) ? 0 : (MatchPiontX << 1);
					ResultPiontY = ((MatchPiontY << 1 )< 0) ? 0 : (MatchPiontY << 1);

					ReferPointX  = ModelID->m_ImageWidth >> 2;
					ReferPointY  = ModelID->m_ImageHeight >> 2;

					//ReferPointX  = (ModelID->m_pShapeInfoPyd1Vec[0].ImgWidth >> 1);
					//ReferPointY  = (ModelID->m_pShapeInfoPyd1Vec[0].ImgHeight >> 1);

					Row1 = ((ResultPiontX - ReferPointX - 2) < 0) ? 0 : (ResultPiontX - ReferPointX - 2);
					Col1  = ((ResultPiontY - ReferPointY - 2) < 0) ? 0 : (ResultPiontY - ReferPointY - 2);
					Row2 = ((ResultPiontX + ReferPointX + 2) > WidthPy) ? WidthPy : (ResultPiontX + ReferPointX + 2);
					Col2  = ((ResultPiontY + ReferPointY + 2) > HeightPy) ? HeightPy : (ResultPiontY + ReferPointY + 2);

					/* Set accurate match image */
					cropImgW = abs(Row1 - Row2);
					cropImgW = ((cropImgW & 1) == 0) ? cropImgW : (cropImgW + 1);
					cropImgH = abs(Col1 - Col2);
					cropImgH = ((cropImgH & 1) == 0) ? cropImgH : (cropImgH + 1);

					SearchImage = cvCreateImage(cvSize(WidthPy, HeightPy), IPL_DEPTH_8U, 1);
					memcpy((uint8_t*)SearchImage->imageData, pImagePy1, WidthPy*HeightPy);
					cropImage = cvCreateImage(cvSize(cropImgW, cropImgH), IPL_DEPTH_8U, 1);
					gen_rectangle(SearchImage, cropImage, Row1, Col1);

					SearchRegion->StartX = ((ResultPiontX - Row1 - 2) < 0) ? 0 : (ResultPiontX - Row1 - 2);
					SearchRegion->StartY = ((ResultPiontY - Col1 - 2) < 0) ? 0 : (ResultPiontY - Col1 - 2);
					SearchRegion->EndX   = SearchRegion->StartX + 4;
					SearchRegion->EndY   = SearchRegion->StartY + 4;

					SearchRegion->AngleRange = ModelID->m_pShapeInfoPyd1Vec[0].AngleNum;
					SearchRegion->AngleStart   = ((MatchAngle - OffSet) < ModelID->m_AngleStart) ?  ModelID->m_AngleStart : (MatchAngle - 2);
					SearchRegion->AngleStop   = ((MatchAngle + OffSet) > ModelID->m_AngleStop) ?  ModelID->m_AngleStop : (MatchAngle + 2);
					SearchRegion->AngleStep   = ModelID->m_AngleStep << 1;

					/* Find shape model in pyramid1 image */
					if(ModelID->m_pShapeInfoPyd1Vec == NULL)
						return;

					MatchResultA ResultPy1;
					shape_match_accurate((uint8_t*)cropImage->imageData, ModelID->m_pShapeInfoPyd1Vec, cropImgW, cropImgH,
						Contrast, MinContrast, MinScore, Greediness, SearchRegion, &ResultPy1);

					MatchPiontX = ResultPy1.CenterLocX + Row1;
					MatchPiontY = ResultPy1.CenterLocY + Col1;
					MatchAngle  = ResultPy1.Angel;
					SocoreMax	= ResultPy1.ResultScore;
					if (SocoreMax  < MinScore)
						continue;

					cvReleaseImage(&cropImage);
					cvReleaseImage(&SearchImage);

					/*------------------------------------------------------------------ */
					//Search model in source image
					ResultPiontX = ((MatchPiontX << 1) < 0) ? 0 : (MatchPiontX << 1);
					ResultPiontY = ((MatchPiontY << 1 )< 0) ? 0 : (MatchPiontY << 1);

					ReferPointX  = ModelID->m_ImageWidth >> 1;
					ReferPointY  = ModelID->m_ImageHeight >> 1;

					//ReferPointX  = (ModelID->m_pShapeInfoTmpVec[0].ImgWidth >> 1);
					//ReferPointY  = (ModelID->m_pShapeInfoTmpVec[0].ImgHeight >> 1);

					Row1 = ((ResultPiontX - ReferPointX - 2) < 0) ? 0 : (ResultPiontX - ReferPointX - 2);
					Col1  = ((ResultPiontY - ReferPointY - 2) < 0) ? 0 : (ResultPiontY - ReferPointY - 2);
					Row2 = ((ResultPiontX + ReferPointX + 2) > width) ? width : (ResultPiontX + ReferPointX + 2);
					Col2  = ((ResultPiontY + ReferPointY + 2) > height) ? height : (ResultPiontY + ReferPointY + 2);

					/* Set accurate match image */
					cropImgW = abs(Row1 - Row2);
					cropImgW = ((cropImgW & 1) == 0) ? cropImgW :  (cropImgW + 1);
					cropImgH = abs(Col1 - Col2);
					cropImgH = ((cropImgH & 1) == 0) ? cropImgH :  (cropImgH + 1);

					SearchImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
					memcpy((uint8_t*)SearchImage->imageData, pData, width*height);
					cropImage = cvCreateImage(cvSize(cropImgW, cropImgH), IPL_DEPTH_8U, 1);
					gen_rectangle(SearchImage, cropImage, Row1, Col1);
					//cvSaveImage("cropImage.bmp", cropImage);

					SearchRegion->StartX = ((ResultPiontX - Row1 - 2) < 0) ? 0 : (ResultPiontX - Row1 - 2);
					SearchRegion->StartY = ((ResultPiontY - Col1 - 2) < 0) ? 0 : (ResultPiontY - Col1 - 2);
					SearchRegion->EndX   = SearchRegion->StartX + 4;
					SearchRegion->EndY   = SearchRegion->StartY + 4;

					SearchRegion->AngleRange = ModelID->m_pShapeInfoTmpVec[0].AngleNum;
					SearchRegion->AngleStart   = ((MatchAngle - OffSet) < ModelID->m_AngleStart) ?  ModelID->m_AngleStart : (MatchAngle - 2);
					SearchRegion->AngleStop   = ((MatchAngle + OffSet) > ModelID->m_AngleStop) ?  ModelID->m_AngleStop : (MatchAngle + 2);
					SearchRegion->AngleStep   = ModelID->m_AngleStep;

					/* Find shape model in source image */
					if(ModelID->m_pShapeInfoTmpVec == NULL)
						return;

					MatchResultA Result;
					shape_match_accurate((uint8_t*)cropImage->imageData, ModelID->m_pShapeInfoTmpVec, cropImgW, cropImgH,
						Contrast, MinContrast, MinScore, Greediness, SearchRegion, &Result);

					if (Result.ResultScore > MinScore)
					{
						ResultList[n].Angel = Result.Angel;
						ResultList[n].ResultScore = Result.ResultScore;
						ResultList[n].CenterLocX = Result.CenterLocX + Row1 - xOffset;
						ResultList[n].CenterLocY = Result.CenterLocY + Col1 - yOffset;
						n++;
					}
					cvReleaseImage(&cropImage);
					cvReleaseImage(&SearchImage);

				}

				free(pImagePy2);
				free(pImagePy1);
				break;
			}
		case 1:
			{
				uint8_t	  *pImagePy1 = (uint8_t *) malloc((in_size/4) * sizeof(uint8_t));
				memcpy(pImagePy1, pOut, in_size/4);

				WidthPy  = width >> 1;
				HeightPy = height >> 1;

				SearchRegion->StartX = (ModelID->m_pShapeInfoPyd1Vec[0].ReferPoint.x >> 1) + (xOffset >> 1);
				SearchRegion->StartY = (ModelID->m_pShapeInfoPyd1Vec[0].ReferPoint.y >> 1) + (yOffset >> 1);
				SearchRegion->EndX   = WidthPy - SearchRegion->StartX;
				SearchRegion->EndY   = HeightPy - SearchRegion->StartY;

				SearchRegion->AngleRange = ModelID->m_pShapeInfoPyd1Vec[0].AngleNum;
				SearchRegion->AngleStart   = ModelID->m_AngleStart;
				SearchRegion->AngleStop   = ModelID->m_AngleStop;
				SearchRegion->AngleStep   = ModelID->m_AngleStep << 1;

				/* Find shape model in pyramid2 image */
				MatchResultA ResultListPy1[MAXTARGETNUM];
				memset(ResultListPy1, 0, MAXTARGETNUM * sizeof(MatchResultA));

				if(ModelID->m_pShapeInfoPyd1Vec == NULL)
					return;

				int TargetNum = 0;
				shape_match(pImagePy1, ModelID->m_pShapeInfoPyd1Vec, WidthPy, HeightPy, &TargetNum,
					Contrast, MinContrast, MinScore, Greediness, SearchRegion, ResultListPy1);

				/*------------------------------------------------------------------ */
				int n = 0;
				int OffSet = 0;
				for (int i = 0; i < MAXTARGETNUM; i++)
				{
					MatchPiontX = ResultListPy1[i].CenterLocX;
					MatchPiontY = ResultListPy1[i].CenterLocY;
					MatchAngle  = ResultListPy1[i].Angel;
					SocoreMax	= ResultListPy1[i].ResultScore;

					if (SocoreMax == 0)
						break;
					if (SocoreMax <= MinScore)
						continue;
					IplImage	*SearchImage, *cropImage;

					/*------------------------------------------------------------------ */
					//Search model in source image
					ResultPiontX = ((MatchPiontX << 1) < 0) ? 0 : (MatchPiontX << 1);
					ResultPiontY = ((MatchPiontY << 1 )< 0) ? 0 : (MatchPiontY << 1);

					ReferPointX  = ModelID->m_ImageWidth >> 1;
					ReferPointY  = ModelID->m_ImageHeight >> 1;

					//ReferPointX  = (ModelID->m_pShapeInfoTmpVec[0].ImgWidth >> 1);
					//ReferPointY  = (ModelID->m_pShapeInfoTmpVec[0].ImgHeight >> 1);

					Row1 = ((ResultPiontX - ReferPointX - 2) < 0) ? 0 : (ResultPiontX - ReferPointX - 2);
					Col1  = ((ResultPiontY - ReferPointY - 2) < 0) ? 0 : (ResultPiontY - ReferPointY - 2);
					Row2 = ((ResultPiontX + ReferPointX + 2) > width) ? width : (ResultPiontX + ReferPointX + 2);
					Col2  = ((ResultPiontY + ReferPointY + 2) > height) ? height : (ResultPiontY + ReferPointY + 2);

					/* Set accurate match image */
					cropImgW = abs(Row1 - Row2);
					cropImgW = ((cropImgW & 1) == 0) ? cropImgW :  (cropImgW + 1);
					cropImgH = abs(Col1 - Col2);
					cropImgH = ((cropImgH & 1) == 0) ? cropImgH :  (cropImgH + 1);

					SearchImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
					memcpy((uint8_t*)SearchImage->imageData, pData, width*height);
					cropImage = cvCreateImage(cvSize(cropImgW, cropImgH), IPL_DEPTH_8U, 1);
					gen_rectangle(SearchImage, cropImage, Row1, Col1);
					//cvSaveImage("cropImage.bmp", cropImage);

					SearchRegion->StartX = ((ResultPiontX - Row1 - 2) < 0) ? 0 : (ResultPiontX - Row1 - 2);
					SearchRegion->StartY = ((ResultPiontY - Col1 - 2) < 0) ? 0 : (ResultPiontY - Col1 - 2);
					SearchRegion->EndX   = SearchRegion->StartX + 4;
					SearchRegion->EndY   = SearchRegion->StartY + 4;

					SearchRegion->AngleRange = ModelID->m_pShapeInfoTmpVec[0].AngleNum;
					SearchRegion->AngleStart   = ((MatchAngle - OffSet) < ModelID->m_AngleStart) ?  ModelID->m_AngleStart : (MatchAngle - 2);
					SearchRegion->AngleStop   = ((MatchAngle + OffSet) > ModelID->m_AngleStop) ?  ModelID->m_AngleStop : (MatchAngle + 2);
					SearchRegion->AngleStep   = ModelID->m_AngleStep;

					/* Find shape model in source image */
					if(ModelID->m_pShapeInfoTmpVec == NULL)
						return;

					MatchResultA Result;
					shape_match_accurate((uint8_t*)cropImage->imageData, ModelID->m_pShapeInfoTmpVec, cropImgW, cropImgH,
						Contrast, MinContrast, MinScore, Greediness, SearchRegion, &Result);

					if (Result.ResultScore > MinScore)
					{
						ResultList[n].Angel = Result.Angel;
						ResultList[n].ResultScore = Result.ResultScore;
						ResultList[n].CenterLocX = Result.CenterLocX + Row1 - xOffset;
						ResultList[n].CenterLocY = Result.CenterLocY + Col1 - yOffset;
						n++;
					}
					cvReleaseImage(&cropImage);
					cvReleaseImage(&SearchImage);
				}

				free(pImagePy1);
				break;
			}
		case 0:
			{
				int offsetx = (ModelID->m_pShapeInfoTmpVec[0].ImgWidth - ModelID->m_ImageWidth) >> 1;
				int offsety = (ModelID->m_pShapeInfoTmpVec[0].ImgHeight - ModelID->m_ImageHeight) >> 1;

				SearchRegion->StartX = ((ModelID->m_pShapeInfoTmpVec[0].ImgWidth >> 1) - offsetx - 4);
				SearchRegion->StartY = ((ModelID->m_pShapeInfoTmpVec[0].ImgHeight >> 1) - offsety - 4);
				SearchRegion->EndX   = ImgWidth - SearchRegion->StartX;
				SearchRegion->EndY   = ImgHeight - SearchRegion->StartY;
				 
				SearchRegion->AngleRange = ModelID->m_pShapeInfoTmpVec[0].AngleNum;
				SearchRegion->AngleStart   = ModelID->m_AngleStart;
				SearchRegion->AngleStop   = ModelID->m_AngleStop;
				SearchRegion->AngleStep   = ModelID->m_AngleStep;

				MatchResultA ResultListSrc[MAXTARGETNUM];
				memset(ResultListSrc, 0, MAXTARGETNUM * sizeof(MatchResultA));

				int TargetNum = 0;
				shape_match((uint8_t*)(Image->imageData), ModelID->m_pShapeInfoTmpVec, ImgWidth, ImgHeight,  &TargetNum,
					Contrast, MinContrast, MinScore, Greediness, SearchRegion, ResultListSrc);

				QuickSort(ResultListSrc, 0, MAXTARGETNUM - 1);

				for (int i = 0; i < TargetNum; i++)
				{
					ResultList[i] = ResultListSrc[i];
				}
			}
		default:
			break;
		}

		free(pOut);
		free(pIn);
		free(SearchRegion);
		if(isBordred)
			cvReleaseImage(&ImgBordered);

		return;
	}
	else
		return;
}

int CShapeMatch::ShiftCos(int y)
{
	if (y<0) y*=-1;
	y %= 360;
	if ( y > 270 )
	{
		return ShiftCos((360 - y));
	}
	else if ( y > 180 )
	{
		return - ShiftCos((y - 180));
	}
	else if ( y > 90 )
	{
		return - ShiftCos((180 - y));
	}
	int index  = (y >> 2);
	int offset = (y % 4);
	// on the borderline of overflowing if use JInt16
	int cosVal = (4 - offset) * K_CosineTable[index]
	+ offset * K_CosineTable[index + 1];
	return cosVal >> 2;
}

int CShapeMatch::ShiftSin(int y)
{
	return ShiftCos(y + 270);
}

float CShapeMatch::Q_rsqrt(float number)
{
	//long i;
	//float x2, y;
	//const float threehalfs = 1.5F;
	//x2 = number * 0.5F;
	//y  = number;
	//i  = * ( long * ) &y;  // evil floating point bit level hacking
	//i  = 0x5f3759df - ( i >> 1 );
	//y  = * ( float * ) &i;
	//y  = y * ( threehalfs - ( x2 * y * y ) ); // 1st iteration
	//y  = y * ( threehalfs - ( x2 * y * y ) ); // 2nd iteration, this can be removed
	//return y;

	float xhalf = 0.5f*number;
	int i = *(int*)&number; // get bits for floating VALUE
	i = 0x5f375a86- (i>>1); // gives initial guess y0
	number = *(float*)&i; // convert bits BACK to float
	number = number*(1.5f-xhalf*number*number); // Newton step, repeating increases accuracy
	return number;
}

float CShapeMatch::new_rsqrt(float f)
{
	//���ǵ�����CPU SSEָ���rsqrt����ֱ�ӵó����

	//__m128 m_a = _mm_set_ps1(f);
	//__m128 m_b = _mm_rsqrt_ps(m_a);

	//return m_b[0];

	return 1/sqrtf(f);
}

void CShapeMatch::QuickSort(MatchResultA *s, int l, int r)
{
	int i, j;
	MatchResultA Temp;
	Temp.Angel 		 = 0;
	Temp.CenterLocX  = 0;
	Temp.CenterLocY  = 0;
	Temp.ResultScore = 0;
	if (l < r)
	{
		i = l;
		j = r;
		Temp = s[i];
		while (i < j)
		{
			while(i < j && s[j].ResultScore < Temp.ResultScore)	//�޸ķ��ſ������������߽���
				j--;
			if(i < j)
				s[i++] = s[j];

			while(i < j && s[i].ResultScore > Temp.ResultScore)	//�޸ķ��ſ������������߽���
				i++;
			if(i < j)

				s[j--] = s[i];

		}
		s[i] = Temp;
		QuickSort(s, l, i-1); /* �ݹ���� */
		QuickSort(s, i+1, r);
	}
}

int CShapeMatch::ConvertLength(int LengthSrc)
{
	int temp = 0;
	for (int i = 4; ; i++)
	{
		temp = (int)pow(2, i);
		if (temp >= LengthSrc)
		{
			LengthSrc = temp;
			break;
		}
	}
	return LengthSrc;
}

/* 
 Various border types, image boundaries are denoted with '|' 
  
 * BORDER_REPLICATE:     aaaaaa|abcdefgh|hhhhhhh 
 * BORDER_REFLECT:       fedcba|abcdefgh|hgfedcb 
 * BORDER_REFLECT_101:   gfedcb|abcdefgh|gfedcba 
 * BORDER_WRAP:          cdefgh|abcdefgh|abcdefg         
 * BORDER_CONSTANT:      iiiiii|abcdefgh|iiiiiii  with some specified 'i' 
 */  
int cv::borderInterpolate( int p, int len, int borderType ) // p����չ�߽��λ�ã�len��ԭͼ���  
{  
    if( (unsigned)p < (unsigned)len )     // ת��Ϊ�޷������ͣ���߽���ϱ߽磺pһ���Ǹ������ұ߽���±߽磬pһ���Ǵ���len�ġ�  
        ;  
    else if( borderType == BORDER_REPLICATE ) // �ظ����ͣ�ÿ�ζ�Ӧԭͼ��λ����0��len-1  
        p = p < 0 ? 0 : len - 1;  
    else if( borderType == BORDER_REFLECT || borderType == BORDER_REFLECT_101 ) // ����/ӳ��  
    {  
        int delta = borderType == BORDER_REFLECT_101;  
        if( len == 1 )  
            return 0;  
        do  
        {  
            if( p < 0 )    // ���䣺��߽��101���ұ߽�  
                p = -p - 1 + delta;  
            else  
                p = len - 1 - (p - len) - delta;  
        }  
        while( (unsigned)p >= (unsigned)len );  
    }  
    else if( borderType == BORDER_WRAP )  // ��װ  
    {  
        if( p < 0 )  // ��߽�  
            p -= ((p-len+1)/len)*len;  
        if( p >= len )  // �ұ߽�  
            p %= len;  
    }  
    else if( borderType == BORDER_CONSTANT )  // ���������⴦��  
        p = -1;  
    else  
        CV_Error( CV_StsBadArg, "Unknown/unsupported border type" );  
    return p;  
}

static void copyMakeBorder_8u( const uchar* src, size_t srcstep, Size srcroi, // ԭͼ ���������ݣ�step����С
							  uchar* dst, size_t dststep, Size dstroi,  // Ŀ��ͼ�����
							  int top, int left, int cn, int borderType )
{
	const int isz = (int)sizeof(int);
	int i, j, k, elemSize = 1;
	bool intMode = false;

	if( (cn | srcstep | dststep | (size_t)src | (size_t)dst) % isz == 0 )
	{
		cn /= isz;
		elemSize = isz;
		intMode = true;
	}

	AutoBuffer<int> _tab((dstroi.width - srcroi.width)*cn);  // ��С����չ�����ұ߽�֮�ͣ������ڴ����չ�ı߽���ԭͼ�е�λ��
	int* tab = _tab;
	int right = dstroi.width - srcroi.width - left;
	int bottom = dstroi.height - srcroi.height - top;

	for( i = 0; i < left; i++ ) // ��߽�
	{
		j = borderInterpolate(i - left, srcroi.width, borderType)*cn;  // �����ԭͼ�ж�Ӧ��λ��
		for( k = 0; k < cn; k++ )  // ÿ��ͨ���Ĵ���
			tab[i*cn + k] = j + k;
	}

	for( i = 0; i < right; i++ )  // �ұ߽�
	{
		j = borderInterpolate(srcroi.width + i, srcroi.width, borderType)*cn;
		for( k = 0; k < cn; k++ )
			tab[(i+left)*cn + k] = j + k;
	}

	srcroi.width *= cn;
	dstroi.width *= cn;
	left *= cn;
	right *= cn;

	uchar* dstInner = dst + dststep*top + left*elemSize;

	for( i = 0; i < srcroi.height; i++, dstInner += dststep, src += srcstep ) // ��ԭͼ�и������ݵ���չ�ı߽���
	{
		if( dstInner != src )
			memcpy(dstInner, src, srcroi.width*elemSize);

		if( intMode )
		{
			const int* isrc = (int*)src;
			int* idstInner = (int*)dstInner;
			for( j = 0; j < left; j++ )
				idstInner[j - left] = isrc[tab[j]];
			for( j = 0; j < right; j++ )
				idstInner[j + srcroi.width] = isrc[tab[j + left]];
		}
		else
		{
			for( j = 0; j < left; j++ )
				dstInner[j - left] = src[tab[j]];
			for( j = 0; j < right; j++ )
				dstInner[j + srcroi.width] = src[tab[j + left]];
		}
	}

	dstroi.width *= elemSize;
	dst += dststep*top;

	for( i = 0; i < top; i++ )  // �ϱ߽�
	{
		j = borderInterpolate(i - top, srcroi.height, borderType);
		memcpy(dst + (i - top)*dststep, dst + j*dststep, dstroi.width); // �������еĸ���
	}

	for( i = 0; i < bottom; i++ ) // �ȱ߽�
	{
		j = borderInterpolate(i + srcroi.height, srcroi.height, borderType);
		memcpy(dst + (i + srcroi.height)*dststep, dst + j*dststep, dstroi.width); // �������еĸ���
	}
}