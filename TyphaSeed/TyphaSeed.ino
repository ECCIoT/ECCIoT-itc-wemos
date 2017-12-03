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


/*************************************************************************************/

//当前的版本信息
#define _VERSION "TyphaSeed v0.9.1 beta"

//配置文件中WiFi的SSID和Password字段名称
#define STR_WIFI_SSID "WIFI_SSID"
#define STR_WIFI_PSWD "WIFI_PSWD"

//创建热点的SSID和Password
#define STR_AP_SSID "WeMos D1"
#define STR_AP_PSWD "12345678"

/*************************************************************************************/


ESP8266WebServer server(80);
TsLed led(PIN_LED);
TsButton btn(PIN_D8,HIGH);

const String CONFIG_FILE_NAME = "/config.json";

//创建JsonConfig
JsonConfig jc(CONFIG_FILE_NAME,
	[](uint8_t error_code, String& error_msg)->bool{
		Serial.printf("JsonConfig Error : (#%d) ", error_code);
		Serial.println(error_msg);
		return false;
	}
);

//重启
void(*reset) (void) = 0; 

void btn_OnKeyDown(){
	led.reverse();
}

//创建Web页面Settings,等待(阻塞)完成WiFi配置
void createSettingWebPage(){

	Serial.println("No configuration files were found, please access SETTING page.");
	Serial.println("URL:http://192.168.4.1/settings");

	//启动WIFI热点
	WiFi.softAP(STR_AP_SSID, STR_AP_PSWD);

	//为服务器添加settings页面
	server.on("/settings", [](){

		//当有2个参数时，保存数据
		if (server.args() == 2){

			Serial.println("\n\nReceived configuration arguments!");

			//将WIFI的账号和密码保存到配置文件
			JsonObject& config = jc.getConfigJson();
			config["DEBUG"] = true;
			config[STR_WIFI_SSID] = server.arg(0);
			config[STR_WIFI_PSWD] = server.arg(1);
			jc.saveConfig(config);

			//显示保存的内容
			Serial.print("\nContents:");
			config.printTo(Serial);
			Serial.println();

			//关闭Web服务器
			Serial.println("WiFi configuration complete!");
			server.close();

			//关闭热点
			WiFi.softAPdisconnect();

			return;
		}
		//当没有参数（或其他参数个数），显示配置页面，但函数不是变量
		else{
			//设置page的开始标签
			server.sendContent("<html><head><title>Setting</title></head><body><div align=\"center\">");

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

	//启动Web服务器
	server.begin();

	//循环接收客户端的页面请求
	int i = 0;
	while (jc.isExist() == false){
		if (i++ % 20 == 0){
			Serial.print("\nPending for finished the configuration");
			i = 1;
		}

		//处理页面请求
		server.handleClient();
		Serial.print(".");
		delay(500);
	}
}

//连接WiFi
void connectWiFi(){
	/*
	根据配置文件是否存在确定当前是否为初始状态（需要配置WIFI的名称和密码）
	*/

	//若配置文件不存在
	if (!jc.isExist()){
		//创建Web页面Settings,等待(阻塞)完成WiFi配置
		createSettingWebPage();
	}

	//若配置文件存在
	JsonObject& config = jc.getConfigJson();
	String ssid = config[STR_WIFI_SSID];
	String pswd = config[STR_WIFI_PSWD];

	//显示保存的内容
	Serial.print("Contents of the ConfigFile :");
	config.printTo(Serial);
	Serial.println();

	//连接WIFI
	WiFi.begin(ssid.c_str(), pswd.c_str());

	//等待WIFI连接
	int timmer = 0;
	int intDelayTime = 500;
	bool bConnected = false;
	Serial.print("WIFI connecting");

	//循环检测WIFI的连接状态
	while (true) {
		delay(intDelayTime);
		led.reverse();
		Serial.print(".");

		//判断WIFI当前状态
		switch (WiFi.status()){

		//若连接失败
		case WL_CONNECT_FAILED:
			//删除配置文件
			jc.deleteConfig();
			Serial.println("\nWIFI ssid(or password) error, config file has been deleted.");
			//创建Web页面Settings,等待(阻塞)完成WiFi配置
			createSettingWebPage();
			break;

		//若找不到无指定SSID的信号
		case WL_NO_SSID_AVAIL:
			//延长WiFi信号扫描的间隔时间
			intDelayTime = 5000;
			break;

		//若WIFI连接成功
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

	//与WiFi建立连接
	connectWiFi();

	/*接下来就可以开始TCP通信了！*/
	Serial.println("OK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
}

void loop() {
	btn.updateState();
	delay(50);
}
