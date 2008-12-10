namespace CalcMD5dotNET
{
	partial class Form1
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.txtMD5_Name = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
			this.txtFile = new System.Windows.Forms.TextBox();
			this.File = new System.Windows.Forms.Label();
			this.txtMD5_Contents = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.txtFileLength = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// txtMD5_Name
			// 
			this.txtMD5_Name.Location = new System.Drawing.Point(12, 81);
			this.txtMD5_Name.Name = "txtMD5_Name";
			this.txtMD5_Name.ReadOnly = true;
			this.txtMD5_Name.Size = new System.Drawing.Size(393, 20);
			this.txtMD5_Name.TabIndex = 2;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(9, 65);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(118, 13);
			this.label2.TabIndex = 1;
			this.label2.Text = "MD5 of string / file path";
			// 
			// openFileDialog1
			// 
			this.openFileDialog1.FileName = "openFileDialog1";
			// 
			// txtFile
			// 
			this.txtFile.AllowDrop = true;
			this.txtFile.Location = new System.Drawing.Point(12, 25);
			this.txtFile.Name = "txtFile";
			this.txtFile.Size = new System.Drawing.Size(798, 20);
			this.txtFile.TabIndex = 0;
			this.txtFile.TextChanged += new System.EventHandler(this.txtFile_TextChanged);
			// 
			// File
			// 
			this.File.AutoSize = true;
			this.File.Location = new System.Drawing.Point(13, 9);
			this.File.Name = "File";
			this.File.Size = new System.Drawing.Size(85, 13);
			this.File.TabIndex = 1;
			this.File.Text = "String / File path";
			// 
			// txtMD5_Contents
			// 
			this.txtMD5_Contents.Location = new System.Drawing.Point(417, 81);
			this.txtMD5_Contents.Name = "txtMD5_Contents";
			this.txtMD5_Contents.ReadOnly = true;
			this.txtMD5_Contents.Size = new System.Drawing.Size(393, 20);
			this.txtMD5_Contents.TabIndex = 3;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(414, 65);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(102, 13);
			this.label1.TabIndex = 1;
			this.label1.Text = "MD5 of file contents";
			// 
			// txtFileLength
			// 
			this.txtFileLength.Location = new System.Drawing.Point(417, 137);
			this.txtFileLength.Name = "txtFileLength";
			this.txtFileLength.ReadOnly = true;
			this.txtFileLength.Size = new System.Drawing.Size(393, 20);
			this.txtFileLength.TabIndex = 3;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(414, 121);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(55, 13);
			this.label3.TabIndex = 1;
			this.label3.Text = "File length";
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(822, 169);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.File);
			this.Controls.Add(this.txtFileLength);
			this.Controls.Add(this.txtMD5_Contents);
			this.Controls.Add(this.txtMD5_Name);
			this.Controls.Add(this.txtFile);
			this.Name = "Form1";
			this.Text = "Calculate MD5";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TextBox txtMD5_Name;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.OpenFileDialog openFileDialog1;
		private System.Windows.Forms.TextBox txtFile;
		private System.Windows.Forms.Label File;
		private System.Windows.Forms.TextBox txtMD5_Contents;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox txtFileLength;
		private System.Windows.Forms.Label label3;
	}
}

