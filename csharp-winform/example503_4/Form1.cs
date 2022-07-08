using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;

namespace example503_4
{
    public partial class Form1 : Form
    {
        MqttClient client;
        string clientId;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            
        }

        void client_MqttMsgPublishReceived(object sender, MqttMsgPublishEventArgs e)
        {
            string ReceivedMessage = Encoding.UTF8.GetString(e.Message);

            //DO SOMETHING..!
            //richTextBox1.Text += ReceivedMessage + "\n";
            string[] mydata = ReceivedMessage.Split(',');
            label1.Text = "PM1.0 : " + mydata[0] + "ug/m3";
            label2.Text = "PM2.5 : " + mydata[1] + "ug/m3";
            label3.Text = "PM10.0 : " + mydata[2] + "ug/m3";
            label4.Text = "수집시간 : " + DateTime.Now.ToString();

        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (client != null && client.IsConnected)
            {
                client.Disconnect();
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //MQTT 브로커와 연결하는 부분
            string BrokerAddress = textBox1.Text;
            client = new MqttClient(BrokerAddress);

            // register a callback-function (we have to implement, see below) which is called by the library when a message was received
            client.MqttMsgPublishReceived += client_MqttMsgPublishReceived;

            // use a unique id as client id, each time we start the application
            clientId = Guid.NewGuid().ToString();
            client.Connect(clientId);

            //구독등록 (subscribe)
            client.Subscribe(new string[] { textBox2.Text }, new byte[] { 0 });
        }
    }
}
