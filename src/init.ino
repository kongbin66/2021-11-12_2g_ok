/*----------------------------------------------------------
                 軟、硬件初始化操作相关
*---------------------------------------------------------*/
#include "config.h"
//硬件初始化
/*
  1.800c电源引脚.LED引脚
  2.串口初始化（MOn&AT）
  3.i2c sht20 eeprom spiffs display key ds1302
*/
void f_hardware_init()
{
  int i = 0;

  //IO设定
  pinMode(LED, OUTPUT); //LED引脚
  digitalWrite(LED, HIGH);
  pinMode(type_connect39, INPUT); //充电线连接

  pinMode(SW_POWER, OUTPUT);      //PMOS控制管脚
  digitalWrite(SW_POWER, LOW);
  
  pinMode(GSM_power, OUTPUT); //
  digitalWrite(GSM_power, LOW);
 
 
  // pinMode(KEY1w,INPUT_PULLUP);
  // pinMode(KEY2p,INPUT_PULLUP);

  //串口设定
  SerialMon.begin(115200);   
   Serial.printf("init_start\n");                             //初始化调试串口
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX); //初始化AT串口
  //按键
  key_init(true);

  //i2c
  Wire.begin(); //I2C设置
  #ifdef _SHT20
  sht20.begin();
  #endif
  ip5306.begin(); //电源芯片设定
  ip5306.setup();
  ip5306.setPowerBoostKeepOn(1); //保持升压输出

  display.init(); //OLED
  display.flipScreenVertically();

  //DS1302
  ds_rtc.init();
  if (ds_rtc.isHalted())
    ds_rtc.setDateTime(&now1); //检查运行DS1302 //数值不对，只是让它运行

  //adc
  set_ADC();
  //eeprom
  EEPROM.begin(4096);         //E2PROM初始化
  get_eeprom_firstBootFlag(); //获取EEPROM第1位,判断是否是初次开机
  eeprom_config_init();       //初始化EEPROM

  //spiffs
  Serial.printf("SPIFFS.begin=%d\n", SPIFFS.begin());
  alFFS_init(); //初始化FFS
  Serial.printf("init_over\n");
}
/*----------------------------------------------------------
                 软件初始化操作相关
*---------------------------------------------------------*/
void f_software_init() //軟件初始化
{
  loopStartTime = millis();
  tempAndHumi_Ready = false;
  screen_loopEnabled = true;

  //下面是固定参数，需要修改时再保存到EEPROM中
  P_show_auto_return_time = 3;
  P_show_On_Start_time = 0; //sys_sec;
  P_show_now_time = sys_sec;

  LAC = 0, CID = 0, MCC = 460, MNC = 0, LAC_FLAG = 10;
  keyState = NOKEYDOWN;
  screenState = MAIN_TEMP_SCREEN;
  bleState = BLE_OFF;
  lockState = UNLOCKED;
  qualifiedState = false;
}
//判断唤醒方式
void f_if_wakeup_mode()
{
  //输出工作状态和唤醒方式

  Serial.printf("workingState:%d\r\n", workingState);
  Serial.printf("oledState:%d\r\n", oledState);
  Serial.print("esp_sleep_get_wakeup_cause:");
  Serial.println(esp_sleep_get_wakeup_cause());

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) //如果是加电和复位唤醒的
  {
    //系统复位后，停止工作，
    workingState = NOT_WORKING; //停止工作, 亮屏
    //oledState = OLED_WORK;
    Serial.println("jiadian_fuwei_ESP_SLEEP_WAKEUP_UNDEFINED,clear spiffs\n");

    //清所有数据
    screen_loopEnabled = true;
    list_first_flag = true;
    lose_first_flag = true;
    postMsgId = 0; //清记录条数
    lose_count = 0;
    deleteFile(SPIFFS, "/list.csv");
    deleteFile(SPIFFS, "/lose.csv");
    
    //重新读取时间信息
    ds_rtc.getDateTime(&now1);
    Serial.printf("time now1: %d-%d-%d %d:%d:%d\r\n", now1.year, now1.month, now1.day, now1.hour, now1.minute, now1.second);
    P_sleep_last_time = now_unixtime; //刷新最后发送时间
    eeprom_config_save_parameter();   //记录参数
   
  }
  else if (oledState == OLED_OFF && workingState == WORKING && (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER)) //不是开机，是定时唤醒。
  {
    Serial.println("dingshihuanxing_ESP_SLEEP_WAKEUP_UNDEFINED\n");
    f_800c_power_switch(true);
    send_Msg_var_GSM_while_OLED_on();
    gpio_hold_dis(GPIO_NUM_23); //解锁电源引脚
    gpio_deep_sleep_hold_dis(); //禁止深度睡眠中引脚锁定
    while (f_MQTT_ON)
    {
      client.loop();
    }
    go_sleep_a_while_with_ext0(); //休眠
  }
  else if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) // && workingState == WORKING)//按键唤醒
  {
    Serial.println("anjian_ESP_SLEEP_WAKEUP_EXT0\n");
   // oledState = OLED_WORK;
  }
}