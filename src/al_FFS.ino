#include "config.h"
//初始化
void alFFS_init()
{
  if (firstBootFlag)
    Serial.printf("format FFS:%d", SPIFFS.format());
  else
    Serial.printf("FFS size:%d,used:%d\r\n", SPIFFS.totalBytes(), SPIFFS.usedBytes());
}

//读文件列表
void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\r\n", dirname);
  //1.打开“/”开头文件
  File root = fs.open(dirname);
  //2.检查是否打开
  if (!root)
  {
    Serial.println("- failed to open directory");
    return;
  }
  //3.检查是否有“/”开头文件
  if (!root.isDirectory())
  {
    Serial.println(" - not a directory");
    return;
  }
  //4.有“/”开头的文件，再创建一个新对象打开下一个文件
  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

//写文件 用法： writeFile(SPIFFS, "/hello.txt", "Hello ");
void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\r\n", path);
  //1.输入文件名打开文件，写
  File file = fs.open(path, FILE_WRITE);
  //2.检查打开成功了吗？
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  //3.打开成功写信息
  if (file.print(message))
  {
    Serial.println("- file written");
  }
  //4.写失败返回信息
  else
  {
    Serial.println("- write failed");
  }
  //5.关闭文件
  file.close();
}
//文件系统添加文件 用法：appendFile(SPIFFS, "/hello.txt", "World!\r\n");
void appendFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\r\n", path);
  //1.输入文件名打开文件，添加文件
  File file = fs.open(path, FILE_APPEND);
  //2.检查打开成功了吗？
  if (!file)
  {
    Serial.println("- failed to open file for appending");
    return;
  }
  //3.打开成功添加信息
  if (file.print(message))
  {
    Serial.println("- message appended");
  }
  //4.添加失败返回信息
  else
  {
    Serial.println("- append failed");
  }
  //5.关闭文件
  file.close();
}
/*************************************list文件 json*************************************/
void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}























































































