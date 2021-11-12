#include "config.h"

void nullfunc()
{
  P_show_On_Start_time = sys_sec;
  P_show_now_time = sys_sec;
}

/***********************************************
按键 初始化
a=1;使能按键工能，a=0;失能按键功能
***********************************************/

void key_init(bool a)
{
  if (a)//使能按键工能
  {
    button2.reset();             //清除一下按钮状态机的状态
    button2.setPressTicks(2000); //设置长按毫秒数
    button2.attachClick(f_button2_click);
    button2.attachDuringLongPress(duringLongPres2);
    button.reset(); //清除一下按钮状态机的状态
    button.attachClick(click);
    button.attachDoubleClick(doubleclick);
    button.attachLongPressStart(longPressStart);
    button.attachDuringLongPress(duringLongPress);
    button.attachLongPressStop(longPressStop);
  }
  else//失能按键功能
  {
    button.attachClick(nullfunc);
    button.attachDoubleClick(nullfunc);
    button.attachLongPressStart(nullfunc);
    button.attachDuringLongPress(nullfunc);
    button.attachLongPressStop(nullfunc);
  }
}
/***********************************************/
//button回调单击
void click()
{
  oledState = OLED_WORK;
  if (oledState == OLED_WORK)
  {
    Serial.println("click");
    keyState = CLICK;
  }
  P_show_On_Start_time = sys_sec;
  P_show_now_time = sys_sec;
}
/***********************************************/
//button回调双击
void doubleclick()
{
  oledState = OLED_WORK;

  if (oledState == OLED_WORK)
  {
    Serial.println("doubleclick");
    keyState = DOUBLECLICK;
  }
  P_show_On_Start_time = sys_sec;
  P_show_now_time = sys_sec;
}
/***********************************************/
//button回调长按开始
void longPressStart()
{
  oledState = OLED_WORK;

  if (oledState == OLED_WORK)
  {
    Serial.println("longPressStart");
    keyState = LONGPRESS_START;
  }
  t_keylongpress_start = sys_sec;
  P_show_On_Start_time = sys_sec;
  P_show_now_time = sys_sec;
}
/***********************************************/
//button回调长按过程中
void duringLongPress()
{
  oledState == OLED_WORK;
  if (oledState = OLED_WORK)
  {
    P_show_now_time = sys_sec;
    t_keylongpress_now = sys_sec;
    keyState = LONGPRESS_DURRING;
  }

  if (button.isLongPressed())//检测到长按
  {
    Serial.print("duringLongPress:");
    Serial.println(button.getPressedTicks());//输出长按滴答

    keyScreen_Start = sys_sec; //处理长按界面下的长按？？
    keyScreen_Now = sys_sec;
  }
  // Serial.println(t_keylongpress_now - t_keylongpress_start);
  // if (t_keylongpress_now - t_keylongpress_start >= 1)
  // {
  //   keyState = LONGPRESS_DURRING;
  // }
}
/***********************************************/
//button回调长按结束时
void longPressStop()
{
  oledState = OLED_WORK;
  P_show_now_time = sys_sec;
  Serial.println("longPressStop");
  keyState = LONGPRESS_END;
}
/***********************************************/
//电源按键单击
void f_button2_click()
{
  oledState = OLED_WORK;
  if (oledState == OLED_WORK)
  {
    Serial.println("button2_click\n");
  }
  P_show_On_Start_time = sys_sec;
  P_show_now_time = sys_sec;
}
/***********************************************/
//button2长按关机
//由于唤醒功能同在此管脚，采用息屏后暗示放手，5秒后设备才关机，如果5秒内不放手的话又会开机
void duringLongPres2()
{
  Serial.println("button2_duringLongPres2****************************************\n");

  // if (oledState == true) //关机
  // {
  //   oledState = false;
  //   Serial.println("guanji\n");
  //   delay(5000);
  // }
  // else
  // {
  //   oledState = true;
  //   Serial.println("kaiji\n");
  //   delay(5000);
  // }


   if(oledState == OLED_WORK)//关机操作
   {
      oledState = OLED_OFF;
      eeprom_oledstate_set(oledState);

      P_show_On_Start_time = sys_sec;
      P_show_now_time = sys_sec;

        F_alarm_status=1;
        digitalWrite(LED,F_alarm_status);

         
         workingState = NOT_WORKING;
         list_first_flag=true;
         lose_first_flag=true;
         postMsgId = 0; //清记录条数
         lose_count=0;
         temp_alarm=0;
         eeprom_config_set(temp_alarm, (uint32_t)period, tempU, tempL,F_alarm_status);
         
         delay(500);
         display.displayOff(); //息屏
         delay(3000);
         Serial.printf("power down\n");
       esp_sleep_enable_ext0_wakeup(WEAKUPKEY2p, LOW); //使能按键唤醒
       gpio_hold_en(GPIO_NUM_32);                     //锁定电源管脚
       gpio_deep_sleep_hold_en();
       esp_deep_sleep_start();
   }
   else//关机
   {

   }
}
/***********************************************/
//按键扫描
void key_loop()
{
  //Serial.printf("in key_loop,keyState=%d\n",keyState);
  switch (keyState)
  {

  case CLICK: //单击
    if (screenState == MAIN_TEMP_SCREEN || screenState == MAIN_HUMI_SCREEN)
    {
      screen_loopEnabled = false;
      screenState = TIPS_SCREEN;
      keyScreen_Start = sys_sec;
      keyScreen_Now = sys_sec;
      keyState = NOKEYDOWN;
    }
    else if (screenState == TIPS_SCREEN || screenState == BLE_SCREEN || screenState == REC_STOP_SCREEN || screenState == REC_START_SCREEN)
    {
      screen_loopEnabled = true;
      Serial.println("anjianfanhui");
      keyState = NOKEYDOWN;
    }
    break;

  case DOUBLECLICK: //双击
    if (screenState == MAIN_TEMP_SCREEN || screenState == MAIN_HUMI_SCREEN || screenState == TIPS_SCREEN)
    {
      keyScreen_Start = sys_sec; //刷新界面自动返回
      keyScreen_Now = sys_sec;

      P_show_On_Start_time = sys_sec; //刷新亮屏时间
      P_show_now_time = sys_sec;
      if (f_Flight_Mode == true) //正在飞行模式，关闭。
      {
        f_Flight_Mode = false;
        workingState = NOT_WORKING;
        screen_loopEnabled = false;
        screenState = fxmod_OFF;
        qualifiedState = false;
        Serial.printf("flight off\n");
        keyState = NOKEYDOWN;
      }
      else if (f_Flight_Mode == false) //关闭时飞行模式，打开
      {
        f_Flight_Mode = true;
        workingState = WORKING;
        screen_loopEnabled = false;
        screenState = fxmod_ON;
        qualifiedState = true;
        Serial.printf("flight on\n");
        keyState = NOKEYDOWN;
      }
    }
    break;
  case LONGPRESS_DURRING:
  {
    keyScreen_Start = sys_sec; //刷新界面自动返回
    keyScreen_Now = sys_sec;
    if (screenState == MAIN_TEMP_SCREEN || screenState == MAIN_HUMI_SCREEN || screenState == TIPS_SCREEN)
    {
      if (workingState == NOT_WORKING)
      {
        screen_loopEnabled = false;
        screenState = REC_START_SCREEN;
        workingState = WORKING;
        old_workingstate = WORKING;
        current_rec_State = START_RECING;
        alFFS_thisRec_firstData_flag = true;
        Serial.println("workingState = WORKING\n");
        keyState = NOKEYDOWN;
        tempUA = tempLA = 0;
        P_sleep_last_time = 0;
      }
      else if (workingState == WORKING)
      {
        screen_loopEnabled = false;
        screenState = REC_STOP_SCREEN; //停止记录界面
        workingState = NOT_WORKING;
        old_workingstate = NOT_WORKING;
        current_rec_State = END_RECING; //记录状态：停止
        Serial.println("workingState = NOT_WORKING\n");

        keyScreen_Start = sys_sec; //停止记录界面自动返回时间刷新
        keyScreen_Now = sys_sec;
        keyState = NOKEYDOWN;
        tempUA = tempLA = 0;
      }
    }
    keyState = NOKEYDOWN; //处理长按界面下的长按
  }
  break;

  case LONGPRESS_END:          //长按结束
    keyScreen_Start = sys_sec; //刷新界面自动返回
    keyScreen_Now = sys_sec;
    keyState = NOKEYDOWN; //处理长按界面下的长按
    break;

  default:
    break;
  }
}
