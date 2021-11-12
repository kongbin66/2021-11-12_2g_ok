#include "config.h"
#include "TinyGsmCommon.h"
#include "TinyGsmClientSIM800.h"
#include "TinyGsmModem.tpp"

/*-------------------------------APN相关定义-------------------------------------*/
const char apn[] = "CMNET"; // Your APN
const char gprsUser[] = ""; // User
const char gprsPass[] = ""; // Password
const char simPIN[] = "";   // SIM card PIN code, if any

/*-------------------------------初始化SIM800L-------------------------------------*/

void f_800c_power_switch(int a)//800c供电
{
  if (a)
  {
    digitalWrite(SW_POWER, HIGH);
    vTaskDelay(100);
    digitalWrite(GSM_power, HIGH);
    
    digitalWrite(ip5306_key, LOW);
    vTaskDelay(100);
    digitalWrite(ip5306_key, HIGH);
    vTaskDelay(100);
  }
  else
  {
    digitalWrite(SW_POWER, LOW);
    digitalWrite(GSM_power, LOW);
  }
}

void setupModem()
{
  f_800c_power_switch(1);
  modem.sleepEnable(0);
  vTaskDelay(1000L);//模块启动需要1秒

  SerialMon.print("Initializing modem...");
  modem.restart();
  modem.init(); //开机后modem初始化一下
  client.disconnect(); //客户端断开连接测试
  SerialMon.println("OK");
}


bool modemTOnetwork(uint32_t a)
{
  //连接网络
    if(modem.waitForNetwork(a))
    {
      return true;
    }
    return false;
}

bool modemTOgprs()
{
  int i;
  for(i=0;i<Reconnection;i++)
  {
    if(modem.gprsConnect(apn, gprsUser, gprsPass))
    {
      break;
    }
    SerialMon.printf("gprs connet:%d\n",i);
  }
  if(i>=Reconnection)return false;
  return true;

}

bool modemToGPRS()
{
  //连接网络
  SerialMon.print("Waiting for network...");
  if(!modemTOnetwork(120000L))
  {
     return false;
  }
  else SerialMon.println(" OK");
  //连接GPRS接入点
  SerialMon.print(F("Connecting to APN: "));
  SerialMon.print(apn);

  if(!modemTOgprs())
  {
     return false;
  }
  else SerialMon.println(" OK");
  SerialMon.print("signalQuality:");
  Serial.println(modem.getSignalQuality());
  return true;
}





/*-------------------------------获取位置信息-------------------------------------*/



bool getLBSLocation()
{
  String lac1;
  String ciid;
  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int min = 0;
  int sec = 0;
  float timezone = 0;
  int count = 0;
  int splitsybol[3];
  bool res = 0;
  char buf[4];
  char *p2;
  char *str;

  Serial.println("getting LBS...");
  String Position = modem.getGsmLocation1();
  res = modem.getNetworkTime(&year, &month, &day, &hour, &min, &sec, &timezone);
#if 0
    Serial.println(year);
    Serial.println(month);
    Serial.println(day);
    Serial.println(hour);
    Serial.println(min);
    Serial.println(sec);
    Serial.println(timezone);
#endif

  if ((Position.length() >= 17) && (res == 1))
  {
    //1.处理大小区号
    for (size_t i = 0; i < Position.length(); i++)
    {
      if (Position[i] == ',')
      {
        splitsybol[count] = i;
        count++;
      }
    }
    lac1 = Position.substring(splitsybol[1] + 1, splitsybol[2]).c_str(); //c_str()函数返回一个指向正规C字符串的指针常量, 内容与本string串相同.
    ciid = Position.substring(splitsybol[2] + 1, splitsybol[3]).c_str();
    // Serial.println(LAC);
    // Serial.println(CIID);
    lac1.replace("\"", "");
    ciid.replace("\"", "");
    //  Serial.println(LAC);
    //  Serial.println(CIID);
    lac1.toCharArray(buf, 5, 0);
    //Serial.println(buf);
    p2 = buf;
    LAC = strtol(p2, &str, 16);
    Serial.println(LAC);
    ciid.toCharArray(buf, 5, 0);
    //Serial.println(buf);
    p2 = buf;
    CID = strtol(p2, &str, 16);
    Serial.println(CID);
    //2.存储数据

    timeNow_Y = year;
    timeNow_M = month;
    timeNow_D = day;
    timeNow_h = hour;
    timeNow_m = min;
    timeNow_s = sec;
    now1.year = timeNow_Y - 2000;
    now1.month = timeNow_M;
    now1.day = timeNow_D;
    now1.hour = timeNow_h;
    now1.minute = timeNow_m;
    now1.second = timeNow_s;
    ds_rtc.setDateTime(&now1);
    now_unixtime = unixtime();
    time_last_async_stamp = now_unixtime;
  }
}
