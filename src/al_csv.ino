
#include "config.h"

#include <stdlib.h>


char savedata[48];
String revAline;
char dataTemplate[] = "%ld,%.2f,%.2f,%.4f,%.4f\n"; //信息模板

void getTempAndHumilocation() 
{
  #ifdef _SHT20
  sht20getTempAndHumi();
  tmsp=now_unixtime;//系统时间
   tp=currentTemp;//温度
   h=currentHumi;//湿度
  #else
  tp=f_get18b20Temp();//温度
   h=0;//湿度
   tmsp=now_unixtime;//系统时间
   #endif
   
   
   
  //  if(f_Flight_Mode==true)
  //  {
  //     e=0;//经纬度
  //     n=0;
  //   }
  //  else
  //  {
  //     e=locationE_;//经纬度
  //     n=locationN_;
  //  }

   snprintf(savedata, 48, dataTemplate, tmsp, tp, h, e, n); 
}
/****将savedata记录到文件系统****/
void saveDataToCSV(String file_add)
{
  File file = SPIFFS.open(file_add, FILE_APPEND);
  if (!file)
  {
    Serial.println("open file failed");
    return;
  }
  file.print(savedata);
  file.close();
  if(dbug)getAllDataFromCSV(file_add);
}
/***********读取CSV全部数据***********/
void getAllDataFromCSV(String file_add)
{
  File file = SPIFFS.open(file_add, FILE_READ);
  String rev = file.readString();
  Serial.println(rev);
  file.close();
}
//在文件系统中读取并删除第一条
void getADataFromCSV(String file_add)
{
  File file = SPIFFS.open(file_add, FILE_READ);
  String revAll = file.readString();
  // Serial.println("文件系统操作之间\n");
  // Serial.println(revAll);

  //Serial.println("正在寻找换行符......");
  int lineIndex = revAll.indexOf('\n');//找换行符
  //Serial.print("一行有字节数：");
  //Serial.println(lineIndex);//换行符在46
  revAline = revAll.substring(0, lineIndex);//读出lineIndex所在行
  Serial.println(revAline);
  file.close();
  revAll = revAll.substring(lineIndex+1);//删除读出的行

  //Serial.println("文件系统操作之后");//再读出RevALL
  //Serial.println(revAll);
  File wfile = SPIFFS.open(file_add, FILE_WRITE);//写入文件系统
  wfile.print(revAll);
}
void parseData(unsigned long* _tmsp,float* _tp,float* _h,float* _e,float* _n)//找到小数点的位置
{
  // Serial.print("读出一行内容：");
  // Serial.println(revAline);

  int count = 0;
  int splitsybol[4];
  //Serial.printf("本行字节长度:%d\n", revAline.length());
  for (size_t i = 0; i < revAline.length(); i++)
  {
    if (revAline[i] == ',')
    {
      splitsybol[count] = i;
      //Serial.printf("第%d个,在 ：%d\n", count, i);
      count++;
    }
  }
  *_tmsp = atol(revAline.substring(0, splitsybol[0]).c_str());//substring():功能是截取字符串
  *_tp = atof(revAline.substring(splitsybol[0] + 1, splitsybol[1]).c_str());//atof()在stdlib.h中。工能是将字符串转换为浮点数。
  *_h = atof(revAline.substring(splitsybol[1] + 1, splitsybol[2]).c_str());//c_str()函数返回一个指向正规C字符串的指针常量, 内容与本string串相同.
  *_e = atof(revAline.substring(splitsybol[2] + 1, splitsybol[3]).c_str());
  *_n = atof(revAline.substring(splitsybol[3] + 1, revAline.length()).c_str());
  // Serial.printf("时间戳：%ld\n", _tmsp);
  // Serial.printf("温度：%.2f\n", _tp);
  // Serial.printf("湿度：%.2f\n", _h);
  // Serial.printf("经度：%.4f\n", _e);
  // Serial.printf("纬度：%.4f\n", _n);
}

// void setup() {
//   Serial.begin(115200);
//   SPIFFS.begin();
//   File wfile = SPIFFS.open("/record.csv", FILE_WRITE);//写入文件系统
 
//   wfile.close();
//   for(int i=0;i<10;i++)//记录10组数
//   {
//      Serial.println("格式打印到内存\n");
//      get_temp_humi_location();//用它转换格式
//      Serial.println("添加到文件系统\n");
//      saveDataToCSV();
//   }
//   delay(100);
//   Serial.println("读出文件系统\n");
//   getAllDataFromCSV();//读出来看看
//   delay(100);
//   Serial.println("在文件系统中读取并删除第一条\n");
//   getADataFromCSV();
//   Serial.println("解析数据\n");
//   parseData();
 
// }

// void loop() {
//   // put your main code here, to run repeatedly:

// }

