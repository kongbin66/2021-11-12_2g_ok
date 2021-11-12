#include"config.h"
//调试开关
void set_dbug(uint8_t a)
{
  dbug = a;
  Serial.printf("dbug=%d\n", dbug);
}

//设置GSM发送间隔：（S）
void SET_Sending_interval(time_t t)
{
  period = t;
  eeprom_config_save_parameter();
  period = (time_t)EEPROM.readLong(2);
  Serial.printf("period:%ld\r\n", period);
}

//设置亮屏时间和息屏到休眠时间
void SET_Last_span_Sleep_span(int x, int y)
{
  screen_On_last_span = x;
  screen_Off_to_sleep_span = y;
  eeprom_config_save_parameter();
  screen_On_last_span = (time_t)EEPROM.readInt(43);
  Serial.printf("screen_On_last_span:%ld\r\n", screen_On_last_span);
  screen_Off_to_sleep_span = (time_t)EEPROM.readInt(47);
  Serial.printf("screen_Off_to_sleep_span:%ld\r\n", screen_Off_to_sleep_span);
}
//设置实时时钟
void SET_ds1302(int year, char momtch, char day, char hour, char minute, char second)
{
  now1.year = year;
  now1.month = momtch;
  now1.day = day;
  now1.hour = hour;
  now1.minute = minute;
  now1.second = second;
  ds_rtc.setDateTime(&now1);
  now_unixtime = unixtime();
}
//设定飞航模式 1飞航 2工作
void set_fxmode(char a, char b, char c)
{
  f_Flight_Mode = a;
  workingState = b;
  dbug = c;
}



//格式化文件系统
void FFS_fromat()
{
  bool i = SPIFFS.format();
  if (!i)
    Serial.printf("FFS_fromat is fail!\n");
  else
    Serial.printf("FFS_fromat ok!\n");
}
//核对系统时间
void sys_time()
{
  screen_loopEnabled = false;
  setupModem();
  modemToGPRS();

  screen_loopEnabled = true;
}
//读取文件系统CSV
void f_get_csv(int x)
{
  Serial.printf("FFS SIZE:%d\n", SPIFFS.totalBytes());
  Serial.printf("FFS USE SIZE:%d\n", SPIFFS.usedBytes());
  listDir(SPIFFS, "/",0);
  if(x){
    Serial.printf("postMsgId = %d\n;", postMsgId);
    getAllDataFromCSV("/list.csv");
  } 
  else{
    Serial.printf("lose_count = %d\n;", lose_count);
    getAllDataFromCSV("/lose.csv");
  } 
}
