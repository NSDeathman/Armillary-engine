using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Armillary_project_manager
{
    public partial class Form1 : Form
    {
        private int clickCount = 0;

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            clickCount++;
            label2.Text = $"Счетчик: {clickCount}";
            MessageBox.Show($"Вы нажали кнопку! Счетчик: {clickCount}");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            clickCount = 0;
            label2.Text = "Счетчик: 0";
            MessageBox.Show("Счетчик сброшен!");
        }
    }
}
