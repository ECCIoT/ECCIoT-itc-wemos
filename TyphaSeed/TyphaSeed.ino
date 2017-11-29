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


void(*resetFunc) (void) = 0; //重启

void btn_OnKeyDown(){
	led.reverse();
}

//Setting页面程序
void SettingPageWebHander(){

	//为服务器添加一个站点
	server.on("/setting", [](){

		//当有2个参数时，保存数据
		if (server.args() == 2){
			server.sendContent("<html><body>");
			server.sendContent(server.arg(0) + "<br/>" + server.arg(1));
			server.sendContent("</body></html>");

			//重启程序
			delay(500);
			resetFunc();      
		}
		//当没有参数（或其他参数个数），显示配置页面，但函数不是变量
		else{
			//设置page的开始标签
			server.sendContent("<html><head><title>Setting</title></head><body><div align=\"center\">");

			//封闭函数局部变量不能在lambda体中引用，除非位于捕获列表中，
			//String htmlSelectTag = getHtmlSelectByWifiAccessPoints();

			//扫描WIFI信号
			int n = WiFi.scanNetworks();

			//如果搜索到WiFi信号
			if (n > 0){
				//表单开始

				server.sendContent("<form>");
				//WIFI SSID下拉选择框
				server.sendContent("<h2>WIFI SSID:</h2><select name=\"ssid\">");
				for (int i = 0; i < n; ++i)
				{
					server.sendContent("<option value = \"" + WiFi.SSID(i) + "\">" + WiFi.SSID(i) + "</option>");
					delay(5);
				}
				server.sendContent("</select>");
				//密码输入栏
				server.sendContent("<br/><h2>Password:</h2><input type=\"password\" name=\"psw\"><br/>");
				//Submit按钮
				server.sendContent("<br/><input type=\"submit\" value=\"Submit\">");
				//表单结束
				server.sendContent("</form>");
			}
			//若附近没有搜索到wifi信号
			else{
				server.sendContent("<h1>No Wifi Signal was found.</h1>");
			}

			//设置page的结束标签
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
		Serial.println("LED引脚电位已反转.");
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
