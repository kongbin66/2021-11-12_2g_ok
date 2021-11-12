/*----------------------------------------------------------
*                 sht20操作相关
*
*---------------------------------------------------------*/
#include "config.h"
#ifdef _SHT20
void sht20getTempAndHumi()
{
  float _currentTemp = sht20.temperature();
  float _currentHumi = sht20.humidity();

  if ((_currentTemp < 100)&&(_currentTemp > -40))
  {
    currentTemp = _currentTemp;
    currentHumi = _currentHumi;
    tempAndHumi_Ready = true;
  }
}

#else
/*----------------------------------------------------------
*                 ds18b20操作相关
*
*---------------------------------------------------------*/

//读18B20温度
bool read_18b20(float* celsius)
{
    byte i;
    byte present = 0; //当前
    byte type_s;
    byte data[12];
    byte addr[8];
    float  fahrenheit; //摄氏度，华氏度
    vTaskDelay(100);
    while(!ds.search(addr))
    {
      // Serial.println("No more addresses.");
       //Serial.println();
        ds.reset_search();
        delay(50);
        return 0;
    }

    // Serial.print("ROM =");
    // for (i = 0; i < 8; i++)
    // {
    //     Serial.write(' ');
    //     Serial.print(addr[i], HEX);
    // }

    if (OneWire::crc8(addr, 7) != addr[7])
    {
        Serial.println("CRC is not valid!");
        return 0;
    }
    //Serial.println();

    // 第一个ROM字节表示哪种芯片
    switch (addr[0])
    {
    case 0x10:
       // Serial.println("  Chip = DS18S20"); // or old DS1820
        type_s = 1;
        break;
    case 0x28:
       // Serial.println("  Chip = DS18B20");
        type_s = 0;
        break;
    case 0x22:
       // Serial.println("  Chip = DS1822");
        type_s = 0;
        break;
    default:
       // Serial.println("Device is not a DS18x20 family device.");
        return 0;
    }
    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1); // 开始转换，最后打开寄生电源

    //delay(750); // 也许750ms足够了，也许不行，我们可以在这里做一个ds.depower（），但是重置会处理它。

    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE); // 读暂存存储器

    // Serial.print("  Data = ");
    // Serial.print(present, HEX);
    // Serial.print(" ");
    for (i = 0; i < 9; i++)
    { // 我们需要9个字节
        data[i] = ds.read();
        // Serial.print(data[i], HEX);
        // Serial.print(" ");
    }
    // Serial.print(" CRC=");
    // Serial.print(OneWire::crc8(data, 8), HEX);
    // Serial.println();

    // 将数据转换为实际温度因为结果是一个16位有符号整数，所以它应该存储为“int16\t”类型，即使在32位处理器上编译，也始终是16位。
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s)
    {
        raw = raw << 3; // 9位分辨率默认值
        if (data[7] == 0x10)
        {
            // “count remain”提供完整的12位分辨率
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
    }
    else
    {
        byte cfg = (data[4] & 0x60);
        // 在低分辨率下，低位是未定义的，所以我们把它们归零
        if (cfg == 0x00)
            raw = raw & ~7; // 9位分辨率，93.75 ms
        else if (cfg == 0x20)
            raw = raw & ~3; // 10位分辨率，187.5 ms
        else if (cfg == 0x40)
            raw = raw & ~1; // 11位分辨率，375 ms
                            ////默认为12位分辨率，750毫秒转换时间
    }
    *celsius = (float)raw / 16.0;
    fahrenheit = *celsius * 1.8 + 32.0;
    // Serial.print("  Temperature = ");
    // Serial.print(*celsius);
    // Serial.print(" Celsius, ");
    // Serial.print(fahrenheit);
    // Serial.println(" Fahrenheit");

    return 1;
}
//jieyingjian 
float f_get18b20Temp()
{
    float celsius;
    while(!read_18b20(&celsius))
    {
        //Serial.printf("ds18b20 error!\n");
        delay(100);
    }
    currentTemp = celsius; 
    return  celsius;

   if(dbug) Serial.printf("currentTemp=%f\n",currentTemp);

}
#endif
