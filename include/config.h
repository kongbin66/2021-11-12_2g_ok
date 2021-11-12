#if !defined(CONFIG_H)
#define CONFIG_H
//设置
#define TINY_GSM_MODEM_SIM800 //引入TinyGSM库. 在引入之前要定义好TINY_GSM_MODEM_SIM800,让它知道我们用的模块型号
#define ds18b20
//#define _SHT20
#define DUMP_AT_COMMANDS //AT指令监控

#define Reconnection 3 //设定GPRS重连次数

/***************************头文件******************************************/
#include "Arduino.h"
#include "SPIFFS.h"
#include "IP5306.h"
#include "WiFi.h"
#include <esp_sleep.h>

#include "Wire.h"
#include "uFire_SHT20.h"
#include <TinyGsmClient.h>
#include "PubSubClient.h"

#include "OneWire.h"
#include "EEPROM.h"
#include "SH1106Wire.h"
#include "images.h"
#include "OneButton.h"
#include <Ds1302.h>
#include "xieyi.h"
#include "ArduinoJson.h"



/***********************************************************************************
                                 管脚定义：
***********************************************************************************/

#define LED 23//LED管脚低电平亮灯
#define type_connect39 39 //充电检测管脚不是ADC
#define SW_POWER 15 //高电平使能 电池供电开机使能
#define ip5306_key 15  //唤醒iP5306
#define GSM_power 14
//iic
#define PIN_I2C_SDA 21 
#define PIN_I2C_SCL 22
//ds1302驱动引脚
#define PIN_ENA 19
#define PIN_CLK 5
#define PIN_DAT 18
//KEY
#define KEY1w      33           //按键1对应引脚
#define WEAKUPKEY1w GPIO_NUM_33 //按键1对应引脚
#define KEY2p      25    //keypin
#define WEAKUPKEY2p GPIO_NUM_25 //按键1对应引脚
#define ds1 4 //18b20引脚
//ADC/
static const int adcpin36 = 36;//选择adc1
//SIM800L 硬件定义
#define MODEM_TX 27       //SIM800L串口TX引脚接在GPIO27
#define MODEM_RX 26       //SIM800L串口RX引脚接在GPIO26
//其他硬件定义//
#define SerialMon Serial      //调试串口为UART0
#define SerialAT  Serial1      //AT串口为UART1
uint32_t test_count = 0;//获取位置失败计数器


//对象
IP5306 ip5306;
SH1106Wire display(0x3c, 21, 22);
#ifdef _SHT20
uFire_SHT20 sht20;
#else
OneWire ds(ds1);  // on pin 10 (a 4.7K resistor is necessary)
#endif

#ifdef DUMP_AT_COMMANDS //串口映射
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

TinyGsmClient gsmclient(modem);// 创建一个GSM型的网络客户端
PubSubClient client(gsmclient);
DynamicJsonDocument doc(512);
//OneWire ds(ds1);  // on pin 10 (a 4.7K resistor is necessary)

OneButton button(KEY1w, true,true);//功能键
OneButton button2(KEY2p, true,true);//电源键
Ds1302 ds_rtc(PIN_ENA, PIN_CLK, PIN_DAT);//创建DS1302对象
Ds1302::DateTime now1;//ds1302读取的时间


//标志
uint8_t dbug =0;//调试开关
int rollback = 0;
//变量
/*————————————————————————公共变量————————————————————————————*/
TaskHandle_t task1; //第二核创建一个任务句柄
TaskHandle_t ds_task;
TaskHandle_t xieyi_task;

//出厂设置定义
#define FACTORY_SLEEPTIME   30     // 300000000    //休眠时间        只适用一次
#define FACTORY_TEMP_LIMIT_ENABLE 0    //出厂温度上下限失能
#define FACTORY_TEMP_UPPER_LIMIT 50.0  //出厂温度上限
#define FACTORY_TEMP_LOWER_LIMIT -40.0 //出厂温度下限
#define FACTORY_DATE_YEAR 1970         //出厂默认时间
#define FACTORY_DATE_MONTH 1           //出厂默认时间
#define FACTORY_DATE_DAY 1             //出厂默认时间
#define FACTORY_TIME_HOUR 0            //出厂默认时间
#define FACTORY_TIME_MIN 0             //出厂默认时间
//系统时间定义/
RTC_DATA_ATTR uint32_t now_unixtime;//现在系统时间 *
RTC_DATA_ATTR unsigned long start_time;//起始记录时间
RTC_DATA_ATTR unsigned long last_time;//最后记录时间

time_t time_last_async_stamp;//上一次的时间戳
time_t sys_sec=0;
/*-------------------------------公共变量,参数定义-------------------------------------*/
//温湿度采集相关
float currentTemp;
float currentHumi;
//F_温湿度读取标志
bool tempAndHumi_Ready;
bool timeNTPdone;
bool F_alarm_status;//报警状态
//判断是否第一次启动

int t_keylongpress_now;//长按
int t_keylongpress_start; //长按开始


#define BLE_ON 1
#define BLE_OFF 2
#define LOCKED 1
#define UNLOCKED 2
#define QUALITIFY_RIGHT 1
#define QUALITIFY_WRONG 2

RTC_DATA_ATTR int bleState;       //蓝牙状态机
RTC_DATA_ATTR int lockState;      //开关箱状态机
RTC_DATA_ATTR int qualifiedState; //合格状态机

/*-------------------------------公共变量,参数定义-------------------------------------*/
//以下参数需要休眠RTC记忆
RTC_DATA_ATTR bool  temp_alarm;      // tempLimit_enable;    //温度上下限报警开关
RTC_DATA_ATTR float tempU=50;//tempUpperLimit;                  //温度上限设定
RTC_DATA_ATTR float tempL=-100;//tempLowerLimit;                  //温度下限设定
RTC_DATA_ATTR long tempUA=0;                        //温度上限报警计数
RTC_DATA_ATTR long tempLA=0;                        //温度下限报警计数
RTC_DATA_ATTR time_t period;                      //休眠时间
RTC_DATA_ATTR time_t sleep_start_time;               //休眠开始时间
RTC_DATA_ATTR time_t sleep_end_time;                 //休眠结束时间
RTC_DATA_ATTR time_t sleep_time_count;               //休眠时长时间
RTC_DATA_ATTR int postMsgId = 0;                     //记录已经post了多少条
RTC_DATA_ATTR int32_t  LAC,CID,MCC=460,MNC=0,LAC_FLAG=10; //位置大区号，小区号
RTC_DATA_ATTR int timeNow_Y, timeNow_M, timeNow_D, timeNow_h, timeNow_m, timeNow_s;
// 创建一个关联到SerialAT的SIM800L模型
/*-------------------设备码-----------------------------*/
#if 0
const char *mqtt_server = "218.201.45.7"; //onenet 的 IP地址
const int port = 1883;                     //端口号
#define mqtt_devid "al_kh00001_zx_0001"         //设备ID
#define mqtt_pubid "4LwKzUwOpX"                //产品ID
//鉴权信息
#define mqtt_password "version=2018-10-31&res=products%2F4LwKzUwOpX%2Fdevices%2Fal_kh00001_zx_0001&et=4092599349&method=md5&sign=xpaXrOTMJ9WJjOVolwJhWw%3D%3D"
#endif

#if 0
const char *mqtt_server = "218.201.45.7"; //onenet 的 IP地址
const int port = 1883;                     //端口号
#define mqtt_devid "al_kh00001_zx_0002"         //设备ID
#define mqtt_pubid "4LwKzUwOpX"                //产品ID
//鉴权信息
#define mqtt_password "version=2018-10-31&res=products%2F4LwKzUwOpX%2Fdevices%2Fal_kh00001_zx_0002&et=4092599349&method=md5&sign=FxSayE%2BpBzK9L1YgXt8rxA%3D%3D"
#endif

#if 0
const char *mqtt_server ="218.201.45.7";// "183.230.102.116"; //onenet 的 IP地址
const int port = 1883;                     //端口号
#define mqtt_devid "al_kh00001_zx_0003"         //设备ID
#define mqtt_pubid "4LwKzUwOpX"                //产品ID
//鉴权信息
#define mqtt_password "version=2018-10-31&res=products%2F4LwKzUwOpX%2Fdevices%2Fal_kh00001_zx_0003&et=4092599349&method=md5&sign=RJjI9dBTNLUXL9rk9zbBtQ%3D%3D"
#endif

#if 1
const char *mqtt_server ="218.201.45.7";// "183.230.102.116"; //onenet 的 IP地址
const int port = 1883;                     //端口号
#define mqtt_devid "al_kh00001_zx_0004"         //设备ID
#define mqtt_pubid "4LwKzUwOpX"                //产品ID
//鉴权信息
#define mqtt_password "version=2018-10-31&res=products%2F4LwKzUwOpX%2Fdevices%2Fal_kh00001_zx_0004&et=1754727523&method=md5&sign=pxhDBzUvAWxDB8r%2FV9fwlg%3D%3D"
#endif

/*-------------------------------云平台相关定义-------------------------------------*/
//设备上传数据的post主题
#define ONENET_TOPIC_PROP_POST "$sys/" mqtt_pubid "/" mqtt_devid "/thing/property/post"
//接收下发属性设置主题
#define ONENET_TOPIC_PROP_SET "$sys/" mqtt_pubid "/" mqtt_devid "/thing/property/set"
//接收下发属性设置成功的回复主题
#define ONENET_TOPIC_PROP_SET_REPLY "$sys/" mqtt_pubid "/" mqtt_devid "/thing/property/set_reply"

//接收设备属性获取命令主题
#define ONENET_TOPIC_PROP_GET "$sys/" mqtt_pubid "/" mqtt_devid "/thing/property/get"
//接收设备属性获取命令成功的回复主题
#define ONENET_TOPIC_PROP_GET_REPLY "$sys/" mqtt_pubid "/" mqtt_devid "/thing/property/get_reply"
//设备期望值获取请求主题
#define ONENET_TOPIC_DESIRED_GET "$sys/" mqtt_pubid "/" mqtt_devid "/thing/property/desired/get"
//设备期望值获取响应主题
#define ONENET_TOPIC_DESIRED_GET_RE "$sys/" mqtt_pubid "/" mqtt_devid "/thing/property/desired/get/reply"

//设备期望值删除请求主题
#define ONENET_TOPIC_DESIRED_DEL "$sys/" mqtt_pubid "/" mqtt_devid "/thing/property/desired/delete"
//设备期望值删除响应主题
#define ONENET_TOPIC_DESIRED_DEL_RE "$sys/" mqtt_pubid "/" mqtt_devid "/thing/property/desired/delete/reply"
//这是post上传数据使用的模板
#define ONENET_POST_BODY_FORMAT "{\"id\":\"1\",\"params\":%s}"

#define mrtt_topical "$sys/%s/%s/cmd/request/#"//订阅主题
//#define mrtt_topical      "$sys/%s/%s/thing/property/post"
#define mrtt_topical_reply "$sys/%s/%s/thing/property/post/reply"





//声明
/***********************************************************************************
             初始化相关init.ino
***********************************************************************************/
void f_hardware_init();//硬件初始化
void f_software_init();//软件初始化
void f_if_wakeup_mode();////判断唤醒方式

/***********************************************************************************
         SIM800相关network.ino
***********************************************************************************/
void setupModem();
bool modemToGPRS();
bool getLBSLocation();
void f_800c_power_switch(int a);//800c供电





















/***********************************************************************************
          ADC相关
***********************************************************************************/
uint32_t ADC_Sampling_interval=0;//ADC采样间隔 
float bat_voltage;
bool P_BAT_FULL=0;//电池充满标志

void set_ADC(void);//设置ADC
void  BAT_ADC_Sampling();//电量监测
void f_power_management();//电源管理


/***********************************************************************************
                     sensor.ino  18b20相关
***********************************************************************************/
 
 
/*******************************************sht20.ino*********************/
/***********获取温湿度************
 当温度在一个合理范围内再更新
 温湿度寄存器。
 条件：(_currentTemp < 100)&&(_currentTemp > -40)满足
 更新：currentTemp，currentHumi
      tempAndHumi_Ready
 ********************************/
void sht20getTempAndHumi();

float f_get18b20Temp();//获取18b20温度



/********************电量采集相关*************************************/





#define IP5306_ADDR 0x75
#define IP5306_REG_SYS_CTL0 0x00
#define Power_min_voltage 3.3//设定最小关机电压

#define grade30 3.4
#define grade50 3.5
#define grade80 3.7
#define grade100 4.2

uint8_t POWER_warning_flag;//电压报警标志 0：正常 1：欠压






/**********************************ds1302相关**************************************/












/*********************************显示屏相关函数 al_oled.ino***************************/
//显示/按键相关定义/
//OLED状态
#define OLED_WORK 3//工作
#define OLED_SLEEP 2 //休眠
//#define OLED_ON 1
#define OLED_OFF 0//关机

//工作状态
#define NOT_WORKING 0//停止工作
#define WORKING 1//工作
//显示状态
#define MAIN_TEMP_SCREEN 0 //温度界面
#define MAIN_HUMI_SCREEN 1 //湿度界面
#define TEMP_HUMI_SCROLL_SCREEN 6 //滚屏温度
#define HUMI_TEMP_SCROLL_SCREEN 7 //滚屏湿度


#define TIPS_SCREEN 2  //单击后提示
#define LOGO_SCREEN 4 //开机
#define BLE_SCREEN 5  //蓝牙
#define SETTING_SUCCESS 8 //设定成功
#define REC_START_SCREEN 9 //开始记录
#define REC_STOP_SCREEN 10//停止记录
#define REC_COUNT_SCREEN 11 //计数
#define fxmod_ON 12 //飞行模式开
#define fxmod_OFF 13 //飞行模式关

#define SHOW_1 100
#define SHOW_2 101
#define SHOW_3 102
#define SHOW_4 104
#define SHOW_5 105
#define SHOW_6 106


void showWelcome();
void screen_show();
void screen_loop();
//比较亮屏开始到此时是否满足息屏和休眠条件
void oled_on_off_switch();
void showBFLC_Screen();
const char *p1=F16x16_b100;//电量图标指针
//充电动画
void Battery_Charge();
//显示初始GSM
void show_Initializing_modem1();
//显示连接GSM网络
void show_Waiting_for_network2();
//显示获取位置
void show_getting_LBS3();
//显示连接ONENET
void show_connecting_to_OneNet4();
//显示上传数据
void show_uploading5();
//显示上传完成
void show_full();


//按键状态
#define NOKEYDOWN 0
#define CLICK 1
#define DOUBLECLICK 2
#define LONGPRESS_START 3
#define LONGPRESS_END 4
#define LONGPRESS_DURRING 5
//state of rec_State
#define START_RECING 0
#define END_RECING 1
#define KEEP_RECING 2




RTC_DATA_ATTR int workingState;        //工作状态机
RTC_DATA_ATTR bool screen_loopEnabled; //是否允许滚屏
RTC_DATA_ATTR int oledState;           //OLED工作状态机
RTC_DATA_ATTR int screenState;         //屏幕状态机

RTC_DATA_ATTR int current_rec_State;   //当前记录状态机 (正在开始记录,正在持续记录,正在停止记录)
time_t loopStartTime;                  //计算主屏幕滚屏的起始时间
time_t loopnowTime;                    //计算主屏幕滚屏的当前时间
time_t looptimeSpan;                   //计算滚屏间隔时间



time_t screen_On_last_span;            //设置的亮屏时间
time_t screen_Off_to_sleep_span;       //设置息屏到休眠时间间隔






time_t now_number = 0;
time_t start_number = 0;



time_t P_show_auto_return_time;  //界面自动返回的时间                   
time_t P_show_On_Start_time;                //亮屏时间-起点
time_t P_show_now_time;                     //亮屏时间-现在

time_t P_sleep_last_time;              //上次休眠时间   





int now_state;
 void showStatusBar();





/*-------------------------------SPIFFS定义-------------------------------------*/
RTC_DATA_ATTR bool alFFS_thisRec_firstData_flag; //本次记录第一次上传
RTC_DATA_ATTR char nowREC_filepath[21];          //记录文件的路径
uint32_t lose_count = 0;//漏发文件现在信息条数
bool lose_first_flag=0;//漏发文件第一次标志
char work_data[512];//漏发暂存数组
int work_data_num =0;//漏发工作表数据指针
//读文件列表
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);


//删除文件
void deleteFile(fs::FS &fs, const char * path);











// /*-------------------------------ali_mqtt服务相关ali_mqtt.ino---------------------*/
// void ali_mqtt_connect();
// void ali_callback(char *topic, byte *payload, unsigned int length);
// void ali_sendTemp_Humi_LBS();
/*-------------------------------onenet_mqtts服务相关onenet_mqtts.ino---------------------*/
bool f_MQTT_ON=0;
bool sendTempAndHumi();//发送数据
void set_topcial();
bool connect_gsm();
bool jiexi_lose();
void send_Msg_var_GSM_while_OLED_on();



bool onenet_connect();
/***************获得期望值********************
1.订阅期望值请求和响应主题，设置回应主题回调函数。
2.发送temp_alarm/tempL/tempU/period到期望值
*********************************************/
void getDesired();

/*-------------------------------休眠服务相关al_sleep.ino---------------------*/
void go_sleep_a_while_with_ext0();//进入休眠

/*--------------------------------eeprom相关函数--------------------*/
void eeprom_oledstate_set(int x);
void get_eeprom_firstBootFlag();
void eeprom_config_init();
void eeprom_config_save_parameter(void);
void eeprom_config_set(bool temp_alarm, uint32_t time1, float tempU, float tempL,bool F_alarm_status);
/*********************************SPIFFS相关函数 al_FFS.ino**********/
void alFFS_init();
void alFFS_addRec();







/*********************************按键相关函数 al_key1.ino***********/


RTC_DATA_ATTR int keyState;            //按键状态机
time_t keyScreen_Start;                //计算按键触发的当前屏的起始时间
time_t keyScreen_Now;                  //计算当前屏的当前时间
void key_init(bool a);
void key_loop();



/*******************************************对时相关函数 al_time.ino*/
void wakeup_init_time();//开机更新
time_t unixtime(void) ;//转换时间戳 只有ds1302操作时使用





/***********************************************************时间相关*/

void SET_SLEEPTIME(time_t t);

bool firstBootFlag; //第一次启动标志位
bool list_first_flag=0;//记录文件第一次标志

bool f_Flight_Mode=0;//飞行模式
bool f_lose=0;  //有漏发标志
bool old_workingstate = 0;


uint32_t f_send_ok=1;//漏发上传成功条数




/*******************************************al_csv.ino************************************************/
//上传参数
unsigned long tmsp ;//时间戳
float tp ;//温度
float h ;//湿度
float e ;//经度
float n ;//纬度

/****采集数据打印到savedata****
 采集：温度湿度/经纬度/系统时间戳（飞行模式时位置信息为0）
 输出：格式打印到savedata[48];
******************************/
void getTempAndHumilocation();
/****将savedata记录到文件系统****/
void saveDataToCSV(String file_add);
/***********读取CSV全部数据***********/
void getAllDataFromCSV(String file_add);
void getADataFromCSV(String file_add);//在文件系统中读取并删除第一条
void parseData(long* _tmsp,float* _tp,float* _h,float* _e,float* _n);//解析CSV


void test_alarm(); //检测报警


#endif // CONFIG_H