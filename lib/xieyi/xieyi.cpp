#include "Arduino.h"
#include "xieyi.h"

 uint8_t CMD_NUM=0;//�����ú�������

 uint8_t rx_buff[num];
//清接收缓冲区
void clear_rx_buff(void)
{
  int i;
  for(i=0;i<num;i++) rx_buff[i]=0;
}
uint8_t exe(void)
{
    int res=0;
    switch(M_name_par.GS)
    {
    case 0://无参�?(void类型)
        res=(*(int(*)())M_name_par.pfune->pf)();
        break;
    case 1:
        res=(*(int(*)(uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0]);
        break;
    case 2://�?2�?参数
        res=(*(int(*)(uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1]);
        break;
    case 3://�?3�?参数
        res=(*(int(*)(uint32_t,uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1],M_name_par.cs[2]);
        break;
    case 4://�?4�?参数
        res=(*(int(*)(uint32_t,uint32_t,uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1],M_name_par.cs[2],M_name_par.cs[3]);
        break;
    case 5://�?5�?参数
        res=(*(int(*)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1],M_name_par.cs[2],M_name_par.cs[3],M_name_par.cs[4]);
        break;
    case 6://�?6�?参数
        res=(*(int(*)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1],M_name_par.cs[2],M_name_par.cs[3],M_name_par.cs[4],M_name_par.cs[5]);
        break;
    case 7://�?7�?参数
        res=(*(int(*)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1],M_name_par.cs[2],M_name_par.cs[3],M_name_par.cs[4],M_name_par.cs[5],M_name_par.cs[6]);
        break;
    case 8://�?8�?参数
        res=(*(int(*)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t))M_name_par.pfune->pf)(M_name_par.cs[0],M_name_par.cs[1],M_name_par.cs[2],M_name_par.cs[3],M_name_par.cs[4],M_name_par.cs[5],M_name_par.cs[6],M_name_par.cs[7]);
        break;
    }
    return res;
}

//��ȡ���ú���������λ��
//���룺���յ��Ĺ����� CMD ,
//�����������λ�� *p
//���أ�0�ɹ� ����0����
uint8_t get_fun_ID(uint8_t cmd,uint8_t *p)
{
    uint8_t i=0;
    for(i=0; i<CMD_NUM; i++)
    {
        if(cmd==name_tab[i].cmd[1])
        {
            *p=i;
            return 0;
        }
    }
    return 1;
}
//辨�?�参数格�?:�?数和类型
void get_fun_par_gslx()
{
     uint8_t i,j;
     uint8_t buf[CMD_data];
     uint8_t par[CMD_data]; //状态暂�?
    for(i=0; i<(CMD_data); i++) par[i]=0;
    j=2;//指令字节所在位�?
    for(i=0; i<(CMD_data); i++) //将纯参数放入数组
    {
        buf[i]=name_tab[M_name_par.ID].cmd[j];
        j++;
    }
#if 0
    printf("*****************************************\r\n");
    for(i=0; i<(CMD_data); i++) //显示串口接收�?
    {
        printf("buf[%d]=%d\r\n",i,buf[i]);
    }
#endif
    for(i=0; i<(CMD_data); i++) //辨�?�参�? pra
    {
        if(buf[i]!=0)  par[buf[i]-1]++;
    }
    j=0;
    for(i=0; i<(CMD_data); i++) //辨�?�参�? GS
    {
        if(par[i]!=0) j++;
    }
    M_name_par.GS=j;
    for(i=0; i<(CMD_data); i++) M_name_par.pra[i]=par[i];
#if 0
    printf("*****************************************\r\n");
    for(i=0; i<(CMD_data); i++) //显示串口接收�?
    {
        printf("par[%d]=%d\r\n",i,M_name_par.pra[i]);
    }
    printf("GS=%d\r\n",M_name_par.GS);
#endif
}

//��ȡָ���ʽ��ָ������������������͡���������
//���룺��ȡ���ĺ�����λ�� id
void get_fun_par(uint8_t id)
{
    M_name_par.pfune =&name_tab[id];//λ��
    M_name_par.ID=id;
    M_name_par.CMD=name_tab[id].cmd[1];
    //�������������ĸ���������
    get_fun_par_gslx();
}


//取数�?
uint32_t yiwei1(uint8_t *buf)
{
    uint32_t i;
    uint8_t j;
    i=buf[0];//将数�?存放

    for(j=0; j<(CMD_data-1); j++)//将�??一�?数据移除�?
    {
        buf[j]=buf[j+1];
    }
    return i;
}
//取uint16_t数据
uint32_t yiwei2(uint8_t*buf)
{
    uint32_t i;
    uint8_t j;
    uint8_t k=2;
    i=((uint16_t)buf[0]<<8)+buf[1];


    for(j=0; j<(CMD_data-2); j++)
    {
        buf[j]=buf[k];//�?二�?��?�载BUF,
        k++;
    }
    return i;

}
//取uint32_t数据
uint32_t yiwei4(uint8_t *buf)
{
    uint32_t i;
    uint8_t j;
    uint8_t k=4;
    i=((uint32_t)buf[0]<<24)+((uint32_t)buf[1]<<16)+((uint32_t)buf[2]<<8)+buf[3];

    for(j=0; j<(CMD_data-4); j++)
    {
        buf[j]=buf[k];;//�?二�?��?�载BUF,
        k++;
    }

    return i;
}



void get_uart_par(uint8_t *buf)
{
     uint8_t i;
     uint8_t cbuf[CMD_data];

    for(i=0; i<(CMD_data); i++)
    {
        cbuf[i]=buf[i];
    }
#if 0
    printf("*****************************************\r\n");
    for(i=0; i<(CMD_data); i++) //显示串口接收�?
    {
        printf("cbuf[%d]=%d\r\n",i,cbuf[i]);
    }
#endif
    //参数�?有限制的
    for(i=0; i<CMD_data; i++)
    {
        if(M_name_par.pra[i]!=0)
        {
            switch(M_name_par.pra[i])
            {
            case 1://
                M_name_par.cs[i]= yiwei1(cbuf);
                break;
            case 2://uint16_t
                M_name_par.cs[i]= yiwei2(cbuf);
                break;
            case 4://uint32_t
                M_name_par.cs[i]=yiwei4(cbuf);
                break;
            }
        }
    }

#if 0
    printf("*****************************************\r\n");
    for(i=0; i<(CMD_data); i++) //显示串口接收�?
    {
        printf("cs[%d]=%d\r\n",i,M_name_par.cs[i]);
    }
#endif
}

uint8_t jiexi(uint8_t *buf,int x)
{
  uint8_t res=0,id=0;
  //�˶�AA BB
  if(rx_buff[0]==0xaa&&rx_buff[x-1]==0xbb)
  {
       #if 0
       Serial.println("ָ���ȡAABB��ȷ\n");
       #endif

       
       res=get_fun_ID(rx_buff[1],&id);
       if(!res)
       {
           //Serial.printf("��ȡID:%d\n",id);
                get_fun_par(id);////获取函数表中的格�? 指令 参数�?数，参数类型 返回类型
                get_uart_par(&rx_buff[2]);
                clear_rx_buff();
                exe();
                return 0;
       }
       else Serial.println("�޴�ָ�");
  }
  else
  {
     Serial.println("ָ��û��ȡ��AABB!\n");
  }
  return 1;
}





//aa 00 a0 01 01 01 01 01 01 01 bb
void xieyi_scan(void)
{
    static uint8_t f=0;
    static int i= 0,J=0;
   // uint8_t sta=0;//״̬
    if(J==0)
	 {
		  J=1;
		  init_xieyi(&CMD_NUM);
		  #if DEBUG
		  printf("NUM=%d\r\n",CMD_NUM);
		  #endif
	 }
    //����ͨѶ���ݣ��ҵ�ָ��ȣ������ݳ���
    while(Serial.available())
   {
      rx_buff[i++] =Serial.read(); 
      f=1;    
   }
   if(f==1)
   {
      f=0;


      
      #if DEBUG
        //�鿴���յ�����û��֤
      Serial.println("���յ����ݣ�");
      Serial.println();
    //   for(i=0;i<x;i++)
    //   Serial.write(rx_buff[i]);
      Serial.println();
      Serial.println();
      uint8_t I_CMDlong=i;//ָ���
      uint8_t I_CMDdata =i-3;//���ݳ���
      uint8_t CMD=rx_buff[1];//��ָ�����
      Serial.printf("ͨѶָ�%d�ֽڣ���������%d�ֽڣ���ָ�����%d\r\n",I_CMDlong,I_CMDdata,CMD);
      #endif




      if(i==CMD_long)//�ȳ�ָ��
      {
              //����ָ��
        jiexi(rx_buff,i);
      
      }
      else Serial.println("ָ��ȸ�ʽ����");
      
 
   }
   i=0;
  





	//   if(i==0)
	//  {
	// 	  i=1;
	// 	  init_xieyi(&CMD_NUM);
	// 	  #if 0
	// 	  printf("NUM=%d\r\n",CMD_NUM);
	// 	  #endif
	//  }
//     if(USART1_RX_STA&0x8000)
//     {
//         if((USART1_rx_buff[0]==0xaa)&&(USART1_rx_buff[CMD_long-1]==0xbb))
//         {
//             res=get_fun_ID(USART1_rx_buff[1],&id);//将串口的数据解析成函数表�?的哪�?函数ID
//             if(!res)
//             {
//                 get_fun_par(id);////获取函数表中的格�? 指令 参数�?数，参数类型 返回类型
//                 get_uart_par(&USART1_rx_buff[2]);
//                 exe();
//             }
// #if DEBUG
//             else printf("无�?�指令！！！\r\n");
// #endif
// #if 0
//             printf("*****************************************\r\n");
// 						if(!res)//显示指令状�?
//                 printf("读取指令成功�?,读取的指�?=%d,串口指令�?=%d,状态：OK!!\r\n",id,USART1_rx_buff[1]);
//             else printf("读取指令失败�?,读取的指�?=%d,cmd=%d,err!!\r\n",id,USART1_rx_buff[1]);
//             for(id=0; id<CMD_long; id++) //显示串口接收�?
//             {
//                 printf("buf[%d]=%d\r\n",id,USART1_rx_buff[id]);
//             }

// #endif
//             USART1_rx_buff[0]=0;
//              USART1_rx_buff[CMD_long-1]=0;
//         }
// #if DEBUG
//         else printf("格式错�??！\r\n");
// #endif
//         USART1_RX_STA=0;
//     }
}




// void jiexi(char *buf,int x)
// {
//   uint8_t i,res,id;
//   //显示传送过来的内�??
//   #if 1
//      for(i=0;i<x;i++)
//       Serial.write(rx_buff[i]);
//       Serial.println();
//   #endif
//   //判断AA BB
//   if(rx_buff[0]==0xaa&&rx_buff[x-1]==0xbb)
//   {
//        Serial.println("CMD ok!");
//        //判断功能�?
//        res=get_fun_ID(rx_buff[1],&id);
    
//        //判断参数�?�?
//        //执�?�命令操�?
//   }
//   else
//   {
//      Serial.println("CMD ERR!");

//   }
// }


// void xieyi(void)
// {
//    static uint8_t f=0;
//    static int i= 0;
//    while(Serial.available())
//    {
//       rx_buff[i++] =Serial.read(); 
//       f=1;    
//    }
//    if(f==1)
//    {
//       f=0;
//       jiexi(rx_buff,i);
//    }
//    i=0;
// }






















































