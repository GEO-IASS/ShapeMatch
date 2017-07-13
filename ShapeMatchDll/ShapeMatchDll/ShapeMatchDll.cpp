// ShapeMatchDll.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "ShapeMatchDll.h"

CShapeMatch Match;
shape_model *Model = NULL;

int EdgeSize = 0;
int MinConstrast = 0;
int Constrast		  =  0;
int Granularity	  =  0;
int ModelWidth  = 0;
int ModelHeight = 0;
bool isCreated = false;

SHAPEMATCHDLL_API void CMatch::TrainShapeModel(iImage Image,  trainparam TrainParam, iPoint *EdgeList, int &ListSize)
{
	MinConstrast = TrainParam.MinContrast;
	Constrast		 =  TrainParam.Contrast;
	Granularity	 =  TrainParam.Granularity;
	ModelWidth  =  Image.width;
	ModelHeight =  Image.height;

	edge_list m_EdgeList;
	m_EdgeList.EdgePiont = (CvPoint *) malloc(Image.width * Image.height * sizeof(CvPoint));
	IplImage *TrainImage = cvCreateImage(cvSize(Image.width, Image.height), IPL_DEPTH_8U, 1);
	memcpy((uint8_t*)TrainImage->imageData, Image.ImageData, Image.width * Image.height);
	//cvSaveImage("Train.bmp", TrainImage);
	Match.train_shape_model(TrainImage, Constrast, MinConstrast, Granularity, &m_EdgeList);
	EdgeSize = m_EdgeList.ListSize;
	ListSize = EdgeSize;
	memcpy(EdgeList, m_EdgeList.EdgePiont, sizeof(iPoint) * EdgeSize);
	free(m_EdgeList.EdgePiont);
	cvReleaseImage(&TrainImage);
}

SHAPEMATCHDLL_API bool CMatch::CreateShapeModel(iImage Image, modelparam ModelParam, int ModelID)
{
	if (EdgeSize ==0)
		return false;
	Model = (shape_model*) malloc(sizeof(shape_model));
	Model->m_AngleStart = ModelParam.AngleStart;		//��ʼ�Ƕ�
	Model->m_AngleStop = ModelParam.AngleStop	;		//��ֹ�Ƕ�
	Model->m_AngleStep  = ModelParam.AngleStep;		//�ǶȲ���
	Model->m_NumLevels = ModelParam.NumLevels;		//����������
	Model->m_Contrast     = Constrast;								//����ֵ
	Model->m_MinContrast = MinConstrast;						//����ֵ
	Model->m_Granularity = Granularity;						    //������
	Model->m_ImageWidth = ModelWidth;						//ģ����
	Model->m_ImageHeight = ModelHeight;						//ģ��߶�

	IplImage *ModelImage = cvCreateImage(cvSize(Image.width, Image.height), IPL_DEPTH_8U, 1);
	memcpy((uint8_t*)ModelImage->imageData, Image.ImageData, Image.width * Image.height);
	Match.initial_shape_model(Model, Image.width, Image.height, EdgeSize);
	isCreated = Match.create_shape_model(ModelImage, Model);
	return isCreated;
}

SHAPEMATCHDLL_API void CMatch::FindShapeModel(iImage Image,  int ModelID, matchparam MatchParam, matchresult *ResultList, int &MatchNum)
{
	IplImage *SearchImage = cvCreateImage(cvSize(Image.width, Image.height), IPL_DEPTH_8U, 1);
	memcpy((uint8_t*)SearchImage->imageData, Image.ImageData, Image.width * Image.height);
	MatchResultA Result[10];
	memset(&Result, 0, 10 * sizeof(MatchResultA));
	matchresult MatchResult[10];
	memset(&MatchResult, 0, 10 * sizeof(matchresult));
	if (isCreated)
	{
		Match.find_shape_model(SearchImage, Model, MatchParam.MinScore, MatchParam.NumMatches, MatchParam.Greediness, Result);
		for (int n = 0; n <MatchParam.NumMatches; n++)
		{
			if (Result[n].ResultScore != 0)
			{
				MatchResult[n].Angel = Result[n].Angel;
				MatchResult[n].ResultScore = Result[n].ResultScore;
				MatchResult[n].ReferencePiont.X = Result[n].CenterLocX;
				MatchResult[n].ReferencePiont.Y = Result[n].CenterLocY;
				MatchNum++;
				//printf(" Location:(%d, %d) Angle: %d Score: %.4f\n", Result[n].CenterLocX, Result[n].CenterLocY, Result[n].Angel, Result[n].ResultScore);
			}
			else
				break;
		}
		memcpy(ResultList, MatchResult, MatchNum * sizeof(matchresult));
	}
	else
	{
		return;
	}
}

SHAPEMATCHDLL_API void CMatch::ReleaseShapeModel(int ModelID)
{
	if (Model != NULL)
	{
		Match.release_shape_model(Model);
		free(Model);
	}
	return;
}

SHAPEMATCHDLL_API void CMatch::GenRectangle(iImage SrcImage, iImage DstImage, iPoint LeftUpPoint)
{
	IplImage *SourceImage = cvCreateImage(cvSize(SrcImage.width, SrcImage.height), IPL_DEPTH_8U, 1);
	memcpy((uint8_t*)SourceImage->imageData, SrcImage.ImageData, SrcImage.width * SrcImage.height);

	IplImage *DestnationImage = cvCreateImage(cvSize(DstImage.width, DstImage.height), IPL_DEPTH_8U, 1);

	Match.gen_rectangle(SourceImage, DestnationImage, LeftUpPoint.X, LeftUpPoint.Y);
	memcpy(DstImage.ImageData, (uint8_t*)DestnationImage->imageData, DstImage.width * DstImage.height);

	cvReleaseImage(&DestnationImage);
	cvReleaseImage(&SourceImage);
}
