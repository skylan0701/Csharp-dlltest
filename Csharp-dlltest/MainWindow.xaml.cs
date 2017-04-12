using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Csharp_dlltest
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        [DllImport(@"C:\Users\sky\Documents\Visual Studio 2015\Projects\dlltest\Debug\dlltest.dll", EntryPoint = "my_send")]
        extern static int my_send(string filename);
        [DllImport(@"C:\Users\sky\Documents\Visual Studio 2015\Projects\dlltest\Debug\dlltest.dll", EntryPoint = "my_recv")]
        extern static int my_recv(ref int flag); 
        [DllImport(@"C:\Users\sky\Documents\Visual Studio 2015\Projects\dlltest\Debug\dlltest.dll", EntryPoint = "luyin")]
        extern static int luyin(ref int flag);
        public Thread tsend,trecv;
        public Thread watchdog;
        int flag = 0;
        public MainWindow()
        {
            InitializeComponent();
            trecv = new Thread(new ThreadStart(csharp_recv));
            trecv.IsBackground = true;
            trecv.Start();
            tsend = new Thread(new ThreadStart(csharp_send));
            tsend.IsBackground = true;
            tsend.Start();
        }
        public void csharp_send(){
                luyin(ref flag);
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
            if (status.Content.Equals("发送"))
            {
                status.Content = "接收";
                sendstatus.Content = "等待中....";
                recvstatus.Content = "接收中....！";
                flag = 0;
            }
            else
            {
                status.Content = "发送";
                recvstatus.Content = "等待中....";
                sendstatus.Content = "发送中....！";
                flag = 1;
            }
            
        }

        public void csharp_recv(){
                my_recv(ref flag);
            /*            if (a == 0)
                        {
                            this.Dispatcher.Invoke(new Action(delegate {
                                //这里写代码     
                                recvstatus.Content = "success!";
                            }));
                            trecv = new Thread(csharp_recv);
                            trecv.Start();
                        }else if(a == -1)
                        {
                            this.Dispatcher.Invoke(new Action(delegate {
                                //这里写代码     
                                recvstatus.Content = "bind error!";
                            }));
                        }else if(a == -2)
                        {
                            this.Dispatcher.Invoke(new Action(delegate {
                                //这里写代码     
                                recvstatus.Content = "recvfrom error!";
                            }));
                        }else
                        {
                            this.Dispatcher.Invoke(new Action(delegate {
                                //这里写代码     
                                recvstatus.Content = "recvfrom yu error!";
                            }));
                        }
            */
        }
        public void watch()
        {
            

        }
    }
}
