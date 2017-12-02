/*
 Name:		TyphaSeed.ino
 Created:	11/26/2017 11:36:44 AM
 Author:	Landriesnidis
*/

#include "JsonConfig.h"

#include "TsWaterSensor.h"
#include "TsLed.h"
#include "TsButton.h"
#include "pins_wemos_d1.h"

#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"

#define STR_WIFI_SSID "WIFI_SSID"
#define STR_WIFI_PSWD "WIFI_PSWD"

#define STR_AP_SSID "WeMos D1"
#define STR_AP_PSWD "12345678"

ESP8266WebServer server(80);
TsLed led(PIN_LED);
TsButton btn(PIN_D8,HIGH);

const String CONFIG_FILE_NAME = "/config.json";

//����JsonConfig
/*
bool js_errorCallback(uint8_t error_code, String& error_msg){
	Serial.printf("JsonConfig Error : (#%d) ", error_code);
	Serial.println(error_msg);
	return false;
}
JsonConfig jc(CONFIG_FILE_NAME, js_errorCallback);
*/
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

//Settingҳ��
void SettingsPageWebHander(){

	//Ϊ���������һ��վ��
	server.on("/settings", [jc](){

		//����2������ʱ����������
		if (server.args() == 2){

			//��WIFI���˺ź����뱣�浽�����ļ�
			JsonObject& config = jc.getConfigJson();
			config["DEBUG"] = true;
			config[STR_WIFI_SSID] = server.arg(0);
			config[STR_WIFI_PSWD] = server.arg(1);
			jc.saveConfig(config);

			//��ʾ���������
			Serial.print("Contents of the ConfigFile :");
			config.printTo(Serial);
			Serial.println();

			//��������
			Serial.println("Restart the board after 5 seconds...");
			delay(5000); 
			server.close();
			reset();      
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
	server.begin();
	while (1){
		server.handleClient();
	}
}

void setup() {
	Serial.begin(115200);
	Serial.println("Start");
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

	/*
	 ���������ļ��Ƿ����ȷ����ǰ�Ƿ�Ϊ��ʼ״̬����Ҫ����WIFI�����ƺ����룩
	*/
	//�������ļ�����
	if (!jc.isExist()){
		//����WIFI�ȵ�
		WiFi.softAP(STR_AP_SSID, STR_AP_PSWD);

		//����Webҳ�棺Settings
		Serial.println("No configuration files were found, please access SETTING page.");
		Serial.println("URL:http://192.168.4.1/settings");
		SettingsPageWebHander();
	}
	//�������ļ�����
	else{
		JsonObject& config = jc.getConfigJson();
		String ssid = config[STR_WIFI_SSID];
		String pswd = config[STR_WIFI_PSWD];
		
		//��ʾ���������
		Serial.print("Contents of the ConfigFile :");
		config.printTo(Serial);
		Serial.println();

		//����WIFI
		WiFi.begin(ssid.c_str(),pswd.c_str());

		//�ȴ�WIFI����
		int timmer = 0;
		bool bConnected = false;
		Serial.print("WIFI connecting");

		//ѭ�����WIFI������״̬
		while (true) {
			delay(500);
			//�ȴ�10��
			if (timmer++ > 20 && !bConnected){
				Serial.println("WIFI connect timeout!");
				reset();
				break;
			}
			Serial.print(".");

			//�ж�WIFI��ǰ״̬
			switch (WiFi.status()){

			//������ʧ�ܻ���ָ��SSID���ź�
			case WL_CONNECT_FAILED || WL_NO_SSID_AVAIL:	
				jc.deleteConfig();
				Serial.println("WIFI ssid(or password) error, config file has been deleted.");
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
		
		/*�������Ϳ��Կ�ʼTCPͨ���ˣ�*/

	}
}

void loop() {
	btn.updateState();
	delay(50);
	led.reverse();
}
