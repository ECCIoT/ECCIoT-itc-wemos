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

//当前的版本信息
#define _VERSION "TyphaSeed v0.9.5 beta"

//配置文件中WiFi的SSID和Password字段名称
#define STR_WIFI_SSID "WIFI_SSID"
#define STR_WIFI_PSWD "WIFI_PSWD"

//创建热点的SSID和Password
#define STR_AP_SSID "WeMos D1"
#define STR_AP_PSWD "12345678"

//配置文件路径
#define CONFIG_FILE_NAME "/config.json"

//设备唯一标识
#define SECRET_KEY "6db19e03"

/*************************************************************************************/

//Web简易服务器
ESP8266WebServer server(80);
//WIFI连接工具类
WiFiClient client;
//AT指令集
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

			Serial.println("\n\nReceived configuration parameters!");

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
		Serial.print(".");
		server.handleClient();
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
	Serial.println();
	Serial.println("WiFi connection parameters:");
	Serial.println("WIFI_SSID:" + ssid);
	Serial.println("WIFI_PSWD:" + pswd);

	//连接WIFI
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

	//等待WIFI连接
	int timmer = 0;
	int intDelayTime = 500;
	bool bConnected = false;
	Serial.print("WIFI connecting");

	//循环检测WIFI的连接状态
	while (true) {
		timmer++;
		delay(intDelayTime);
		led.reverse();
		Serial.print(".");

		//超时时间为30s
		if (30 < ((intDelayTime / 1000.0)*timmer)){
			Serial.println("\nWIFI connection timeout!");
			break;
		}

		//判断WIFI当前状态
		switch (WiFi.status()){

		//若连接失败
		case WL_CONNECT_FAILED:
			static int failedTimes = 1;
			if (failedTimes <= 5){
				Serial.printf("WIFI connect failed. try again. (%d/5)\n",failedTimes++);
				WiFi.begin(ssid.c_str(), pswd.c_str());
				break;
			}
			failedTimes = 1;
			//删除配置文件
			jc.deleteConfig();
			Serial.println("\nWIFI ssid(or password) error, config file has been deleted.");
			//创建Web页面Settings,等待(阻塞)完成WiFi配置
			createSettingWebPage();
			break;

		//若找不到无指定SSID的信号
		case WL_NO_SSID_AVAIL:
			if (intDelayTime != 5000){
				Serial.print("\nNo SSID(");
				Serial.print(ssid);
				Serial.println(") avail. Change the scan interval to 5 seconds.");
			}
			//延长WiFi信号扫描的间隔时间
			intDelayTime = 5000;
			timmer = 0;
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

//初始化AT指令集
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
		//当参数个数为0时显示当前的配置，否则将新参数写入配置
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

//更新电元状态
void updateComponentState(){
	btn.updateState();
}

//接收来自串口的数据
void receiveDataFromSerial(){
	static String temp_s = "";
	char temp_c;

	if (!Serial)
		return;
	while (Serial.available() > 0)
	{
		temp_c = char(Serial.read());													//单字节读取串口数据
		if (temp_c == '\r') {															//判断是否为终止符
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

//接收来自TCP的数据
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
	//设置串口波特率
	Serial.begin(115200);
	//串口中显示启动标识
	Serial.println("\nStart");
	//关闭板载LED指示灯
	led.setState(true);
	//为还原键添加单击事件代码
	btn.addEvent([]{
		if (jc.isExist()){
			jc.deleteConfig();
			Serial.println("Config file has been deleted.");
		}
		else{
			Serial.println("Config file does not exist.");
		}
	});

	//初始化AT指令集
	initATCommands();

	//与WiFi建立连接
	connectWiFi();

	/*接下来就可以开始TCP通信了！*/
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
	/*一、检测网络连接状态*/
	//如果丢失WIFI连接
	if (WiFi.status() == WL_CONNECTION_LOST){
		connectWiFi();
	}

	/*二、更新电元状态（这部分如果能实现多线程就好啦！）*/
	updateComponentState();

	/*三、接收串口消息*/
	if (Serial.available()){
		receiveDataFromSerial();
	}

	/*四、接收TCP消息*/
	if (client.available()){
		client.status();
		receiveDataFromTCP();
	}
	
}