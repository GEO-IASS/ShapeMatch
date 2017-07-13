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
		/*	��������TrainShapeModel
			�������ܣ�ѵ����״ģ��
			���������Image ����ͼ��, MinContrast ��С��ֵ, Contrast ��ֵ
			���ر�����EdgeList[] ��Ե������
			ע�ͣ�		*/

		static SHAPEMATCHDLL_API bool CreateShapeModel(iImage Image, modelparam ModelParam, int ModelID);
		/*--------------------------------------------------------------------------------------------*/
		/*	��������CreateShapeModel
			�������ܣ�������״ģ���ļ�
			���������Image ����ͼ��, ModelID ģ��ID
			���ر�����
			ע�ͣ�		*/

		static SHAPEMATCHDLL_API void FindShapeModel(iImage Image, int ModelID, matchparam MatchParam, matchresult *ResultList, int &MatchNum);
		/*--------------------------------------------------------------------------------------------*/
		/*	��������FindShapeModel
			�������ܣ�����ƥ��
			���������Image ����ͼ��, ModelID ģ��ID, ResultList ����б�
			���ر�����
			ע�ͣ�		*/

		static SHAPEMATCHDLL_API void ReleaseShapeModel(int ModelID);
		/*--------------------------------------------------------------------------------------------*/
		/*	��������ReleaseShapeModel
			�������ܣ��ͷ�ģ���ڴ�ռ�
			���������ModelID ģ��ID
			���ر�����
			ע�ͣ�		*/

		static SHAPEMATCHDLL_API void GenRectangle(iImage SrcImage, iImage DstImage, iPoint LeftUpPoint);
				/*--------------------------------------------------------------------------------------------*/
		/*	��������GenRectangle
			�������ܣ���ȡͼ��ROI����
			���������SrcImage ����ͼ��, DstImage ���ͼ��, LeftUpPoint ���Ͻ�
			���ر�����
			ע�ͣ�		*/
	};

	/* extern "C" { */
#ifdef __cplusplus
}
#endif