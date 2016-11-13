using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;

namespace wfrc
{
    public partial class Form1 : Form
    {
        dplnet dn;
        int sd = 0;
        int csd = 0;
        byte[] buf = new byte[512];
        byte[] ip = new byte[16];
        int btn_open_sts = 0;
        public Form1()
        {
           InitializeComponent();
           dn = new dplnet();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(btn_open_sts ==0)
            { 
                btn_open_sts = 1;
                button1.Text = "접속해제";
                byte[] ip = Encoding.Default.GetBytes(textBox1.Text);
                int type = Int32.Parse(textBox3.Text);
                int typ = 0;
                if(type == 1)  typ = 0x08; 
                sd = dn.DNAOpen(ip, 9000, typ);
                textBox2.Text += textBox1.Text + " Open \r\n";
                if(type == 0 && sd>0)
                {
                    csd = dn.DNAAccept(sd, buf, 0);
                    if(csd > 0)
                    {
                        textBox2.Text += "CONNECT RC CAR \r\n";
                    }
                }
            }
            else
                {
                    btn_open_sts = 0;
                    button1.Text="접속";
                    textBox2.Text += textBox1.Text+" CLOSE\r\n";
                    dn.DNAClose(sd);
                }            
            }
        private void button2_Click_1(object sender, EventArgs e)
        {

            if (comboBox1.SelectedIndex == comboBox2.SelectedIndex)
                textBox2.Text += " 이동 불가 ";
            else
                textBox2.Text += (comboBox1.SelectedItem + " 에서 " + comboBox2.SelectedItem + "으로 이동 시작...");
            textBox2.Text += "\r\n";
            byte[] wbuf = Encoding.Default.GetBytes(comboBox1.Text + comboBox2.Text);
            dn.DNAWrite(sd, wbuf, (comboBox1.Text + comboBox2.Text).Length, null, 0);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            int err = 0;
            err = dn.DNARead(csd, buf, 512, ip, 0);
            if(err>0)
            {
                buf[err] = 0;
                textBox2.Text += Encoding.Default.GetString(buf);
                textBox2.Text += "\r\n";
                textBox2.Select(textBox2.Text.Length, 0);
                textBox2.ScrollToCaret();
            }
        }
    }
}