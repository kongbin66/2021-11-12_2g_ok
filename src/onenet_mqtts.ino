#include "config.h"
/*---------------------------连接ONENET平台-------------------------------*/
bool onenet_connect()

{
  //连接OneNet并上传数据
  Serial.print("connecting to OneNet IOT...");
  client.setServer(mqtt_server, port);                   //设置客户端连接的服务器,连接Onenet服务器, 使用6002端口
  client.connect(mqtt_devid, mqtt_pubid, mqtt_password); //客户端连接到指定的产品的指定设备.同时输入鉴权信息
  if (client.connected())
    return true;
  else
    return false;
}

/*******发送温度湿度时间戳经纬度到onenet*****
发送温度湿度经纬度时间戳到ONENET
订阅主题："/thing/property/post"
发送成功返回：true
******************************************/
bool sendTempAndHumi()
{

  if (client.connected())
  {
    //先拼接出json字符串
    char param[1024];
    char jsonBuf[1024];

    if (current_rec_State == START_RECING)
      start_time = tmsp, last_time = tmsp;
    else
      last_time = tmsp;

    sprintf(param, "{\"temp\":{\"value\": %.2f,\"time\": %lu000},\"humi\":{\"value\":%.2f,\"time\": %lu000},\"start_time\":{\"value\": %lu,\"time\": %lu000},\"last_time\":{\"value\":%lu,\"time\":%lu000},\"tempLA\":{\"value\":%ld,\"time\":%lu000},\"tempUA\":{\"value\": %ld,\"time\": %lu000},\"$OneNET_LBS\":{\"value\":[{\"cid\":%ld,\"lac\":%ld,\"mcc\":%ld,\"mnc\":%ld,\"flag\":%ld}],\"time\": %lu000}}", tp, tmsp, h, tmsp, start_time, tmsp, last_time, tmsp, tempLA, tmsp, tempUA, tmsp, CID, LAC, MCC, MNC, LAC_FLAG, tmsp);

    sprintf(jsonBuf, ONENET_POST_BODY_FORMAT, param);
    //再从mqtt客户端中发布post消息
    if (client.publish(ONENET_TOPIC_PROP_POST, jsonBuf))
    {
      Serial.print("Post message to cloud: ");
      Serial.println(jsonBuf);
      current_rec_State = KEEP_RECING;
      //OneNet_connected_Time = millis();
      //rec_count++;
      return true;
    }
    else
    {
      Serial.println("Publish message to cloud failed!");
      return false;
    }
  }
  else
    Serial.println("connect failed!");

  return false;
}

void getDesired()
{
  //Serial.printf("  in getDesired !\n");
  //1.检测onenet连接
  if (client.connected())
  {
    client.subscribe(ONENET_TOPIC_DESIRED_GET);    //订阅获得期望值请求主题
    client.subscribe(ONENET_TOPIC_DESIRED_GET_RE); //订阅设备期望值获取响应主题
    client.setCallback(callback);                  //设置期望值回调
    //先拼接出json字符串
    char param[164];
    char jsonBuf[356];
    sprintf(param, "[\"temp_alarm\",\"tempL\",\"tempU\",\"period\"]"); //我们把要上传的数据写在param里
    sprintf(jsonBuf, ONENET_POST_BODY_FORMAT, param);
    //再从mqtt客户端中发布post消息
    if (client.publish(ONENET_TOPIC_DESIRED_GET, jsonBuf))
    {
      Serial.print("Post message2 to cloud: ");
      Serial.println(jsonBuf);
      f_MQTT_ON = 1;
    }
    else
    {
      Serial.println("Publish message to cloud failed!");
    }
  }
}

//回调函数
void callback(char *topic, byte *payload, unsigned int length)
{
  screenState = MAIN_TEMP_SCREEN;
  Serial.print("message rev:  ");
  Serial.println(topic);
  Serial.print("payload:  ");
  for (size_t i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  //解析JSON
  if (strstr(topic, ONENET_TOPIC_DESIRED_GET))
  {
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.println("parse json failed");
      return;
    }
    JsonObject setAlinkMsgObj = doc.as<JsonObject>();
    temp_alarm = setAlinkMsgObj["data"]["temp_alarm"]["value"];
    tempU = setAlinkMsgObj["data"]["tempU"]["value"];
    tempL = setAlinkMsgObj["data"]["tempL"]["value"];
    period = ((int)setAlinkMsgObj["data"]["period"]["value"]) * 60;
    
 
    
    if (temp_alarm)
    {
      if (tp > tempU)//温度上限报警
         tempUA++;
      else
        tempUA = 0;

      if (tp < tempL)//温度下限报警
        tempLA++;
      else
        tempLA = 0;

      if ((tp > tempU) || (tp < tempL))F_alarm_status=0;
      else F_alarm_status=1;
      Serial.printf("temp_alarm:%d,tempUA:%lu,tempLA:%lu\n", temp_alarm, tempUA, tempLA);
    }
    else//bug:关掉报警时会出现，不应该报警报警情况。
    {
      tempUA = 0;
      tempLA = 0;
      F_alarm_status=1;
    }
    digitalWrite(LED,F_alarm_status);
    eeprom_config_set(temp_alarm, (uint32_t)period, tempU, tempL,F_alarm_status);
    //发送数据
    if (sendTempAndHumi()) //数据发送成功
    {
      Serial.printf("sendTempAndHumi ok!\n");
      saveDataToCSV("/list.csv");
      postMsgId++; //记录到文件系统
      // delay(2000);
      // modem.poweroff();
    }
    else
      Serial.printf("sendTempAndHumi false!\n");
    f_MQTT_ON = 0;
    delay(1000);
    screenState = MAIN_TEMP_SCREEN;
    //modem.poweroff();
    digitalWrite(32, LOW);
  }
  modem.sleepEnable(1);
  vTaskDelay(1000);
  //modem.poweroff();
  f_800c_power_switch(false);
}




/*****MQTT设置订阅主题,并设定期望值回调函数*****/
void set_topcial()
{
  //Serial.printf("in set_topcial\n");
  if (client.connected())
  {
    //Serial.printf("in set_topcial ok!\n");
    char subscribeTopic[75];
    char topicTemplate[] = "$sys/%s/%s/cmd/request/#"; //信息模板
    snprintf(subscribeTopic, 75, topicTemplate, mqtt_pubid, mqtt_devid);
    client.subscribe(subscribeTopic); //订阅命令下发主题
    getDesired();                     //获得云端数据
  }
  //Serial.printf("out set_topcial !\n");
}
//模块连接
bool connect_gsm()
{
  screenState = SHOW_1;
  screen_show();
  setupModem();
  screenState = SHOW_2;
  screen_show();
  if (!modemToGPRS())
    return false;
  screenState = SHOW_3;
  screen_show();
  if (!getLBSLocation())
    return false;
  screenState = SHOW_4;
  screen_show();
  if (!onenet_connect())
    return false; //实验过连不上会在连接困难，直接存漏发，下次执行。
  screenState = SHOW_5;
  screen_show();
  return true;
}
/******************************************************************************
漏发处理
根据上次发送信息状态，更新数据发送一条数据
*******************************************************************************/
bool jiexi_lose()
{
  Serial.print("in jiexi_lose\n");
  static bool f_last_send_ok = 1; //上次漏发上传成功标志
  Serial.printf("lose_count=%d,f_send_ok=%d,bool_send1_ok=%d\n", lose_count, f_send_ok, f_last_send_ok);
  if (client.connected())
  {
    if (f_last_send_ok) //上一次发送成功，重新读取lose
    {
      getADataFromCSV("/lose.csv");
      parseData(&tmsp, &tp, &h, &e, &n);
    }
    Serial.printf("in jiexi_lose.tmsp=%ld,tp=%.2f,h=%.2f,e=%.4f,n=%.4f\n", tmsp, tp, h, e, n);



    if (temp_alarm)
    {
      if (tp > tempU)//温度上限报警
         tempUA++;
      else
        tempUA = 0;

      if (tp < tempL)//温度下限报警
        tempLA++;
      else
        tempLA = 0;

      if ((tp > tempU) || (tp < tempL))digitalWrite(LED, LOW);
      else digitalWrite(LED, HIGH);
      Serial.printf("temp_alarm:%d,tempUA:%lu,tempLA:%lu\n", temp_alarm, tempUA, tempLA);
    }
    else//bug:关掉报警时会出现，不应该报警报警情况。
    {
      tempUA = 0;
      tempLA = 0;
      digitalWrite(LED,HIGH);
    }

    if (sendTempAndHumi())
    {
      f_send_ok++;
      f_last_send_ok = 1;
      saveDataToCSV("/list.csv");
      postMsgId++; //记录到文件系统
      //  delay(2000);
      //  modem.poweroff();
      return true;
    }
    else //发送失败
    {
      f_lose = 1;
      f_last_send_ok = 0;
      return false;
    }
  }
  else
  {
    f_lose = 1;
    f_last_send_ok = 0;
    return false;
  }
}

/*****************************************************************************************
发送数据到onnet
******************************************************************************************/
void send_Msg_var_GSM_while_OLED_on()
{
  
  P_sleep_last_time = now_unixtime;

  if (workingState == WORKING) //工作模式
  {
    getTempAndHumilocation();  //采集温湿度，时间戳，没有经纬度(保持上次数据)。
    if (f_Flight_Mode == true) //飞行模式
    {
      saveDataToCSV("/lose.csv");
      lose_count++;  //记录到漏发文件
      f_lose = true; //置位标志位
      if (dbug)
      {
        Serial.println("/*****************lose**********************/\n ");
        getAllDataFromCSV("/lose.csv");
        Serial.printf("lose_count = %d\n ", lose_count);
      }
    }
    else //正常发送或有漏传
    {
      //1.联接网络
      if (!connect_gsm())
      {
        saveDataToCSV("/lose.csv");
        lose_count++; //漏发条数//先将本条加入到LOSE
        f_lose = 1;
        Serial.println("connect_gsm false!\n");
        return;
      }

      if (f_lose) //有漏发
      {
        saveDataToCSV("/lose.csv");
        lose_count++;        //漏发条数//先将本条加入到LOSE
        if (lose_count != 0) //连续补漏发
        {
          while (lose_count != f_send_ok)
          {
            jiexi_lose();
            vTaskDelay(500);
          }
        }
        //补发完毕,请标志
        if ((f_send_ok) == lose_count)
        {
          lose_first_flag = true;
          lose_count = 0;
          f_send_ok = 0;
          f_lose = false;
          deleteFile(SPIFFS, "/lose.csv");

          if (old_workingstate == NOT_WORKING)
          {
            workingState = NOT_WORKING;
          }
        }
      }
      else //正常发送
      {
        set_topcial();
      }
    }
    screenState = SHOW_6;
    screen_show();
  }
  else if ((workingState == NOT_WORKING) && (f_lose == true) && (f_Flight_Mode == false)) //有漏发，自动启动补传
  {
    old_workingstate = NOT_WORKING;
    workingState = WORKING;
  }
  
  
}