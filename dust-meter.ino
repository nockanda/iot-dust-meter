#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

#include "PMS.h"
#include <SoftwareSerial.h>

SoftwareSerial nockanda(D1,D2);

PMS pms(nockanda);
PMS::DATA data;

#define btn D5

//AP모드일때의 정보
#ifndef APSSID
#define APSSID "nockanda_pms"
#define APPSK  "11213144"
#endif

const char * ap_ssid = APSSID;
const char * ap_password = APPSK;
ESP8266WebServer server(80);

// Update these with values suitable for your network.

String ssid = "";
String password = "";
String mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

String input_page = "\
<head>\
<meta name=\"viewport\" content=\"width=device-width\">\
<meta charset=\"utf-8\">\
</head>\
<form action=process method=post>\
<table width=500 border=1>\
  <tr>\
     <th colspan=2>녹칸다의 온습도센서!</th>\
  </tr>\
  <tr>\
     <td width=200>공유기id</td>\
     <td><input type=text name=id value=%id%></td>\
  </tr>\
  <tr>\
     <td>공유기패스워드</td>\
     <td><input type=text name=pw value=%pw%></td>\
  </tr>\
  <tr>\
     <td>mqtt서버주소</td>\
     <td><input type=text name=addr value=%addr%></td>\
  </tr>\
  <tr>\
     <td>mqtt토픽</td>\
     <td><input type=text name=topic value=%topic%></td>\
  </tr>\
  <tr>\
     <td>업로드간격</td>\
     <td><input type=text name=time value=%time%></td>\
  </tr>\
  <td colspan=2 align=center><input type=submit value=설정하기></td>\
</table>\
</form>\
";

void handleRoot() {
  //wemos d1 mini보드가 ap모드로 동작하면서
  //스마트폰쪽으로 응답을 전송하는 부분!

  ssid = readFile("id.txt");
  password = readFile("pw.txt");
  mqtt_server = readFile("addr.txt");
  String mytopic = readFile("topic.txt");
  String mytime2 = readFile("time.txt");

  input_page.replace("%id%",ssid);
  input_page.replace("%pw%",password);
  input_page.replace("%addr%",mqtt_server);
  input_page.replace("%topic%",mytopic);
  input_page.replace("%time%",mytime2);
  
  server.send(200, "text/html", input_page);
  //delay(2000);
  //ESP.restart(); //보드가 강제로 재부팅된다!
}

void handleProcess(){
  //server.args()
  //server.arg(i)
  //server.argName(i)
  /*
  Serial.println("새로운 메시지가 도착했습니다!");
  for (uint8_t i = 0; i < server.args(); i++) {
      Serial.println(server.argName(i) + ": " + server.arg(i));
  }
  */
  //server.arg(0) : id
  //파일로써 설정값을 업데이트하는 부분!
  writeFile("id.txt",server.arg(0).c_str());
  writeFile("pw.txt",server.arg(1).c_str());
  writeFile("addr.txt",server.arg(2).c_str());
  writeFile("topic.txt",server.arg(3).c_str());
  writeFile("time.txt",server.arg(4).c_str());
  server.send(200, "text/html", "OK");
  //넘겨받은 값으로 설정값을 업데이트하면 끝난다
  //그리고나서 재부팅하기
  delay(1000);
  ESP.restart();
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void nockanda_connect() {
  // Loop until we're reconnected
  Serial.print("Attempting MQTT connection...");
  // Create a random client ID
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);
  // Attempt to connect
  if (client.connect(clientId.c_str())) {
    Serial.println("connected");
    // Once connected, publish an announcement...
    //client.publish("outTopic", "hello world");
    // ... and resubscribe
    //client.subscribe("inTopic");
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  pinMode(btn,INPUT);

  Serial.println("깨어났다!");

  //나는 파일 시스템에 접근을 할거야~~
  if (!SPIFFS.begin())
  {
    Serial.println("Failed to mount file system");
    return;
  }

  //여기서 버튼이 눌려지면 설정모드로 가고
  //아니면 원래 하기로 했던 작업을 수행한다
  if(digitalRead(btn) == LOW){
    Serial.println("설정모드로 진입합니다"); 
    //여기서부터 AP모드 동작
    WiFi.softAP(ap_ssid, ap_password);
    server.on("/", handleRoot);
    server.on("/process", handleProcess);
    server.begin();
    Serial.println("HTTP server started");
  }else{
    //정상작동하는 경우
    //spiffs로 기록된 정보를 읽어온다
      
    ssid = readFile("id.txt");
    password = readFile("pw.txt");
    mqtt_server = readFile("addr.txt");
    String mytopic = readFile("topic.txt");
    String mytime2 = readFile("time.txt");

    if(ssid != "" && password != "" && mqtt_server != "" && mytopic != "" && mytime2 != ""){
    
      int mytime = mytime2.toInt();
  
      setup_wifi(); //와이파이 접속하는 부분!
      client.setServer(mqtt_server.c_str(), 1883); //MQTT 서버 정보를 등록하는 부분
      //client.setCallback(callback);
      nockanda_connect(); //서버와 연결하는 부분

      nockanda.begin(9600);
      pms.passiveMode();
      pms.wakeUp();
      delay(5000); //5초간 쉰다(녹칸다가 임의로 적어놓은 숫자)

      if(client.connected()){
        //아~~ MQTT서버와 연결이 되었구나!
        pms.requestRead();

        
        if (pms.readUntil(data))
        {
          Serial.print("PM 1.0 (ug/m3): ");
          Serial.println(data.PM_AE_UG_1_0);
      
          Serial.print("PM 2.5 (ug/m3): ");
          Serial.println(data.PM_AE_UG_2_5);
      
          Serial.print("PM 10.0 (ug/m3): ");
          Serial.println(data.PM_AE_UG_10_0);
          String csv_data = String(data.PM_AE_UG_1_0) + "," + String(data.PM_AE_UG_2_5)+ "," + String(data.PM_AE_UG_10_0);
          Serial.println(csv_data);
          client.publish(mytopic.c_str(), csv_data.c_str());
          delay(500);
          Serial.println("10초간 딥슬립");
          ESP.deepSleep(mytime * 1000000); //10초
        }
        else
        {
          //측정에 실패하면 아무것도 안한다!
          Serial.println("No data.");
        }
        
      }
    }else{
      Serial.println("공장에서 따끈따끈하게 나온 제품이라 설정정보가 없습니다!");
      //입력정보가 없으면 어떡할래?
      ESP.deepSleep(0);
    }
  }
}

void loop() {
  server.handleClient();
}

void deleteFile(const char * path){
  Serial.printf("Deleting file: %s\r\n", path);
  if(SPIFFS.remove(path)){
    Serial.println("file deleted");
  } else {
    Serial.println("delete failed");
  }
}

String readFile(const char * path){
  String output = "";
  Serial.printf("Reading file: %s\r\n", path);
  File file = SPIFFS.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return "";
  }
    Serial.println("read from file:");
    while(file.available()){
      char c = file.read();
      output += c;
      //Serial.write(c);
    }

    return output;
}
  
void writeFile(const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);
  File file = SPIFFS.open(path, "w");
  if(!file){
    Serial.println("failed to open file for writing");
    return;
  } if(file.print(message)){
      Serial.println("file written");
  } else {
    Serial.println("frite failed");
  }
}