 #include "xieyi.h"
 #include "config.h"
 //1.引用外部函数 
void set_dbug(uint8_t a);//调试参数
void SET_Sending_interval(time_t t);//设置GSM发送间隔：（S）
void SET_Last_span_Sleep_span(int x,int y);//设置亮屏时间和息屏到休眠时间
void set_fxmode(char ,char,char);//设定飞航模式 1飞航 2工作

void FFS_fromat();//格式化文件系统
void sys_time();//核对系统时间
void SET_ds1302(int year, char momtch, char day, char hour, char minute, char second);//设置实时时钟
void f_800c_power_switch(int a);//800c供电
void f_get_csv(int x);//读取文件系统CSV

//2.填写指令参数格式
  NAME_TAB name_tab[]=
 {
	  (void*)set_dbug,                                        {0xaa,0xdb, 1, 0, 0, 0, 0, 0,0,0, 0xbb}, 
	  (void*)SET_Sending_interval,                            {0xaa,0x00, 1, 1, 1, 1, 0, 0,0,0, 0xbb},
      (void*)SET_Last_span_Sleep_span,                        {0xaa,0x01, 1, 1, 1, 1, 2, 2,2,2, 0xbb},
	  (void*)f_get_csv,                                       {0xaa,0x02, 1, 0, 0, 0, 0, 0,0,0, 0xbb},
	  	
	  (void*)set_fxmode,                                      {0xaa,0x03, 1, 2, 3, 0, 0, 0,0,0, 0xbb},
      
	  (void*)FFS_fromat,                                      {0xaa,0x08, 0, 0, 0, 0, 0, 0,0,0, 0xbb},
	  (void*)sys_time,                                        {0xaa,0x0a, 0, 0, 0, 0, 0, 0,0,0, 0xbb}, 
	  (void*)SET_ds1302,                                      {0xaa,0x0e, 1, 1, 2, 3, 4, 5,6,0, 0xbb},
	  (void*)f_800c_power_switch,                             {0xaa,0xf0, 1, 0, 0, 0, 0, 0,0,0, 0xbb},
	  (void*)f_get_csv,                                       {0xaa,0xf1, 1, 0, 0, 0, 0, 0,0,0, 0xbb},	 
 };
//3.根据函数表修改xieyi.h的tab
//4.在主程序中调用xieyi_scan();






















 
  void init_xieyi(uint8_t *p)
 {
     *p = (sizeof(name_tab)/sizeof(name_tab[0]));
 } 
 
NAME_par  M_name_par =
{
	 name_tab,
	 exe,
	 0,//ID
	 0,//CMD
	 0,// uint8_t GS;
	 0,//uint8_t pra1;
	 0,//uint8_t cs2;
};


