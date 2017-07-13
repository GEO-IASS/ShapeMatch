#pragma once

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

using namespace std;
using namespace cv;

#define		MAX_NUM_INSTANCES		300				//���Ŀ�����
#define		MIN_NUM_LEVELS				0					//��С����������
#define		MAX_NUM_LEVELS				5					//������������

const int K_CosineTable[24] =
{
	8192, 8172, 8112, 8012, 7874, 7697,
	7483, 7233, 6947, 6627, 6275, 5892, 
	5481, 5043, 4580, 4096, 3591, 3068,
	2531, 1981, 1422, 856,   285,   -285
};

//ƥ�����ṹ��
struct MatchResultA
{
	int 			Angel;						//ƥ��Ƕ�
	int 			CenterLocX;				//ƥ��ο���X����
	int			CenterLocY;				//ƥ��ο���Y����
	float 		ResultScore;				//ƥ��ķ�
};

//������Ϣ�ṹ��
struct ShapeInfo
{
	CvPoint			ReferPoint;					//ģ����������RefPoint
	CvPoint			*Coordinates;				//ģ����������
	float					*EdgeMagnitude;			//�ݶȵ���
	short				*EdgeDerivativeX;			//X�����ݶ�
	short				*EdgeDerivativeY;			//Y�����ݶ�
	int 					ImgWidth;					//ͼ����
	int					ImgHeight;					//ͼ��߶�
	int					NoOfCordinates;			//���������
	int					Angel;							//��ת�Ƕ�
	int					PyLevel;						//����������
	int					AngleNum;					//�Ƕȸ���
};

//ģ���ļ��ṹ��
struct shape_model
{
	int	ID;												//ģ��ID
	int 	m_NumLevels;								//����������
	int 	m_Contrast;									//����ֵ
	int 	m_MinContrast;							//����ֵ
	int 	m_Granularity;								//��Ե������
	int 	m_AngleStart;								//ģ����ת��ʼ�Ƕ�
	int 	m_AngleStop;								//ģ����ת��ֹ����
	int 	m_AngleStep;								//�ǶȲ���
	int    m_ImageWidth;							//ԭģ��ͼ����
	int    m_ImageHeight;							//ԭģ��ͼ��߶�
	bool	m_IsInited;									//��ʼ����־

	ShapeInfo *m_pShapeInfoPyd1Vec;		//ģ���������1��ͼ��ı�Ե��Ϣ
	ShapeInfo *m_pShapeInfoPyd2Vec;		//ģ���������2��ͼ��ı�Ե��Ϣ
	ShapeInfo *m_pShapeInfoPyd3Vec;		//ģ���������3��ͼ��ı�Ե��Ϣ
	ShapeInfo *m_pShapeInfoTmpVec;		//ԭģ��ͼ��ı�Ե��Ϣ
};

//��������
struct search_region
{
	int 	StartX;											//X�������
	int 	StartY;											//y�������
	int 	EndX;											//x�����յ�
	int 	EndY;											//y�����յ�
	int 	AngleRange;									//�����Ƕ���Ŀ
	int    AngleStart;									//����Ԥ�ȽǶ�
	int	AngleStop;									//������ֹ�Ƕ�
	int    AngleStep;									//�����ǶȲ���

};

//�߽���б�
struct edge_list
{
	CvPoint *EdgePiont;						//��Ե��������
	int 		 ListSize;							//�����С

};

class CShapeMatch
{
public:
	CShapeMatch(void);
	~CShapeMatch(void);

	void gaussian_filter(uint8_t* corrupted, uint8_t* smooth, int width, int height);
		/*--------------------------------------------------------------------------------------------*/
	/*	��������gen_rectangle
		�������ܣ�����ROI
		���������Image ����ͼ��, Row1 ���Ͻǵ�ĺ�����, Column1 ���Ͻǵ��������
		���ر�����
		ע�ͣ�		*/

	void gen_rectangle(IplImage *Image, IplImage *ModelRegion, int Row1, int Column1);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������gen_rectangle
		�������ܣ�����ROI
		���������Image ����ͼ��, Row1 ���Ͻǵ�ĺ�����, Column1 ���Ͻǵ��������
		���ر�����
		ע�ͣ�		*/

	void board_image(IplImage *SrcImg, IplImage *ImgBordered, int32_t xOffset, int32_t yOffset);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������board_image
		�������ܣ�ͼ��߽���չ
		���������SrcImage ����ͼ��, DstImage ���ͼ��
		���ر�����
		ע�ͣ�		*/

	void rotate_image (uint8_t *SrcImgData, uint8_t *MaskImgData, int srcWidth, int srcHeight, uint8_t *DstImgData, uint8_t *MaskRotData, int dstWidth, int dstHeight, int Angle);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������rotate_image
		�������ܣ�ͼ����ת����
		���������SrcImage ����ͼ��, DstImage ���ͼ��, Angle��ת�Ƕ�
		���ر�����DstImgData ��ת���ͼ��
		ע�ͣ�		*/

	void rotateImage (IplImage* srcImage, IplImage* dstImage, int Angle);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������rotate_image
		�������ܣ�ͼ����ת����
		���������SrcImage ����ͼ��, DstImage ���ͼ��, Angle��ת�Ƕ�
		���ر�����DstImgData ��ת���ͼ��
		ע�ͣ�		*/

	void image_pyramid(uint8_t *SrcImgData,  int srcWidth, int srcHeight, uint8_t *OutImgData);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������imagePyramid
		�������ܣ�ͼ����������� 2*2����
		���������SrcImage ����ͼ��, srcWidth ͼ���ȣ� srcHeight ͼ��߶�
		���ر�����OutImageData ������ͼ��
		ע�ͣ�		*/

	void imagePyramid(uint8_t *SrcImgData,  int srcWidth, int srcHeight, uint8_t *OutImgData);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������imagePyramid
		�������ܣ�ͼ����������� ˫���Բ�ֵ
		���������SrcImage ����ͼ��, srcWidth ͼ���ȣ� srcHeight ͼ��߶�
		���ر�����OutImageData ������ͼ��
		ע�ͣ�		*/

	void initial_shape_model(shape_model *ModelID, int Width, int Height, int EdgeSize);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������initial_shape_model
		�������ܣ���ʼ��ģ����Դ
		���������ModelID ģ���ļ�
		���ر�����
		ע�ͣ�		*/

	bool release_shape_model(shape_model *ModelID);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������release_shape_model
		�������ܣ��ͷ�ģ����Դ
		���������ModelID ģ���ļ�
		���ر�����
		ע�ͣ�		*/

	void extract_shape_info(uint8_t *ImageData, ShapeInfo *ShapeInfoData, int Contrast, int MinContrast, int PointReduction, uint8_t *MaskImgData);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������extract_shape_info
		�������ܣ���ȡ��״��Ϣ
		���������ImageData ͼ������, MinContrast ��С��ֵ, Contrast ��ֵ
		���ر�����shape_info ��״��Ϣ
		ע�ͣ�		*/

	bool build_model_list(ShapeInfo *ShapeInfoVec, uint8_t *ImageData, uint8_t *MaskData, int Width, int Height, int Contrast, int MinContrast, int Granularity);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������build_model_list
		�������ܣ������Ƕ�ģ������
		���������ShapeInfoVec ����ָ��, MinContrast ��С��ֵ, Contrast ��ֵ, Graininess ��Ե������
		���ر�����ModelID ģ���ļ�
		ע�ͣ�		*/

	void train_shape_model(IplImage *Image, int Contrast, int MinContrast, int PointReduction, edge_list *EdgeList);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������train_shape_model
		�������ܣ�ѵ����״ģ��
		���������Image ����ͼ��, MinContrast ��С��ֵ, Contrast ��ֵ
		���ر�����EdgeList ��Ե������
		ע�ͣ�		*/

	bool create_shape_model(IplImage *Template, shape_model *ModelID);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������create_shape_model
		�������ܣ�����ƥ��ģ��
		���������Template ģ��ͼ��, NumLevels ����������, AngleStart ��ʼ�Ƕ�, AngleExtent �Ƕȷ�Χ, AngleStep �ǶȲ���, PointReduction ��Ե����������, Contrast
		���ر�����ModelID ģ���ļ�
		ע�ͣ�		*/

	void shape_match(uint8_t *SearchImage, ShapeInfo *ShapeInfoVec, int Width, int Height, int *NumMatches, int Contrast, int MinContrast, float MinScore, float Greediness, search_region *SearchRegion, MatchResultA *ResultList);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������shape_match
		�������ܣ���״ƥ�亯��
		���������SearchImage ������ͼ������,  ShapeInfo ģ����״��Ϣ, Width ͼ����, Height ͼ��߶�, NumMatches ƥ��Ŀ���� MinScore ��С����, Greediness ̰����, SearchRegion ������Χ
		���ر�����ResultList ƥ����
		ע�ͣ�		*/
	
	void shape_match_accurate(uint8_t *SearchImage, ShapeInfo *ShapeInfoVec, int Width, int Height, int Contrast, int MinContrast, float MinScore, float Greediness, search_region *SearchRegion, MatchResultA *ResultList);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������shape_match_accurate
		�������ܣ���ȷ��״ƥ�亯��
		���������SearchImage ������ͼ������,  ShapeInfo ģ����״��Ϣ, Width ͼ����, Height ͼ��߶�, NumMatches ƥ��Ŀ���� MinScore ��С����, Greediness ̰����, SearchRegion ������Χ
		���ر�����ResultList ƥ����
		ע�ͣ�		*/

	void find_shape_model(IplImage *Image, shape_model *ModelID, float MinScore, int NumMatches, float Greediness, MatchResultA *ResultList);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������find_shape_model
		�������ܣ�������ͼ��Ѱ��ģ��Ŀ��
		���������Image ������ͼ��,  ModelID ģ���ļ�, AngleStart ��ʼ�Ƕ�, AngleExtent �Ƕȷ�Χ, MinScore ��С����, NumMatches ƥ��Ŀ����
		���ر�����Row ƥ��ο����X����, Column ƥ��ο����Y����, Angle Ŀ����ת�Ƕ�, Score ����
		ע�ͣ�		*/

	int  ShiftCos(int y);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������ShiftCos
		�������ܣ��������Ǻ���
		���������y �Ƕ�
		���ر���������ֵ

		ע�ͣ�		*/

	int  ShiftSin(int y);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������ShiftSin
		�������ܣ��������Ǻ���
		���������y �Ƕ�
		���ر���������ֵ
		ע�ͣ�		*/

	float Q_rsqrt( float number );
	/*--------------------------------------------------------------------------------------------*/
	/*	��������Q_rsqrt
		�������ܣ����ƽ��������
		���������number
		���ر�����ƽ��������
		ע�ͣ�		*/

	float new_rsqrt(float f);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������new_rsqrt
		�������ܣ����ƽ��������
		���������f
		���ر�����ƽ��������
		ע�ͣ�		*/

	void QuickSort(MatchResultA *s, int l, int r);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������QuickSort
		�������ܣ���������
		���������s[] ������飬
		���ر�����
		ע�ͣ�		*/

	int ConvertLength(int LengthSrc);
	/*--------------------------------------------------------------------------------------------*/
	/*	��������ConvertLength
		�������ܣ�����ת��
		���������LengthSrc ��ת�����ȣ�
		���ر�����ת�����
		ע�ͣ�		*/
};

