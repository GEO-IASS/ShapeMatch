using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace TestForm
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            //ImgBuf.width = IMG_WIDTH;
            //ImgBuf.height = IMG_HEIGHT;
            //ImgData = new byte[ImgBuf.width * ImgBuf.height + 8];
            //pImgData = Marshal.AllocHGlobal(ImgBuf.width * ImgBuf.height);
        }

        #region Mouse event
        int x1;     //鼠标按下时横坐标
        int y1;     //鼠标按下时纵坐标
        bool HeadImageBool = true;    // 此布尔变量用来判断pictureBox1控件是否有图片

        Point p1;   //定义鼠标按下时的坐标点
        Point p2;   //定义移动鼠标时的坐标点
        Point p3;   //定义松开鼠标时的坐标点

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left && isSetROI)
            {
                this.Cursor = Cursors.Cross;
                this.p1 = new Point(e.X, e.Y);
                x1 = e.X;
                y1 = e.Y;
                if (this.pictureBox1.Image != null)
                {
                    HeadImageBool = true;
                    if (isSetROI)
                    {
                        Roi.TopLeftX = e.X;
                        Roi.TopLeftY = e.Y;
                    }
                }
                else
                {
                    HeadImageBool = false;
                }
            }
            else if (e.Button == MouseButtons.Right)
            {
                if (isSetROI)
                {
                    int ImgWidth = Roi.BottomRightX - x1;
                    int ImgHeight = Roi.BottomRightY - y1;
                    if (ImgWidth <= 0 || ImgHeight <= 0)
                        return;
                    if ((ImgWidth & 1) != 0)
                    {
                        ImgWidth += 1;
                    }
                    if ((ImgHeight & 1) != 0)
                    {
                        ImgHeight += 1;
                    }
                    Roi.BottomRightY = e.Y;
                    TempleBitmap = new Bitmap(ImgWidth, ImgHeight);
                    isSetROI = false;
                    Rectangle cloneRect = new Rectangle(x1, y1, ImgWidth, ImgHeight);
                    PixelFormat format =SourceBitmap.PixelFormat;
                    TempleBitmap = SourceBitmap.Clone(cloneRect, format);
                    this.pictureBox2.Image = TempleBitmap;

                    int dataBytes = ImgWidth * ImgHeight;
                    TmpImgData = new byte[dataBytes];
                    pTmpImgData = Marshal.AllocHGlobal(dataBytes);

                    iPoint LeftTop;
                    LeftTop.X = x1;
                    LeftTop.Y = y1;
                    TempleImage.width = ImgWidth;
                    TempleImage.height = ImgHeight;
                    TempleImage.ImageData = pTmpImgData;
                    ShapeMatchAPI.GenRectangle(SourceImage, TempleImage, LeftTop);
                }
            }

        }

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (this.Cursor == Cursors.Cross)
            {
                this.p2 = new Point(e.X, e.Y);
                if ((p2.X - p1.X) > 0 && (p2.Y - p1.Y) > 0)     //当鼠标从左上角向开始移动时P3坐标
                {
                    this.p3 = new Point(p1.X, p1.Y);
                }
                if ((p2.X - p1.X) < 0 && (p2.Y - p1.Y) > 0)     //当鼠标从右上角向左下方向开始移动时P3坐标
                {
                    this.p3 = new Point(p2.X, p1.Y);
                }
                if ((p2.X - p1.X) > 0 && (p2.Y - p1.Y) < 0)     //当鼠标从左下角向上开始移动时P3坐标
                {
                    this.p3 = new Point(p1.X, p2.Y);
                }
                if ((p2.X - p1.X) < 0 && (p2.Y - p1.Y) < 0)     //当鼠标从右下角向左方向上开始移动时P3坐标
                {
                    this.p3 = new Point(p2.X, p2.Y);
                }
                this.pictureBox1.Invalidate();  //使控件的整个图面无效，并导致重绘控件
            }
        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            if (HeadImageBool)
            {
                if (isSetROI)
                {
                    Roi.BottomRightX = e.X;
                    Roi.BottomRightY = e.Y;
                }
                this.Cursor = Cursors.Default;
            }
            else
            {
                this.Cursor = Cursors.Default;
            }
        }
        #endregion

        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {
            int w = pictureBox1.ClientRectangle.Width; 
            int h = pictureBox1.ClientRectangle.Height;
            int CenterX = (p2.X + p1.X) / 2;
            int CenterY = (p2.Y + p1.Y) / 2;
            if (HeadImageBool && isSetROI)
            {
                Rectangle rect = new Rectangle(p3, new Size(System.Math.Abs(p2.X - p1.X), System.Math.Abs(p2.Y - p1.Y)));
                e.Graphics.DrawRectangle(new Pen(Color.Red, 1), rect);
                e.Graphics.DrawLine(new Pen(Color.Blue, 1), CenterX - 5, CenterY, CenterX + 5, CenterY);
                e.Graphics.DrawLine(new Pen(Color.Blue, 1), CenterX, CenterY - 5, CenterX, CenterY + 5);
            }
            if (isMatch)
            {
                for (int i = 0; i < MatchNum; i++)
                {
                    CenterX = ResultList[i].ReferencePiont.X;
                    CenterY = ResultList[i].ReferencePiont.Y;
                    e.Graphics.DrawLine(new Pen(Color.Red, 1), CenterX - 5, CenterY, CenterX + 5, CenterY);
                    e.Graphics.DrawLine(new Pen(Color.Red, 1), CenterX, CenterY - 5, CenterX, CenterY + 5);
                }
                isMatch = false;
            }
        }

        private void pictureBox2_Paint(object sender, PaintEventArgs e)
        {
            if (isTrain)
            {
                Pen pen = new Pen(Color.Green, 1);
                Bitmap bmp = new Bitmap(pictureBox2.Image.Width, pictureBox2.Image.Height);
                Graphics graphics = Graphics.FromImage(bmp);
                for (int i = 0; i < EdgePoints.Length - 1; i++)
                {
                    bmp.SetPixel(EdgePoints[i].X , EdgePoints[i].Y , Color.Red);
                }
                e.Graphics.DrawImage(bmp, new Point(0, 0));
                isTrain = false;
            }
        }

        private void button_open_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFi = new OpenFileDialog();
            openFi.Filter = "图像文件(Jpeg, Gif, Bmp, etc.)|*.jpg;*.jpeg;*.gif;*.bmp;*.tif; *.tiff; *.png| Jpeg 图像文件(*.jpg;*.jpeg)"
                + "|*.jpg;*.jpeg |GIF 图像文件(*.gif)|*.gif |BMP图像文件(*.bmp)|*.bmp|Tiff图像文件(*.tif;*.tiff)|*.tif;*.tiff|Png图像文件(*.png)"
                + "| *.png |所有文件(*.*)|*.*";
            if (openFi.ShowDialog() == DialogResult.OK)
            {
                //使用打开的图片路径创建位图对像
                string strHeadImagePath = openFi.FileName;
                SourceBitmap = new Bitmap(strHeadImagePath);
                this.pictureBox1.Image = SourceBitmap;
                int ImgWidth = pictureBox1.Image.Width;
                int ImgHeight = pictureBox1.Image.Height;

                SourceImage.width = ImgWidth;
                SourceImage.height = ImgHeight;

                //拷贝图象到处理模块
                if (SourceBitmap.PixelFormat != PixelFormat.Format8bppIndexed)
                {
                    SourceBitmap = RgbToGrayScale(SourceBitmap);
                }
                BitmapData bmpData = SourceBitmap.LockBits(new Rectangle(0, 0, ImgWidth, ImgHeight), ImageLockMode.WriteOnly, PixelFormat.Format8bppIndexed);
                SrcImgData = new byte[bmpData.Stride * ImgHeight];
                pSrcImgData = Marshal.AllocHGlobal(ImgWidth * ImgHeight);
                Marshal.Copy(bmpData.Scan0, SrcImgData, 0, bmpData.Stride * ImgHeight);
                Marshal.Copy(SrcImgData, 0, pSrcImgData, ImgWidth * ImgHeight);
                SourceBitmap.UnlockBits(bmpData);
                SourceImage.ImageData = pSrcImgData;
                isImageOK = true;
                this.pictureBox1.Invalidate();
            }
        }

        private void button_setroi_Click(object sender, EventArgs e)
        {
            isSetROI = true;
        }

        private void button_trainmodel_Click(object sender, EventArgs e)
        {
            trainparam TrainParam;
            TrainParam.MinContrast = Convert.ToInt32(textBox1.Text); 
            TrainParam.Contrast       = Convert.ToInt32(textBox2.Text);
            TrainParam.Granularity   = Convert.ToInt32(textBox3.Text);

            if (TempleImage.width == 0 || TempleImage.height == 0)
            {
                MessageBox.Show("图像大小不符！");
            }
            IntPtr pEdgeList = Marshal.AllocHGlobal(TempleImage.width * TempleImage.height * Marshal.SizeOf(typeof(iPoint)));
            ShapeMatchAPI.TrainShapeModel(TempleImage, TrainParam, pEdgeList, ref EdgeListSize);
            iPoint[] EdgeList = new iPoint[EdgeListSize];

            if (EdgeListSize != 0)
            {
                isTrain = true;
                EdgePoints = new Point[EdgeListSize];
                for (int i = 0; i < EdgeListSize; i++)
                {
                    IntPtr ptr = new IntPtr(pEdgeList.ToInt32() + Marshal.SizeOf(typeof(iPoint)) * i); 
                    EdgeList[i] = (iPoint)Marshal.PtrToStructure(ptr, typeof(iPoint));
                    EdgePoints[i].X = EdgeList[i].X;
                    EdgePoints[i].Y = EdgeList[i].Y;
                }
                this.pictureBox2.Invalidate();
                MessageBox.Show("训练成功！");
            }
        }

        private void button_createmodel_Click(object sender, EventArgs e)
        {
            if (TempleImage.width == 0 || TempleImage.height == 0)
            {
                MessageBox.Show("图像大小不符！");
            }
            modelparam ModelParam;
            ModelParam.AngleStart          = Convert.ToInt32(textBox4.Text);
            ModelParam.AngleStop          = Convert.ToInt32(textBox5.Text);
            ModelParam.AngleStep          = Convert.ToInt32(textBox6.Text);
            ModelParam.NumLevels         = Convert.ToInt32(textBox7.Text);
            ModelParam.PointReduction  = 1;
            isCreate = ShapeMatchAPI.CreateShapeModel(TempleImage, ModelParam, 0);
            if (isCreate)
            {
                MessageBox.Show("创建成功！");
            }
            else
                MessageBox.Show("创建失败！");
        }

        private void button_releasemodel_Click(object sender, EventArgs e)
        {
            ShapeMatchAPI.ReleaseShapeModel(0);
        }

        private void button_findmodel_Click(object sender, EventArgs e)
        {
            matchparam MatchParam;
            MatchParam.NumMatches = Convert.ToInt32(textBox8.Text);
            MatchParam.MinScore       = (float)Convert.ToDouble(textBox9.Text);
            MatchParam.Greediness     = (float)Convert.ToDouble(textBox10.Text);
            MatchParam.ID = 0;
            MatchNum = 0;
            IntPtr pResultList = Marshal.AllocHGlobal(10 * Marshal.SizeOf(typeof(matchresult)));

            System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
            sw.Start();
            ShapeMatchAPI.FindShapeModel(SourceImage, 0, MatchParam, pResultList, ref MatchNum);
            sw.Stop();
            long totalTime = sw.ElapsedMilliseconds;

            label13.Text = totalTime.ToString() + " ms";
            label15.Text = MatchNum.ToString();

            if (MatchNum != 0)
            {
                ResultList = new matchresult[MatchNum];
                for (int i = 0; i < MatchNum; i++)
                {
                    IntPtr ptr = new IntPtr(pResultList.ToInt32() + Marshal.SizeOf(typeof(matchresult)) * i);
                    ResultList[i] = (matchresult)Marshal.PtrToStructure(ptr, typeof(matchresult));
                }
                isMatch = true;
                this.pictureBox1.Invalidate();
            }
            else
                MessageBox.Show("匹配失败！");
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            ShapeMatchAPI.ReleaseShapeModel(0);
        }

        // 用灰度数组新建一个8位灰度图像。  
        private static Bitmap BuiltGrayBitmap(byte[] rawValues, int width, int height)
        {
            // 新建一个8位灰度位图，并锁定内存区域操作  
            Bitmap bitmap = new Bitmap(width, height, PixelFormat.Format8bppIndexed);
            BitmapData bmpData = bitmap.LockBits(new Rectangle(0, 0, width, height),
                 ImageLockMode.WriteOnly, PixelFormat.Format8bppIndexed);

            // 计算图像参数  
            int offset = bmpData.Stride - bmpData.Width;        // 计算每行未用空间字节数  
            IntPtr ptr = bmpData.Scan0;                         // 获取首地址  
            int scanBytes = bmpData.Stride * bmpData.Height;    // 图像字节数 = 扫描字节数 * 高度  
            byte[] grayValues = new byte[scanBytes];            // 为图像数据分配内存  

            // 为图像数据赋值  
            int posSrc = 0, posScan = 0;                        // rawValues和grayValues的索引  
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    grayValues[posScan++] = rawValues[posSrc++];
                }
                // 跳过图像数据每行未用空间的字节，length = stride - width * bytePerPixel  
                posScan += offset;
            }

            // 内存解锁  
            Marshal.Copy(grayValues, 0, ptr, scanBytes);
            bitmap.UnlockBits(bmpData);  // 解锁内存区域  

            // 修改生成位图的索引表，从伪彩修改为灰度  
            ColorPalette palette;
            // 获取一个Format8bppIndexed格式图像的Palette对象  
            using (Bitmap bmp = new Bitmap(1, 1, PixelFormat.Format8bppIndexed))
            {
                palette = bmp.Palette;
            }
            for (int i = 0; i < 256; i++)
            {
                palette.Entries[i] = Color.FromArgb(i, i, i);
            }
            // 修改生成位图的索引表  
            bitmap.Palette = palette;

            return bitmap;
        }  

        // 将源图像灰度化，并转化为8位灰度图像。   
        private static Bitmap RgbToGrayScale(Bitmap original)
        {
            if (original != null)
            {
                // 将源图像内存区域锁定  
                Rectangle rect = new Rectangle(0, 0, original.Width, original.Height);
                BitmapData bmpData = original.LockBits(rect, ImageLockMode.ReadOnly,
                     original.PixelFormat);

                // 获取图像参数  
                int width = bmpData.Width;
                int height = bmpData.Height;
                int stride = bmpData.Stride;  // 扫描线的宽度  
                int offset = stride - width * 3;  // 显示宽度与扫描线宽度的间隙  
                IntPtr ptr = bmpData.Scan0;   // 获取bmpData的内存起始位置  
                int scanBytes = stride * height;  // 用stride宽度，表示这是内存区域的大小  

                // 分别设置两个位置指针，指向源数组和目标数组  
                int posScan = 0, posDst = 0;
                byte[] rgbValues = new byte[scanBytes];  // 为目标数组分配内存  
                Marshal.Copy(ptr, rgbValues, 0, scanBytes);  // 将图像数据拷贝到rgbValues中  
                // 分配灰度数组  
                byte[] grayValues = new byte[width * height]; // 不含未用空间。  
                // 计算灰度数组  
                for (int i = 0; i < height; i++)
                {
                    for (int j = 0; j < width; j++)
                    {
                        double temp = rgbValues[posScan++] * 0.11 +
                            rgbValues[posScan++] * 0.59 +
                            rgbValues[posScan++] * 0.3;
                        grayValues[posDst++] = (byte)temp;
                    }
                    // 跳过图像数据每行未用空间的字节，length = stride - width * bytePerPixel  
                    posScan += offset;
                }

                // 内存解锁  
                Marshal.Copy(rgbValues, 0, ptr, scanBytes);
                original.UnlockBits(bmpData);  // 解锁内存区域  

                // 构建8位灰度位图  
                Bitmap retBitmap = BuiltGrayBitmap(grayValues, width, height);
                return retBitmap;
            }
            else
            {
                return null;
            }
        }

        public struct RegionCut
        {
            public int TopLeftX;                //区域左上角X坐标
            public int TopLeftY;                //区域左上角Y坐标
            public int BottomRightX;        //区域右下角X坐标
            public int BottomRightY;        //区域右下角Y坐标
        }

        #region variable
        public const int IMG_WIDTH = 1280;
        public const int IMG_HEIGHT = 1024;
        public RegionCut Roi;
        public bool isInitailed = false;
        public bool isImageOK = false;
        public bool isSetROI = false;
        public bool isTrain = false;
        public bool isCreate = false;
        public bool isMatch = false;
        public iImage SourceImage;
        public iImage TempleImage;
        public Bitmap SourceBitmap;
        public Bitmap TempleBitmap;
        public IntPtr pSrcImgData;
        public IntPtr pTmpImgData;
        public byte[] SrcImgData;
        public byte[] TmpImgData;
        public Point[] EdgePoints;
        public matchresult[] ResultList; 
        public int EdgeListSize = 0;
        public int MatchNum = 0;
        public double result = 0;
        #endregion
    }
}
