/*
 Name:		TyphaSeed.ino
 Created:	11/26/2017 11:36:44 AM
 Author:	Landriesnidis
*/

#include "ATCommand.h"
#include "JsonConfig.h"

#include "TsWaterSensor.h"
#include "TsLed.h"
#include "TsButton.h"
#include "pins_wemos_d1.h"

#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"


/*************************************************************************************/

//��ǰ�İ汾��Ϣ
#define _VERSION "TyphaSeed v0.9.1 beta"

//�����ļ���WiFi��SSID��Password�ֶ�����
#define STR_WIFI_SSID "WIFI_SSID"
#define STR_WIFI_PSWD "WIFI_PSWD"

//�����ȵ��SSID��Password
#define STR_AP_SSID "WeMos D1"
#define STR_AP_PSWD "12345678"

/*************************************************************************************/


ESP8266WebServer server(80);
WiFiClient client;

ATCommand atc;

TsLed led(PIN_LED);
TsButton btn(PIN_D8,HIGH);

const String CONFIG_FILE_NAME = "/config.json";


//����JsonConfig
JsonConfig jc(CONFIG_FILE_NAME,
	[](uint8_t error_code, String& error_msg)->bool{
		Serial.printf("JsonConfig Error : (#%d) ", error_code);
		Serial.println(error_msg);
		return false;
	}
);

//����
void(*reset) (void) = 0; 

void btn_OnKeyDown(){
	led.reverse();
}

//����Webҳ��Settings,�ȴ�(����)���WiFi����
void createSettingWebPage(){

	Serial.println("No configuration files were found, please access SETTING page.");
	Serial.println("URL:http://192.168.4.1/settings");

	//����WIFI�ȵ�
	WiFi.softAP(STR_AP_SSID, STR_AP_PSWD);

	//Ϊ���������settingsҳ��
	server.on("/settings", [](){

		//����2������ʱ����������
		if (server.args() == 2){

			Serial.println("\n\nReceived configuration parameters!");

			//��WIFI���˺ź����뱣�浽�����ļ�
			JsonObject& config = jc.getConfigJson();
			config["DEBUG"] = true;
			config[STR_WIFI_SSID] = server.arg(0);
			config[STR_WIFI_PSWD] = server.arg(1);
			jc.saveConfig(config);

			//��ʾ���������
			Serial.print("\nContents:");
			config.printTo(Serial);
			Serial.println();

			//�ر�Web������
			Serial.println("WiFi configuration complete!");
			server.close();

			//�ر��ȵ�
			WiFi.softAPdisconnect();

			return;
		}
		//��û�в�������������������������ʾ����ҳ�棬���������Ǳ���
		else{
			//����page�Ŀ�ʼ��ǩ
			server.sendContent("<html><head><title>Setting</title></head><body><div align=\"center\">");

			//ɨ��WIFI�ź�
			int n = WiFi.scanNetworks();

			//���������WiFi�ź�
			if (n > 0){
				//����ʼ
				server.sendContent("<form>");
				//WIFI SSID����ѡ���
				server.sendContent("<h2>WIFI SSID:</h2><select name=\"ssid\">");
				for (int i = 0; i < n; ++i)
				{
					server.sendContent("<option value = \"" + WiFi.SSID(i) + "\">" + WiFi.SSID(i) + "</option>");
					delay(5);
				}
				server.sendContent("</select>");
				//����������
				server.sendContent("<br/><h2>Password:</h2><input type=\"password\" name=\"psw\"><br/>");
				//Submit��ť
				server.sendContent("<br/><input type=\"submit\" value=\"Submit\">");
				//������
				server.sendContent("</form>");
			}
			//������û��������wifi�ź�
			else{
				server.sendContent("<h1>No Wifi Signal was found.</h1>");
			}

			//����page�Ľ�����ǩ
			server.sendContent("</div></body></html>");
		}
	});

	//����Web������
	server.begin();

	//ѭ�����տͻ��˵�ҳ������
	int i = 0;
	while (jc.isExist() == false){
		if (i++ % 20 == 0){
			Serial.print("\nPending for finished the configuration");
			i = 1;
		}

		//����ҳ������
		Serial.print(".");
		server.handleClient();
		delay(500);
	}
}

//����WiFi
void connectWiFi(){
	/*
	���������ļ��Ƿ����ȷ����ǰ�Ƿ�Ϊ��ʼ״̬����Ҫ����WIFI�����ƺ����룩
	*/

	//�������ļ�������
	if (!jc.isExist()){
		//����Webҳ��Settings,�ȴ�(����)���WiFi����
		createSettingWebPage();
	}

	//�������ļ�����
	JsonObject& config = jc.getConfigJson();
	String ssid = config[STR_WIFI_SSID];
	String pswd = config[STR_WIFI_PSWD];

	//��ʾ���������
	Serial.println();
	Serial.println("WiFi connection parameters:");
	Serial.println("WIFI_SSID:" + ssid);
	Serial.println("WIFI_PSWD:" + pswd);

	//����WIFI
	if (WiFi.isConnected()){
		Serial.print("WIFI has been connected. - SSID:");
		Serial.println(WiFi.SSID());

		if (WiFi.SSID().equals(ssid.c_str())){
			Serial.print("IP Address:");
			Serial.println(WiFi.localIP());
			return;
		}
		else{
			WiFi.disconnect();
			delay(3000);
		}
	}

	WiFi.begin(ssid.c_str(), pswd.c_str());

	//�ȴ�WIFI����
	int timmer = 0;
	int intDelayTime = 500;
	bool bConnected = false;
	Serial.print("WIFI connecting");

	//ѭ�����WIFI������״̬
	while (true) {
		timmer++;
		delay(intDelayTime);
		led.reverse();
		Serial.print(".");

		//��ʱʱ��Ϊ30s
		if (30 < ((intDelayTime / 1000.0)*timmer)){
			Serial.println("\nWIFI connection timeout!");
			break;
		}

		//�ж�WIFI��ǰ״̬
		switch (WiFi.status()){

		//������ʧ��
		case WL_CONNECT_FAILED:
			static int failedTimes = 1;
			if (failedTimes <= 5){
				Serial.printf("WIFI connect failed. try again. (%d/5)\n",failedTimes++);
				WiFi.begin(ssid.c_str(), pswd.c_str());
				break;
			}
			failedTimes = 1;
			//ɾ�������ļ�
			jc.deleteConfig();
			Serial.println("\nWIFI ssid(or password) error, config file has been deleted.");
			//����Webҳ��Settings,�ȴ�(����)���WiFi����
			createSettingWebPage();
			break;

		//���Ҳ�����ָ��SSID���ź�
		case WL_NO_SSID_AVAIL:
			if (intDelayTime != 5000){
				Serial.print("\nNo SSID(");
				Serial.print(ssid);
				Serial.println(") avail. Change the scan interval to 5 seconds.");
			}
			//�ӳ�WiFi�ź�ɨ��ļ��ʱ��
			intDelayTime = 5000;
			timmer = 0;
			break;

		//��WIFI���ӳɹ�
		case WL_CONNECTED:
			bConnected = true;
			Serial.println("\nWIFI connection success!");
			Serial.print("IP Address:");
			Serial.println(WiFi.localIP());
			break;
		}

		if (bConnected)break;
	}
	
}


void initATCommands(){

	CommandItem cmdTest("TEST", [](CommandParameter param)->String{
		Serial.printf("parameter count : %d\n", param.count());
		for (int i = 0; i < param.count(); i++){
			printf("arg%d=%s\n", i, param.get(i).c_str());
		}
		return "OK";
	});

	CommandItem cmdLED("LED", [](CommandParameter param)->String{
		if (param.count() != 1){
			return "ERROR";
		}
		String arg0 = param.get(0);
		if (arg0.equals("-1")){
			led.reverse();
		}
		else if (arg0.equals("0")){
			led.setState(true);
		}
		else if (arg0.equals("1")){
			led.setState(false);
		}
		else{
			return "ERROR";
		}
		
		return "OK";
	});

	

	atc.addCommandItem(cmdTest);
	atc.addCommandItem(cmdLED);
}

void setup() {
	Serial.begin(115200);
	Serial.println("Start");
	//WiFi.disconnect();
	led.setState(false);
	btn.addEvent([]{
		if (jc.isExist()){
			jc.deleteConfig();
			Serial.println("Config file has been deleted.");
		}
		else{
			Serial.println("Config file does not exist.");
		}
	});

	//��ʼ��ATָ�
	initATCommands();

	//��WiFi��������
	connectWiFi();

	

	/*�������Ϳ��Կ�ʼTCPͨ���ˣ�*/
	//Serial.println("OK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	
	if (!client.connect("172.20.73.137",60000)){
		Serial.println("Failed to connect to the server.");
		return;
	}
	client.println("Hello World!");
}

void updateComponentState(){
	btn.updateState();
}

void receiveDataFromSerial(){
	char c;
	String line;
	while (true){
		c = (char)Serial.read();
		if (c != '\n')
			line += c;
		else
			break;
	}
	Serial.printf("Received AT command from the Serial : %s",line.c_str());
	Serial.print(atc.parse(line).c_str());
}

void receiveDataFromTCP(){
	while (true){
		String line = client.readStringUntil('\n');
		if (line.length() > 0){
			client.print(atc.parse(line).c_str());
		}
		else{
			break;
		}
	}
}

void loop() {
	/*һ�������������״̬*/
	//�����ʧWIFI����
	//if (WiFi.status() == WL_CONNECTION_LOST){
	//	connectWiFi();
	//}

	/*�������µ�Ԫ״̬���ⲿ�������ʵ�ֶ��߳̾ͺ�������*/
	updateComponentState();

	/*�������մ�����Ϣ*/
	if (Serial.available()){
		receiveDataFromSerial();
	}

	/*�ġ�����TCP��Ϣ*/
	if (client.available()){
		receiveDataFromTCP();
	}
	
}
