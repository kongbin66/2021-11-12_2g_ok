#include "config.h"

void test_alarm() //检测报警
{
      if (tp > tempU)//温度上限报警
         tempUA++;
      else 
        tempUA = 0;

      if (tp < tempL)//温度下限报警
        tempLA++;
      else 
        tempLA = 0;
}

    // if (temp_alarm)
    // {
    //   if (tp > tempU)//温度上限报警
    //      tempUA++;
    //   else
    //     tempUA = 0;

    //   if (tp < tempL)//温度下限报警
    //     tempLA++;
    //   else
    //     tempLA = 0;

    //   if ((tp > tempU) || (tp < tempL))digitalWrite(LED, LOW);
    //   else digitalWrite(LED, HIGH);
    //   Serial.printf("temp_alarm:%d,tempUA:%lu,tempLA:%lu\n", temp_alarm, tempUA, tempLA);
    // }
    // else//bug:关掉报警时会出现，不应该报警报警情况。
    // {
    //   tempUA = 0;
    //   tempLA = 0;
    //   digitalWrite(LED,HIGH);
    // }