#include "ShapeMatch.h"

typedef struct iPoint{
	int X;
	int Y;
}iPoint;

typedef struct ROI{
	int x;			// ���Ͻǵ�����
	int y;
	int width;	// ROI�Ŀ�͸�
	int height;
}ROI;

typedef struct iImage{
	uint8_t  *ImageData;		// ͼ������
	int width;						// ͼ��������� 
	int height;						// ͼ���������
}iImage;

typedef struct trainparam{
	int 		Contrast;					//����ֵ
	int 		MinContrast;			//����ֵ
	int		Granularity;				//��Ե������
}trainparam;

typedef struct modelparam{
	int 	  NumLevels;					//����������
	int 	  AngleStart;					//ģ����ת��ʼ�Ƕ�
	int 	  AngleStop;					//ģ����ת��ֹ�Ƕ�
	int 	  AngleStep;					//�ǶȲ���
	int      PointReduction;			//ƥ���������
}modelparam;

typedef struct matchparam{
	int 		ID;							//ѡ��ģ���id
	int		NumMatches;			//ƥ�����
	float 	MinScore;					//��С�ķ�
	float 	Greediness;				//̰����
}matchparam;

typedef struct matchresult{
	int 		Angel;						//ƥ��Ƕ�
	float 	ResultScore;				//ƥ��ķ�
	iPoint   ReferencePiont;			//�ο�������
}matchresult;