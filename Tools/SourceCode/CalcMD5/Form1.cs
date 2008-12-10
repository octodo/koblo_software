using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace CalcMD5dotNET
{
	public partial class Form1 : Form
	{
		bool m_bLock = false;
		System.Security.Cryptography.MD5 md5 = System.Security.Cryptography.MD5.Create();

		public Form1()
		{
			InitializeComponent();
		}

		private void Form1_Load(object sender, EventArgs e)
		{

		}

		private byte[] StringToByteArray(string s)
		{
			char[] acIn = s.ToCharArray();
			byte[] auc = new byte[acIn.Length];
			for (int i = 0; i < acIn.Length; i++)
			{
				auc[i] = Convert.ToByte(acIn[i]);
			}
			return auc;
		}

		private string ByteArrayToHexString(byte[] auc)
		{
			char[] acNibble = "0123456789abcdef".ToCharArray();
			string s = "";
			for (int iIx = 0; iIx < auc.Length; iIx++)
			{
				byte uc = auc[iIx];
				s += acNibble[uc >> 4];
				s += acNibble[uc & 0x0F];
			}
			return s;
		}

		private void DoFile(bool bShow)
		{
			if (m_bLock)
				return;
			m_bLock = true;

			txtFileLength.Text = "";
			try
			{
				openFileDialog1.CheckFileExists = true;
				if (txtFile.Text != "")
					openFileDialog1.FileName = txtFile.Text;
				if ((!bShow) || (openFileDialog1.ShowDialog() == DialogResult.OK))
				{
					byte[] aucMD5_Name = md5.ComputeHash(StringToByteArray(txtFile.Text));
					txtMD5_Name.Text = ByteArrayToHexString(aucMD5_Name);
					System.IO.Stream f = openFileDialog1.OpenFile();
					if (f != null)
					{
						txtFileLength.Text = f.Length.ToString();
						txtFile.Text = openFileDialog1.FileName;
						byte[] aucMD5 = md5.ComputeHash(f);
						f.Close();
						txtMD5_Contents.Text = ByteArrayToHexString(aucMD5);
					}
				}
			}
			catch
			{
				txtMD5_Contents.Text = "(error reading file)";
			}

			m_bLock = false;
		}

		private void txtFile_TextChanged(object sender, EventArgs e)
		{
			DoFile(false);
		}
	}
}