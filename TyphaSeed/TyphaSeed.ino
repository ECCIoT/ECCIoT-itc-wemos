/*
 Name:		TyphaSeed.ino
 Created:	11/26/2017 11:36:44 AM
 Author:	Landriesnidis
*/

#include "stdlib.h"

#include "ATCommand.h"
#include "JsonConfig.h"

#include "TsComponent.h"
#include "pins_wemos_d1.h"

#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"

/*************************************************************************************/

//��ǰ�İ汾��Ϣ
#define _VERSION "TyphaSeed v0.9.5 beta"

//�����ļ���WiFi��SSID��Password�ֶ�����
#define STR_WIFI_SSID "WIFI_SSID"
#define STR_WIFI_PSWD "WIFI_PSWD"

//�����ȵ��SSID��Password
#define STR_AP_SSID "WeMos D1"
#define STR_AP_PSWD "12345678"

//�����ļ�·��
#define CONFIG_FILE_NAME "/config.json"

//�豸Ψһ��ʶ
#define SECRET_KEY "6db19e03"

/*************************************************************************************/

//Web���׷�����
ESP8266WebServer server(80);
//WIFI���ӹ�����
WiFiClient client;
//ATָ�
ATCommand atc;


//TsLed led0(PIN_D0);
//TsLed led1(PIN_D1);
//TsLed led2(PIN_D2);
//TsLed led3(PIN_D3_D15);
//TsLed led4(PIN_D4_D14);
//TsLed led5(PIN_D5_D13);
//TsLed led6(PIN_D6_D12);
//TsLed led7(PIN_D7_D11);
//
//TsLed arrLED[8] = { led0, led1, led2, led3, led4, led5, led6, led7 };

TsLed led(PIN_D9_LED);

TsLed relay1(PIN_D3_D15);
TsLed relay2(PIN_D4_D14);
TsLed relay[2] = { relay1, relay2};

TsButton btn(PIN_D8,HIGH);


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

//��ʼ��ATָ�
void initATCommands(){
	CommandItem cmdTest("TEST", [](CommandParameter param)->String{
		Serial.printf("parameter count : %d\n", param.count());
		for (int i = 0; i < param.count(); i++){
			printf("arg%d=%s\n", i, param.get(i).c_str());
		}
		return "OK";
	});
	CommandItem cmdVersion("VERSION", [](CommandParameter param)->String{
		return _VERSION;
	});
	CommandItem cmdKey("KEY", [](CommandParameter param)->String{
		return SECRET_KEY;
	});
	CommandItem cmdInitConfig("INITCONFIG", [](CommandParameter param)->String{
		jc.deleteConfig();
		return "OK";
	});
	CommandItem cmdConfig("CONFIG", [](CommandParameter param)->String{
		JsonObject& config = jc.getConfigJson();
		config.printTo(Serial);
		Serial.println();
		return "OK";
	});
	CommandItem cmdServer("SERVER", [](CommandParameter param)->String{
		//����������Ϊ0ʱ��ʾ��ǰ�����ã������²���д������
		JsonObject& config = jc.getConfigJson();
		if (param.count() == 0){
			String addr = config["SERVER_ADDR"];
			Serial.println(addr);
			return "OK";
		}
		else if (param.count() == 2){
			config["SERVER_ADDR"] = param.get(0).c_str();
			config["SERVER_PORT"] = param.get(0).c_str();
			jc.saveConfig(config);
			return "OK";
		}
		else{
			return "ERROR";
		}
	});
	CommandItem cmdRelay("RELAY", [](CommandParameter param)->String{
		if (param.count() == 2){
			int pin, value;
			pin = atoi(param.get(0).c_str());
			value = atoi(param.get(1).c_str());
			Serial.printf("Relay #%d -> %d\n",pin,value);
			relay[pin].setState(value == 1 ? true : false);
			return "OK";
		}
		else{
			return "ERROR";
		}
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

	//CommandItem cmdBlink("BLINK", [](CommandParameter param)->String{
	//	if (param.count() != 0){
	//		return "ERROR";
	//	}
	//	int i;
	//	for (i = 0; i < 8;++i){
	//		Serial.printf("Blink -> D%d\n",i);
	//		int j = 20;
	//		while (j>0){
	//			arrLED[i].reverse();
	//			delay(200);
	//			j--;
	//		}
	//	}

	//	return "OK";
	//});

	atc.addCommandItem(cmdTest);
	atc.addCommandItem(cmdKey);
	atc.addCommandItem(cmdVersion);
	atc.addCommandItem(cmdConfig); 
	atc.addCommandItem(cmdInitConfig);
	atc.addCommandItem(cmdServer); 
	atc.addCommandItem(cmdRelay);
	atc.addCommandItem(cmdLED);
	//atc.addCommandItem(cmdBlink);
}

//���µ�Ԫ״̬
void updateComponentState(){
	btn.updateState();
}

//�������Դ��ڵ�����
void receiveDataFromSerial(){
	static String temp_s = "";
	char temp_c;

	if (!Serial)
		return;
	while (Serial.available() > 0)
	{
		temp_c = char(Serial.read());													//���ֽڶ�ȡ��������
		if (temp_c == '\r') {															//�ж��Ƿ�Ϊ��ֹ��
			//Serial.printf("Received AT command from the Serial : %s", temp_s.c_str());
			//Serial.printf("%s\n", temp_s.c_str());
			Serial.println(atc.parse(temp_s).c_str());
			temp_s = "";
		}
		else {
			temp_s += temp_c;
		}
		delay(2);
	}
	
}

//��������TCP������
void receiveDataFromTCP(){
	while (true){
		String line = client.readStringUntil('\n');
		if (line.length() > 0){
			Serial.printf("%s\n", line.c_str());
			client.print(atc.parse(line).c_str());
		}
		else{
			break;
		}
	}
}

void setup() {
	//���ô��ڲ�����
	Serial.begin(115200);
	//��������ʾ������ʶ
	Serial.println("\nStart");
	//�رհ���LEDָʾ��
	led.setState(true);
	//Ϊ��ԭ����ӵ����¼�����
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
	//String server_addr = jc.getConfigJson()["SERVER_ADDR"];
	//int server_port = jc.getConfigJson()["SERVER_PORT"];
	if (!client.connect("120.25.222.235", 60000)){
		Serial.println("Failed to connect to the server.");
		return;
	}
	
	//client.println("Start");

	/*while (1){
		relay1.reverse();
		delay(1000);
	}*/
}

void loop() {
	/*һ�������������״̬*/
	//�����ʧWIFI����
	if (WiFi.status() == WL_CONNECTION_LOST){
		connectWiFi();
	}

	/*�������µ�Ԫ״̬���ⲿ�������ʵ�ֶ��߳̾ͺ�������*/
	updateComponentState();

	/*�������մ�����Ϣ*/
	if (Serial.available()){
		receiveDataFromSerial();
	}

	/*�ġ�����TCP��Ϣ*/
	if (client.available()){
		client.status();
		receiveDataFromTCP();
	}
	
}