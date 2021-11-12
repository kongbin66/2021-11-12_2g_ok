#include "config.h"
void go_sleep_a_while_with_ext0()//进入休眠
{
  uint32_t i;
  //更新睡眠时间
   //oledState = OLED_OFF;
   sleep_start_time = now_unixtime ;  //获取睡眠开始时间 
  //正在记录状态，开启定时唤醒
  if (workingState == WORKING)
  {
     i=period-(sleep_start_time- P_sleep_last_time);
     Serial.printf("i:%d\n",i);
     if(i<0||i>period) i=1;
     //启用定时唤醒
     esp_sleep_enable_timer_wakeup(i*1000000);//计时器唤醒
     Serial.println("period:"+(String)period);
     Serial.println("now sleep for " + (String)(i)+"seconds!");
  } 
  display.displayOff();
  Serial.println("EEPROM P_sleep_last_time:"+(String)P_sleep_last_time);
  eeprom_config_save_parameter();
  
  delay(500);
  uint64_t mask=0;
  mask=(1ull<<33);
  esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ALL_LOW );
  // esp_sleep_enable_ext0_wakeup(WEAKUPKEY1w, 0);//使能按键唤醒 //外部唤醒（ext0）只能使用RTC功能的GPIO：0，2，4，12-15，25-27，32-39。
  esp_sleep_enable_ext0_wakeup(WEAKUPKEY2p, 0);
 // esp_sleep_enable_ext0_wakeup(WEAKUPKEY1w, LOW);//使能按键唤醒 //外部唤醒（ext0）只能使用RTC功能的GPIO：0，2，4，12-15，25-27，32-39。
  gpio_hold_en(GPIO_NUM_14);//1锁定电源管脚
  gpio_deep_sleep_hold_en();//2全部管脚保存
  esp_deep_sleep_start();  //3.进入深度睡眠模式

}