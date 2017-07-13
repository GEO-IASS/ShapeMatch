using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace TestForm
{
    public struct iPoint
    {
        public int X;
        public int Y;
    }

    public struct iImage
    {
	public IntPtr  ImageData;		    // 图像数据
	public int width;						// 图像宽像素数 
	public int height;						// 图像高像素数
    }

    public struct ROI
    {
        public int x;			     // 左上角点x坐标
	    public int y;               // 左上角点y坐标
	    public int width;	     // ROI区域宽度
	    public int height;       // ROI区域高度
    }

    public struct trainparam
    {
	    public int 		Contrast;					//高阈值
	    public int 		MinContrast;			//低阈值
	    public int		Granularity;				//边缘颗粒度
    }
    public struct modelparam
    {
	    public int 	    NumLevels;					//金字塔级数
	    public int 	    AngleStart;					//模板旋转起始角度
	    public int 	    AngleStop;				    //模板旋转角度幅度
	    public int 	    AngleStep;					//角度步长
	    public int      PointReduction;			//匹配加速因子
    }

    public struct matchparam
    {
        public int 		ID;							//选择模板的id
	    public int		NumMatches;			//匹配个数
        public float 	MinScore;					//最小的分
	    public float 	Greediness;				//贪婪度
    }

    public struct matchresult
    {
	    public int 		Angel;						//匹配角度
	    public float 	ResultScore;				//匹配的分
	    public iPoint ReferencePiont;		//参考点坐标
    }

    public class ShapeMatchAPI
    {
        [DllImport("ShapeMatchDll.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        public static extern void TrainShapeModel(iImage Image, trainparam TrainParam, IntPtr EdgeList, ref int ListSize);

        [DllImport("ShapeMatchDll.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CreateShapeModel(iImage Image, modelparam ModelParam, int ModelID);

        [DllImport("ShapeMatchDll.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        public static extern void FindShapeModel(iImage Image, int ModelID, matchparam MatchParam, IntPtr ResultList, ref int MatchNum);

        [DllImport("ShapeMatchDll.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        public static extern void ReleaseShapeModel(int ModelID);

        [DllImport("ShapeMatchDll.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        public static extern void GenRectangle(iImage SrcImage, iImage DstImage, iPoint LeftUpPoint);
    }
}
