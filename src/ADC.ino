#include "config.h"


void set_ADC(void)
{
  //ADC设置

  analogSetWidth(12); //分辨率4095
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db); //全通道衰减11db,能测到2.6v
  analogSetPinAttenuation(adcpin36, ADC_11db);
  adcAttachPin(adcpin36);
  Serial.printf("ADC ok\n");
}
/*****************************************************************
 *4.2-100
 3.85-75
 3.75-50
 3.6-25
 3.4-5 
******************************************************************/
void BAT_ADC_Sampling() //电池电压采样
{

  int j = 0, k;
  for (k = 0; k < 10; k++)
  {
    j += analogRead(adcpin36);
    delay(100);
    //Serial.printf("zpotpin36=%d\n", j);
  }
  j = j / k;
  if(dbug)Serial.printf("potpin36=%d\n", j);
  bat_voltage = ((3.3 / 4095) * j) * 2;
  if(dbug)Serial.printf("bat_voltage=%.2f\n", bat_voltage);
  if (bat_voltage >= 4.2) p1 = F16x16_b100;
  else if (bat_voltage < 4.2 && bat_voltage >= 3.85)
    p1 = F16x16_b80;
  else if (bat_voltage < 3.85 && bat_voltage >= 3.75)
    p1 = F16x16_b40;
  else if (bat_voltage < 3.75 && bat_voltage >= 3.6)
    p1 = F16x16_b20;
  else if (bat_voltage < 3.6 && bat_voltage >= 3.4)
    p1 = F16x16_b10;
   else if (bat_voltage >= 3.4)
    p1 = F16x16_b0;
   
}

// void f_power_management()
// {
//   //连续监测TYPEC连接情况
//   //如果连接：每间隔x秒监测一下电池是否充满，充满后显示电量满，否则显示充电动画。
//   //如果未连接：每隔一定时间监测实施电池电量，显示出来。

//   P_tpyec_connect = f_typec_connect();
//   if (P_tpyec_connect) //连接了充电线
//   {
//     if (now_unixtime - ADC_Sampling_interval > 6)//时间间隔到
//     {
//       ADC_Sampling_interval = now_unixtime;

//       BAT_ADC_Sampling();
//       if (bat_voltage >=4.0) //检测电量
//       {
//         p1 = F16x16_b100;
//         P_BAT_FULL = 1;
//         if(dbug)Serial.printf("full\n");
//       }
//       else
//       {
//          P_BAT_FULL = 0;
//       }
//     }
//     else//时间未到，电池不满，显示充电动画
//     {
//       if (P_BAT_FULL == 0)
//         Battery_Charge();
//       //Serial.printf("no full\n");
//     }
//   }
//   else //未连接充电线
//   {
//     if (now_unixtime - ADC_Sampling_interval > 6)
//     {
//       ADC_Sampling_interval = now_unixtime;
//       BAT_ADC_Sampling();
//       Serial.printf("dianliang jiance\n");
//     }
//   }
// }


void f_power_management()
{
  //连续监测TYPEC连接情况
  //如果连接：每间隔x秒监测一下电池是否充满，充满后显示电量满，否则显示充电动画。
  //如果未连接：每隔一定时间监测实施电池电量，显示出来。
  int j=0;
  for(int i=0;i<3;i++)
  {
     if(digitalRead(type_connect39))j++;
     else j=0;
     vTaskDelay(10);
  }

  if (j>=3) //连接了充电线
  {
    
    if (now_unixtime - ADC_Sampling_interval > 6)//时间间隔到
    {
      ADC_Sampling_interval = now_unixtime;

      BAT_ADC_Sampling();
      if (bat_voltage >=4.0) //检测电量
      {
        p1 = F16x16_b100;
        P_BAT_FULL = 1;
        if(dbug)Serial.printf("full\n");
      }
      else
      {
         P_BAT_FULL = 0;
      }
    }
    else//时间未到，电池不满，显示充电动画
    {
      if (P_BAT_FULL == 0)
        Battery_Charge();
      //Serial.printf("no full\n");
    }
  }
  else //未连接充电线
  {
    if (now_unixtime - ADC_Sampling_interval > 6)
    {
      ADC_Sampling_interval = now_unixtime;
      BAT_ADC_Sampling();
      Serial.printf("dianliang jiance\n");
    }
  }
  j=0;
}