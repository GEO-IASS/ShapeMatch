#ifdef SHAPEMATCHDLL_EXPORTS
#define SHAPEMATCHDLL_API __declspec(dllexport) 
#else
#define SHAPEMATCHDLL_API __declspec(dllimport) 
#endif

#include "ShapeMatchDef.h"
/* By C++ language compiler */
#ifdef __cplusplus
extern "C" {
#endif

	class CMatch
	{
	public:
		static SHAPEMATCHDLL_API void TrainShapeModel(iImage Image, trainparam TrainParam, iPoint *EdgeList, int &ListSize);
		/*--------------------------------------------------------------------------------------------*/
		/*	函数名：TrainShapeModel
			函数功能：训练形状模板
			输入变量：Image 输入图像, MinContrast 最小阈值, Contrast 阈值
			返回变量：EdgeList[] 边缘点序列
			注释：		*/

		static SHAPEMATCHDLL_API bool CreateShapeModel(iImage Image, modelparam ModelParam, int ModelID);
		/*--------------------------------------------------------------------------------------------*/
		/*	函数名：CreateShapeModel
			函数功能：创建形状模板文件
			输入变量：Image 输入图像, ModelID 模板ID
			返回变量：
			注释：		*/

		static SHAPEMATCHDLL_API void FindShapeModel(iImage Image, int ModelID, matchparam MatchParam, matchresult *ResultList, int &MatchNum);
		/*--------------------------------------------------------------------------------------------*/
		/*	函数名：FindShapeModel
			函数功能：查找匹配
			输入变量：Image 输入图像, ModelID 模板ID, ResultList 结果列表
			返回变量：
			注释：		*/

		static SHAPEMATCHDLL_API void ReleaseShapeModel(int ModelID);
		/*--------------------------------------------------------------------------------------------*/
		/*	函数名：ReleaseShapeModel
			函数功能：释放模板内存空间
			输入变量：ModelID 模板ID
			返回变量：
			注释：		*/

		static SHAPEMATCHDLL_API void GenRectangle(iImage SrcImage, iImage DstImage, iPoint LeftUpPoint);
				/*--------------------------------------------------------------------------------------------*/
		/*	函数名：GenRectangle
			函数功能：获取图像ROI区域
			输入变量：SrcImage 输入图像, DstImage 输出图像, LeftUpPoint 左上角
			返回变量：
			注释：		*/
	};

	/* extern "C" { */
#ifdef __cplusplus
}
#endif