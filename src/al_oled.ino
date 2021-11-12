#include "config.h"

//启动界面 安冷科技
void showWelcome()
{
  display.setFont(ArialMT_Plain_10);
  for (size_t i = 0; i < 64; i++)
  {
    display.clear();
    display.drawXbm(128 - 2 * i, 16, 32, 32, logo_logo);
    display.drawXbm(32 + 128 - 2 * i, 20, 24, 24, logo_an);
    display.drawXbm(56 + 128 - 2 * i, 20, 24, 24, logo_leng);
    display.drawXbm(80 + 128 - 2 * i, 20, 24, 24, logo_ke);
    display.drawXbm(104 + 128 - 2 * i, 20, 24, 24, logo_ji);
    display.drawString(12, 51 + 64 - i, "wwww.anleng-tec.com");
    display.display();
  }
  delay(2000);
}

//温度显示界面
void showTemp()
{
  char tempParam[32];
  sprintf(tempParam, "%.1f°C", currentTemp);
  display.clear();
  showStatusBar();
  display.drawXbm(0, 20, 16, 32, temp_icon);
  display.setFont(Roboto_Condensed_38);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(72, 12, tempParam);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.display();
}
//湿度显示界面
void showHumi()
{
  char humiParam[32];
  sprintf(humiParam, "%.1f%%", currentHumi);
  display.clear();
  showStatusBar();
  display.drawXbm(0, 22, 16, 32, humi_icon);
  display.setFont(Roboto_Condensed_38);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(72, 12, humiParam);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.display();
}
//温湿度滚动界面(阻塞)
void temp_humi_Scroll()
{
  for (size_t i = 0; i < 33; i++)
  {
    char tempParam[32];
    sprintf(tempParam, "%.1f°C", currentTemp);
    char humiParam[32];
    sprintf(humiParam, "%.1f%%", currentHumi);
    display.clear();
    showStatusBar();
    if (screenState == TEMP_HUMI_SCROLL_SCREEN)
    {
      display.drawXbm(0 - i * 4, 20, 16, 32, temp_icon);
      display.setFont(Roboto_Condensed_38);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(72 - i * 4, 12, tempParam);
      display.drawXbm(0 + 128 - i * 4, 22, 16, 32, humi_icon);
      display.setFont(Roboto_Condensed_38);
      display.drawString(72 + 128 - i * 4, 12, humiParam);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
    }
    if (screenState == HUMI_TEMP_SCROLL_SCREEN)
    {
      display.drawXbm(0 - i * 4, 22, 16, 32, humi_icon);
      display.setFont(Roboto_Condensed_38);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(72 - i * 4, 12, humiParam);
      display.drawXbm(0 + 128 - i * 4, 20, 16, 32, temp_icon);
      display.setFont(Roboto_Condensed_38);
      display.drawString(72 + 128 - i * 4, 12, tempParam);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
    }
    //delay(100);
    display.display();
  }
}
//单击后的提示界面
void showTips_Screen()
{
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawXbm(24, 0, 14, 16, tips_shi2);
  display.drawXbm(44, 0, 14, 16, tips_yong);
  display.drawXbm(64, 0, 14, 16, tips_shuo);
  display.drawXbm(84, 0, 14, 16, tips_ming);
  display.drawXbm(0, 16, 14, 16, tips_chang);
  display.drawXbm(14, 16, 14, 16, tips_an);
  display.drawString(28, 16, ":");
  display.drawXbm(36, 16, 14, 16, tips_kai);
  display.drawXbm(50, 16, 14, 16, tips_shi);
  display.drawString(64, 16, "/");
  display.drawXbm(72, 16, 14, 16, tips_ting);
  display.drawXbm(86, 16, 14, 16, tips_zhi);
  display.drawXbm(100, 16, 14, 16, tips_ji);
  display.drawXbm(114, 16, 14, 16, tips_lu);
  display.drawXbm(0, 32, 14, 16, tips_shuang);
  display.drawXbm(14, 32, 14, 16, tips_ji2);
  display.drawString(28, 32, ":");
  display.drawXbm(36, 32, 14, 16, tips_kai);
  display.drawXbm(50, 32, 14, 16, tips_guan);
  display.drawXbm(64, 32, 14, 16, tips_fei);
  display.drawXbm(78, 32, 14, 16, tips_xing);
  display.drawXbm(92, 32, 14, 16, tips_mo);
  display.drawXbm(106, 32, 14, 16, tips_shik);
  display.drawCircleQuads(52, 57, 6, 6);
  display.drawHorizontalLine(52, 51, 25);
  display.drawHorizontalLine(52, 63, 25);
  display.drawCircleQuads(76, 57, 6, 9);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(Roboto_Condensed_12);
  display.drawString(64, 50, "OK");
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.display();
}

//补发漏传界面
void showBFLC_Screen()
{
  display.clear();
  display.setFont(Roboto_Condensed_38);
  display.drawString(0, 0, "BFLC: T");
  display.display();
}

//长按后的界面
void show_recStart_Screen()
{
  display.clear();
  display.drawHorizontalLine(0, 10, 80);
  display.drawHorizontalLine(48, 54, 80);
  display.drawXbm(4, 20, 24, 24, tips_recon);
  display.drawXbm(28, 20, 24, 24, tips_kai3);
  display.drawXbm(52, 20, 24, 24, tips_shi3);
  display.drawXbm(76, 20, 24, 24, tips_ji3);
  display.drawXbm(100, 20, 24, 24, tips_lu3);
  display.display();
}
//飞行模式
void showfxms_Screen1()
{
  display.clear();
  display.drawHorizontalLine(0, 10, 80);
  display.drawHorizontalLine(48, 54, 80);
  display.drawXbm(4, 20, 24, 24, tips_recon);
  display.drawXbm(28, 20, 24, 24, temp_f);
  display.drawXbm(52, 20, 24, 24, temp_x);
  display.drawXbm(76, 20, 24, 24, temp_m);
  display.drawXbm(100, 20, 24, 24, temp_s);
  display.display();
}
//记录模式
void showfxms_Screen2()
{
  display.clear();
  display.drawHorizontalLine(0, 10, 80);
  display.drawHorizontalLine(48, 54, 80);
  display.drawXbm(4, 20, 24, 24, tips_recon);
  display.drawXbm(28, 20, 24, 24, temp_j);
  display.drawXbm(52, 20, 24, 24, temp_l);
  display.drawXbm(76, 20, 24, 24, temp_m);
  display.drawXbm(100, 20, 24, 24, temp_s);
  display.display();
}

void show_recStop_Screen()
{
  display.clear();
  display.drawHorizontalLine(0, 10, 80);
  display.drawHorizontalLine(48, 54, 80);
  display.drawXbm(4, 20, 24, 24, tips_recoff);
  display.drawXbm(28, 20, 24, 24, tips_ting3);
  display.drawXbm(52, 20, 24, 24, tips_zhi3);
  display.drawXbm(76, 20, 24, 24, tips_ji3);
  display.drawXbm(100, 20, 24, 24, tips_lu3);
  display.display();
}

/******************************上传画面1******************************/
//显示初始GSM
void show_Initializing_modem1()
{
  display.clear();
  display.setFont(Roboto_Condensed_12);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 5, "Initializing modem...");
  display.drawProgressBar(5, 50, 118, 8, 5);
  display.display();
}
/******************************上传画面2******************************/
//显示连接GSM网络
void show_Waiting_for_network2()
{
  display.clear();
  display.setFont(Roboto_Condensed_12);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 5, "Waiting for network...");
  display.drawProgressBar(5, 50, 118, 8, 40);
  display.display();
}
/******************************上传画面3******************************/
//显示获取位置
void show_getting_LBS3()
{
  display.clear();
  display.setFont(Roboto_Condensed_12);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 5, "getting LBS...");
  display.drawProgressBar(5, 50, 118, 8, 70);
  display.display();
}
/******************************上传画面4******************************/
//显示连接ONENET
void show_connecting_to_OneNet4()
{
  display.clear();
  display.setFont(Roboto_Condensed_12);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 5, "connecting to OneNet");
  display.drawProgressBar(5, 50, 118, 8, 90);
  display.display();
}
/******************************上传画面5******************************/
//显示上传数据
void show_uploading5()
{
  display.clear();
  display.setFont(Roboto_Condensed_12);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 5, "uploading...");
  display.drawProgressBar(5, 50, 118, 8, 100);
  vTaskDelay(1000);
  // display.clear();
  display.display();
}
/******************************上传画面6******************************/
//显示上传完成
void show_full()
{
  display.drawProgressBar(5, 50, 118, 8, 100);
  display.display();
  vTaskDelay(200);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

//充电动画
void Battery_Charge()
{
  now_number = millis();
  now_state = (now_number - start_number);
  if (now_state > 500)
  {
    static int i = 0;
    i++;
    if (i > 6)
      i = 0;
    switch (i)
    {
    case 0:
      p1 = F16x16_b0;
      break;
    case 1:
      p1 = F16x16_b10;
      break;
    case 2:
      p1 = F16x16_b20;
      break;
    case 3:
      p1 = F16x16_b40;
      break;
    case 4:
      p1 = F16x16_b60;
      break;
    case 5:
      p1 = F16x16_b80;
      break;
    case 6:
      p1 = F16x16_b100;
      break;
    default:
      break;
    }
    start_number = millis();
  }
}

//状态栏
void showStatusBar()
{
  char time[20];
  char voltage[10];
  // Serial.println("in showStatusBar");
  // Serial.printf("bleState :%d\n",bleState );

  //工作状态
  if (workingState == WORKING)
    display.drawIco16x16(0, 0, F16x16_right);
  else
    display.drawIco16x16(0, 0, F16x16_space);

  //BLE开关
  if (bleState == BLE_ON) // && workingState != WORKING)
    display.drawIco16x16(16, 0, F16x16_bluetooth);
  else
    display.drawIco16x16(16, 0, F16x16_space);

  //方圆（合格）
  if (qualifiedState == true && workingState == WORKING)
    display.drawIco16x16(32, 0, F16x16_rec);
  else
    display.drawIco16x16(32, 0, F16x16_space);

  //锁定
  if (lockState == LOCKED) // && workingState == WORKING)
    display.drawIco16x16(48, 0, F16x16_lock);
  else
    display.drawIco16x16(48, 0, F16x16_space);

  display.setFont(Roboto_Condensed_12);

  //时间
  snprintf(time, sizeof(time), "%02d:%02d:%02d", now1.hour, now1.minute, now1.second);
  display.drawString(64, 0, time);

  //电量显示

  display.drawIco16x16(112, 0, p1);
  //画水平线
  display.drawHorizontalLine(0, 16, 128);
  display.drawHorizontalLine(0, 53, 128);

  //记录条数信息
  display.setFont(Dialog_plain_8);
  display.drawString(0, 56, "REC:");
  display.drawString(20, 56, (String)postMsgId);

  //日期
  snprintf(time, 20, "%04d-%02d-%02d", now1.year + 2000, now1.month, now1.day);
  display.drawString(78, 56, time);

  // //电压显示
  if (dbug == 10)
  {
    snprintf(voltage, sizeof(voltage), "%.2f", bat_voltage);
    display.drawString(40, 56, voltage);
  }

  //充电线
  //display.drawString(25,56, (String)P_tpyec_connect);
}
//屏幕状态机循环
void screen_loop()
{
  #ifdef _SHT20
  if (screen_loopEnabled)
  {
    loopnowTime = millis();
    looptimeSpan = loopnowTime - loopStartTime;

    switch (looptimeSpan)
    {
    case 0 ... 5000:
      screenState = MAIN_TEMP_SCREEN;
      break;
    case 5001 ... 5200:
      screenState = TEMP_HUMI_SCROLL_SCREEN;
      break;
    case 5201 ... 9800:
      screenState = MAIN_HUMI_SCREEN;
      break;
    case 9801 ... 10000:
      screenState = HUMI_TEMP_SCROLL_SCREEN;
      break;
    case 10001 ... INT_MAX:
      loopStartTime = millis();
      screenState = MAIN_TEMP_SCREEN;
      break;
    default:
      break;
    } 
  }
  #endif
}

void screen_show()
{
  switch (screenState)
  {
  case MAIN_TEMP_SCREEN:
    showTemp();
    break;
  case TEMP_HUMI_SCROLL_SCREEN:
    temp_humi_Scroll();
    break;
  case MAIN_HUMI_SCREEN:
    showHumi();
    break;
  case HUMI_TEMP_SCROLL_SCREEN:
    temp_humi_Scroll();
    break;

  case TIPS_SCREEN:
    keyScreen_Now = sys_sec;
    if (keyScreen_Now - keyScreen_Start < P_show_auto_return_time)
      showTips_Screen();
    else
    {
      screen_loopEnabled = true;
      screenState = MAIN_TEMP_SCREEN;
    }
    break;

  case fxmod_ON://飞行模式开
    keyScreen_Now = sys_sec;
    if (keyScreen_Now - keyScreen_Start < P_show_auto_return_time)
      showfxms_Screen1();
    else
    {
      screen_loopEnabled = true;
      screenState = MAIN_TEMP_SCREEN;
    }
    break;

  case fxmod_OFF://飞行模式关
    keyScreen_Now = sys_sec;
    if (keyScreen_Now - keyScreen_Start < P_show_auto_return_time)
      showfxms_Screen2();
    else
    {
      screen_loopEnabled = true;
      screenState = MAIN_TEMP_SCREEN;
    }
    break;

  case REC_START_SCREEN://记录模式开始
    keyScreen_Now = sys_sec;
    if (keyScreen_Now - keyScreen_Start < P_show_auto_return_time)
    {
      show_recStart_Screen();
      vTaskDelay(3000);
    }
    else
    {
      screen_loopEnabled = true;
      screenState = MAIN_TEMP_SCREEN;
    }
    break;

  case REC_STOP_SCREEN://记录模式关闭
    keyScreen_Now = sys_sec;
    if (keyScreen_Now - keyScreen_Start < P_show_auto_return_time){
        show_recStop_Screen();
    }  
    // else if (keyState != LONGPRESS_DURRING)
    // {
    //   screen_loopEnabled = true;
    //   screenState = MAIN_TEMP_SCREEN;
    // }
    else{
      screen_loopEnabled = true;
      screenState = MAIN_TEMP_SCREEN;
    }
    break;

  case SHOW_1:
    show_Initializing_modem1();
    break;
  case SHOW_2:
    show_Waiting_for_network2();
    break;
  case SHOW_3:
    show_getting_LBS3();
    break;
  case SHOW_4:
    show_connecting_to_OneNet4();
    break;
  case SHOW_5:
    show_uploading5();
    break;
  default:
    break;
  }
}

void oled_on_off_switch()
{
  P_show_now_time = sys_sec;

  if (P_show_now_time - P_show_On_Start_time > screen_On_last_span) //亮屏时间到了吗
  {
    // if (oledState == OLED_ON) //到了
    // {
    //   oledState = OLED_OFF;
    //   display.displayOff();
    //   Serial.println("displayOff");
   
    // }
    if(oledState==OLED_WORK)
    {
       oledState=OLED_SLEEP;
      display.displayOff();
      Serial.println("displayOff");
      eeprom_oledstate_set(oledState);
    }
      
  }
  if (P_show_now_time - P_show_On_Start_time > screen_On_last_span )//+ screen_Off_to_sleep_span) //该进入睡眠吗
  {
     f_800c_power_switch(false);
    go_sleep_a_while_with_ext0();
  }
}