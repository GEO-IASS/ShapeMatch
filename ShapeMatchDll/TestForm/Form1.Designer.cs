namespace TestForm
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.button_open = new System.Windows.Forms.Button();
            this.pictureBox2 = new System.Windows.Forms.PictureBox();
            this.button_setroi = new System.Windows.Forms.Button();
            this.button_trainmodel = new System.Windows.Forms.Button();
            this.button_createmodel = new System.Windows.Forms.Button();
            this.button_findmodel = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.TrainModel = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.CreateModel = new System.Windows.Forms.GroupBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.textBox7 = new System.Windows.Forms.TextBox();
            this.textBox6 = new System.Windows.Forms.TextBox();
            this.textBox5 = new System.Windows.Forms.TextBox();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.FindModel = new System.Windows.Forms.GroupBox();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.textBox10 = new System.Windows.Forms.TextBox();
            this.textBox9 = new System.Windows.Forms.TextBox();
            this.textBox8 = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.button_releasemodel = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
            this.TrainModel.SuspendLayout();
            this.panel1.SuspendLayout();
            this.CreateModel.SuspendLayout();
            this.FindModel.SuspendLayout();
            this.SuspendLayout();
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackColor = System.Drawing.SystemColors.ControlDark;
            this.pictureBox1.Location = new System.Drawing.Point(0, 0);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(640, 480);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pictureBox1.TabIndex = 0;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox1_Paint);
            this.pictureBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseDown);
            this.pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseMove);
            this.pictureBox1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseUp);
            // 
            // button_open
            // 
            this.button_open.Location = new System.Drawing.Point(659, 224);
            this.button_open.Name = "button_open";
            this.button_open.Size = new System.Drawing.Size(75, 23);
            this.button_open.TabIndex = 1;
            this.button_open.Text = "打开图片";
            this.button_open.UseVisualStyleBackColor = true;
            this.button_open.Click += new System.EventHandler(this.button_open_Click);
            // 
            // pictureBox2
            // 
            this.pictureBox2.BackColor = System.Drawing.SystemColors.ControlDark;
            this.pictureBox2.Location = new System.Drawing.Point(658, 12);
            this.pictureBox2.Name = "pictureBox2";
            this.pictureBox2.Size = new System.Drawing.Size(226, 200);
            this.pictureBox2.TabIndex = 2;
            this.pictureBox2.TabStop = false;
            this.pictureBox2.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox2_Paint);
            // 
            // button_setroi
            // 
            this.button_setroi.Location = new System.Drawing.Point(740, 224);
            this.button_setroi.Name = "button_setroi";
            this.button_setroi.Size = new System.Drawing.Size(75, 23);
            this.button_setroi.TabIndex = 3;
            this.button_setroi.Text = "设置ROI";
            this.button_setroi.UseVisualStyleBackColor = true;
            this.button_setroi.Click += new System.EventHandler(this.button_setroi_Click);
            // 
            // button_trainmodel
            // 
            this.button_trainmodel.Location = new System.Drawing.Point(135, 51);
            this.button_trainmodel.Name = "button_trainmodel";
            this.button_trainmodel.Size = new System.Drawing.Size(75, 23);
            this.button_trainmodel.TabIndex = 5;
            this.button_trainmodel.Text = "训练模板";
            this.button_trainmodel.UseVisualStyleBackColor = true;
            this.button_trainmodel.Click += new System.EventHandler(this.button_trainmodel_Click);
            // 
            // button_createmodel
            // 
            this.button_createmodel.Location = new System.Drawing.Point(22, 71);
            this.button_createmodel.Name = "button_createmodel";
            this.button_createmodel.Size = new System.Drawing.Size(75, 23);
            this.button_createmodel.TabIndex = 6;
            this.button_createmodel.Text = "创建模板";
            this.button_createmodel.UseVisualStyleBackColor = true;
            this.button_createmodel.Click += new System.EventHandler(this.button_createmodel_Click);
            // 
            // button_findmodel
            // 
            this.button_findmodel.Location = new System.Drawing.Point(138, 46);
            this.button_findmodel.Name = "button_findmodel";
            this.button_findmodel.Size = new System.Drawing.Size(75, 23);
            this.button_findmodel.TabIndex = 7;
            this.button_findmodel.Text = "搜索匹配";
            this.button_findmodel.UseVisualStyleBackColor = true;
            this.button_findmodel.Click += new System.EventHandler(this.button_findmodel_Click);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(62, 20);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(35, 21);
            this.textBox1.TabIndex = 8;
            this.textBox1.Text = "30";
            this.textBox1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(175, 20);
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(35, 21);
            this.textBox2.TabIndex = 8;
            this.textBox2.Text = "120";
            this.textBox2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // textBox3
            // 
            this.textBox3.Location = new System.Drawing.Point(62, 53);
            this.textBox3.Name = "textBox3";
            this.textBox3.Size = new System.Drawing.Size(35, 21);
            this.textBox3.TabIndex = 8;
            this.textBox3.Text = "1";
            this.textBox3.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // TrainModel
            // 
            this.TrainModel.Controls.Add(this.label3);
            this.TrainModel.Controls.Add(this.label2);
            this.TrainModel.Controls.Add(this.label1);
            this.TrainModel.Controls.Add(this.textBox3);
            this.TrainModel.Controls.Add(this.textBox1);
            this.TrainModel.Controls.Add(this.textBox2);
            this.TrainModel.Controls.Add(this.button_trainmodel);
            this.TrainModel.Location = new System.Drawing.Point(659, 253);
            this.TrainModel.Name = "TrainModel";
            this.TrainModel.Size = new System.Drawing.Size(226, 81);
            this.TrainModel.TabIndex = 9;
            this.TrainModel.TabStop = false;
            this.TrainModel.Text = "训练模板";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(16, 56);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(41, 12);
            this.label3.TabIndex = 9;
            this.label3.Text = "颗粒度";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(128, 23);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(41, 12);
            this.label2.TabIndex = 9;
            this.label2.Text = "高阈值";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(15, 23);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 12);
            this.label1.TabIndex = 9;
            this.label1.Text = "低阈值";
            // 
            // panel1
            // 
            this.panel1.AutoScroll = true;
            this.panel1.Controls.Add(this.pictureBox1);
            this.panel1.Location = new System.Drawing.Point(12, 12);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(640, 480);
            this.panel1.TabIndex = 4;
            // 
            // CreateModel
            // 
            this.CreateModel.Controls.Add(this.label7);
            this.CreateModel.Controls.Add(this.label8);
            this.CreateModel.Controls.Add(this.label6);
            this.CreateModel.Controls.Add(this.label5);
            this.CreateModel.Controls.Add(this.label4);
            this.CreateModel.Controls.Add(this.textBox7);
            this.CreateModel.Controls.Add(this.textBox6);
            this.CreateModel.Controls.Add(this.button_releasemodel);
            this.CreateModel.Controls.Add(this.button_createmodel);
            this.CreateModel.Controls.Add(this.textBox5);
            this.CreateModel.Controls.Add(this.textBox4);
            this.CreateModel.Location = new System.Drawing.Point(659, 343);
            this.CreateModel.Name = "CreateModel";
            this.CreateModel.Size = new System.Drawing.Size(226, 100);
            this.CreateModel.TabIndex = 10;
            this.CreateModel.TabStop = false;
            this.CreateModel.Text = "创建模板";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(-93, 82);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(53, 12);
            this.label7.TabIndex = 7;
            this.label7.Text = "起始角度";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(110, 50);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(65, 12);
            this.label8.TabIndex = 7;
            this.label8.Text = "金字塔级数";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(8, 50);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(53, 12);
            this.label6.TabIndex = 7;
            this.label6.Text = "角度步长";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(116, 21);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(53, 12);
            this.label5.TabIndex = 7;
            this.label5.Text = "终止角度";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(8, 21);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(53, 12);
            this.label4.TabIndex = 7;
            this.label4.Text = "起始角度";
            // 
            // textBox7
            // 
            this.textBox7.Location = new System.Drawing.Point(175, 44);
            this.textBox7.Name = "textBox7";
            this.textBox7.Size = new System.Drawing.Size(35, 21);
            this.textBox7.TabIndex = 8;
            this.textBox7.Text = "3";
            this.textBox7.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // textBox6
            // 
            this.textBox6.Location = new System.Drawing.Point(62, 47);
            this.textBox6.Name = "textBox6";
            this.textBox6.Size = new System.Drawing.Size(35, 21);
            this.textBox6.TabIndex = 8;
            this.textBox6.Text = "1";
            this.textBox6.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // textBox5
            // 
            this.textBox5.Location = new System.Drawing.Point(175, 17);
            this.textBox5.Name = "textBox5";
            this.textBox5.Size = new System.Drawing.Size(35, 21);
            this.textBox5.TabIndex = 8;
            this.textBox5.Text = "360";
            this.textBox5.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // textBox4
            // 
            this.textBox4.Location = new System.Drawing.Point(62, 18);
            this.textBox4.Name = "textBox4";
            this.textBox4.Size = new System.Drawing.Size(35, 21);
            this.textBox4.TabIndex = 8;
            this.textBox4.Text = "0";
            this.textBox4.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // FindModel
            // 
            this.FindModel.Controls.Add(this.button_findmodel);
            this.FindModel.Controls.Add(this.label11);
            this.FindModel.Controls.Add(this.label10);
            this.FindModel.Controls.Add(this.label9);
            this.FindModel.Controls.Add(this.textBox10);
            this.FindModel.Controls.Add(this.textBox9);
            this.FindModel.Controls.Add(this.textBox8);
            this.FindModel.Location = new System.Drawing.Point(659, 452);
            this.FindModel.Name = "FindModel";
            this.FindModel.Size = new System.Drawing.Size(226, 81);
            this.FindModel.TabIndex = 11;
            this.FindModel.TabStop = false;
            this.FindModel.Text = "搜索匹配";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(11, 51);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(41, 12);
            this.label11.TabIndex = 7;
            this.label11.Text = "贪婪度";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(116, 24);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(53, 12);
            this.label10.TabIndex = 7;
            this.label10.Text = "最小评分";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(11, 24);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(53, 12);
            this.label9.TabIndex = 7;
            this.label9.Text = "匹配个数";
            // 
            // textBox10
            // 
            this.textBox10.Location = new System.Drawing.Point(70, 46);
            this.textBox10.Name = "textBox10";
            this.textBox10.Size = new System.Drawing.Size(35, 21);
            this.textBox10.TabIndex = 8;
            this.textBox10.Text = "0.9";
            this.textBox10.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // textBox9
            // 
            this.textBox9.Location = new System.Drawing.Point(175, 21);
            this.textBox9.Name = "textBox9";
            this.textBox9.Size = new System.Drawing.Size(35, 21);
            this.textBox9.TabIndex = 8;
            this.textBox9.Text = "0.7";
            this.textBox9.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // textBox8
            // 
            this.textBox8.Location = new System.Drawing.Point(70, 20);
            this.textBox8.Name = "textBox8";
            this.textBox8.Size = new System.Drawing.Size(35, 21);
            this.textBox8.TabIndex = 8;
            this.textBox8.Text = "1";
            this.textBox8.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(12, 509);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(59, 12);
            this.label12.TabIndex = 7;
            this.label12.Text = "搜索耗时:";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(66, 509);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(29, 12);
            this.label13.TabIndex = 7;
            this.label13.Text = "0 ms";
            this.label13.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(119, 509);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(59, 12);
            this.label14.TabIndex = 7;
            this.label14.Text = "搜索个数:";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(173, 509);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(11, 12);
            this.label15.TabIndex = 7;
            this.label15.Text = "0";
            this.label15.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // button_releasemodel
            // 
            this.button_releasemodel.Location = new System.Drawing.Point(135, 71);
            this.button_releasemodel.Name = "button_releasemodel";
            this.button_releasemodel.Size = new System.Drawing.Size(75, 23);
            this.button_releasemodel.TabIndex = 6;
            this.button_releasemodel.Text = "销毁模板";
            this.button_releasemodel.UseVisualStyleBackColor = true;
            this.button_releasemodel.Click += new System.EventHandler(this.button_releasemodel_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(891, 536);
            this.Controls.Add(this.FindModel);
            this.Controls.Add(this.label15);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.label14);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.CreateModel);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.button_setroi);
            this.Controls.Add(this.pictureBox2);
            this.Controls.Add(this.button_open);
            this.Controls.Add(this.TrainModel);
            this.Name = "Form1";
            this.Text = "Form1";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
            this.TrainModel.ResumeLayout(false);
            this.TrainModel.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.CreateModel.ResumeLayout(false);
            this.CreateModel.PerformLayout();
            this.FindModel.ResumeLayout(false);
            this.FindModel.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Button button_open;
        private System.Windows.Forms.PictureBox pictureBox2;
        private System.Windows.Forms.Button button_setroi;
        private System.Windows.Forms.Button button_trainmodel;
        private System.Windows.Forms.Button button_createmodel;
        private System.Windows.Forms.Button button_findmodel;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.GroupBox TrainModel;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.GroupBox CreateModel;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textBox4;
        private System.Windows.Forms.TextBox textBox5;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox textBox7;
        private System.Windows.Forms.TextBox textBox6;
        private System.Windows.Forms.GroupBox FindModel;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox textBox8;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox textBox9;
        private System.Windows.Forms.TextBox textBox10;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Button button_releasemodel;
    }
}

