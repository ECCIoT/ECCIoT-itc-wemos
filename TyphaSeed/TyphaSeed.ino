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



ESP8266WebServer server(80);
TsLed led(PIN_LED);
TsButton btn(PIN_D8,HIGH);

const String CONFIG_FILE_NAME = "/config.json";
JsonConfig jc(CONFIG_FILE_NAME, jc_ErrorCallback);

void jc_ErrorCallback(uint8_t error_code, String& error_msg){
	Serial.printf("JsonConfig Error : (#%d) %s", error_code, error_msg);
}


void(*resetFunc) (void) = 0; //����

void btn_OnKeyDown(){
	led.reverse();
}

//Settingҳ�����
void SettingPageWebHander(){

	//Ϊ���������һ��վ��
	server.on("/setting", [](){

		//����2������ʱ����������
		if (server.args() == 2){
			server.sendContent("<html><body>");
			server.sendContent(server.arg(0) + "<br/>" + server.arg(1));
			server.sendContent("</body></html>");

			//��������
			delay(500);
			resetFunc();      
		}
		//��û�в�������������������������ʾ����ҳ�棬���������Ǳ���
		else{
			//����page�Ŀ�ʼ��ǩ
			server.sendContent("<html><head><title>Setting</title></head><body><div align=\"center\">");

			//��պ����ֲ�����������lambda�������ã�����λ�ڲ����б��У�
			//String htmlSelectTag = getHtmlSelectByWifiAccessPoints();

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

	/*
	btn.addEvent([]{
		led.reverse();
		Serial.println("LED���ŵ�λ�ѷ�ת.");
	});
	*/

	btn.onKeyDown = btn_OnKeyDown;
	//WiFi.softAP("WeMos D1", "12345678");
	//Serial.println("Server started");
	//Serial.println("http://192.168.4.1/setting");
	//SettingPageWebHander();
}

void loop() {
	btn.updateState();
	delay(50);
}
