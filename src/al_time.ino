#include "config.h"


void wakeup_init_time()//开机更新
{
  sleep_end_time = now_unixtime ;
  sleep_time_count = sleep_end_time - P_sleep_last_time;
  Serial.printf("sleep count: %ld ,end_time:%ld,P_sleep_last_time:%ld\r\n", sleep_time_count ,sleep_end_time,P_sleep_last_time);
}




#define SECONDS_FROM_1970_TO_2000 \
946656000
#define pgm_read_byte(addr)   (*(const unsigned char *)(addr))
const uint8_t daysInMonth[] PROGMEM = {31, 28, 31, 30, 31, 30,31, 31, 30, 31, 30};

static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) 
{
  if (y >= 2000U)
    y -= 2000U;
  uint16_t days = d;
  for (uint8_t i = 1; i < m; ++i)
    days += pgm_read_byte(daysInMonth + i - 1);
  if (m > 2 && y % 4 == 0)
    ++days;
  return days + 365 * y + (y + 3) / 4 - 1;
}
static uint32_t time2ulong(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
  return ((days * 24UL + h) * 60 + m) * 60 + s;
}

time_t unixtime() //转换时间戳 只有ds1302操作时使用
{
  time_t t;
  uint16_t days = date2days(now1.year, now1.month, now1.day);
  t = time2ulong(days, now1.hour, now1.minute, now1.second);
  t += SECONDS_FROM_1970_TO_2000; // seconds from 1970 to 2000
  return t;
}

