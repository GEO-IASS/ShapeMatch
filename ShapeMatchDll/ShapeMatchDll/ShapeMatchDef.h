#include "ShapeMatch.h"

typedef struct iPoint{
	int X;
	int Y;
}iPoint;

typedef struct ROI{
	int x;			// 左上角点坐标
	int y;
	int width;	// ROI的宽和高
	int height;
}ROI;

typedef struct iImage{
	uint8_t  *ImageData;		// 图像数据
	int width;						// 图像宽像素数 
	int height;						// 图像高像素数
}iImage;

typedef struct trainparam{
	int 		Contrast;					//高阈值
	int 		MinContrast;			//低阈值
	int		Granularity;				//边缘颗粒度
}trainparam;

typedef struct modelparam{
	int 	  NumLevels;					//金字塔级数
	int 	  AngleStart;					//模板旋转起始角度
	int 	  AngleStop;					//模板旋转终止角度
	int 	  AngleStep;					//角度步长
	int      PointReduction;			//匹配加速因子
}modelparam;

typedef struct matchparam{
	int 		ID;							//选择模板的id
	int		NumMatches;			//匹配个数
	float 	MinScore;					//最小的分
	float 	Greediness;				//贪婪度
}matchparam;

typedef struct matchresult{
	int 		Angel;						//匹配角度
	float 	ResultScore;				//匹配的分
	iPoint   ReferencePiont;			//参考点坐标
}matchresult;