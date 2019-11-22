/*******************************************************************************
* 实验名			   : 万年历实验
* 使用的IO	     : 
* 实验效果       :1602显示时钟，按K3进入时钟设置，按K1选择设置的时分秒日月，按K2选择
*选择设置加1。 
*	注意					 ：
*******************************************************************************/

#include<reg51.h>
#include"lcd.h"
#include"ds1302.h"
#include "oled.h"
#include"temp.h"
#include"i2c.h"

sbit key1=P1^3;
sbit key2=P1^6;
sbit key3=P1^5;
sbit key4=P1^4;
sbit key5=P1^7;

sbit k1=P2^4;
sbit k2=P2^5;
sbit k3=P2^6;
sbit k4=P2^7;
sbit dp=P0^7;
extern unsigned char code shuzi[][36];
extern unsigned char code yangshuzi[][36];
extern unsigned char code xinqi[][32];
extern unsigned char code yangxinqi[][32];
void Int0Configuration();
void LcdDisplay();
unsigned char SetState,SetPlace;
void Delay10ms(void);   //误差 0us
u8 DisplayData[8];
u8 code smgduan[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
					0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};//显示0~F的
unsigned char  m1=0;
unsigned char  m2=0;
unsigned char  m3=0;
unsigned char  m4=0;
unsigned char  m5=0;


unsigned char  q1=1;
unsigned char  q2=1;
unsigned char  q3=1;
 char  q4=0;
unsigned char  q5=1;
unsigned char	e1=0;
 char	e2=0;
unsigned char	a1=0;
char	a2=0;
int s1=0;
int s2=0;
int s3=0;
sbit beep=P2^3;
 uchar TIME1[7] = {0, 0, 0x12, 0x01, 0x01, 0x02, 0x13};
uchar TIME2[7] = {0, 0, 0x12, 0x01, 0x01, 0x02, 0x13};
uchar TIME3[7] = {0, 0, 0x12, 0x01, 0x01, 0x02, 0x13};
uchar TIME4[7] = {0, 0, 0x12, 0x01, 0x01, 0x02, 0x13};
uchar TIME5[7] = {0, 0, 0x12, 0x01, 0x01, 0x02, 0x13};
uchar TIME6[7] = {0, 0, 0x12, 0x01, 0x01, 0x02, 0x13};
 
/*******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
*******************************************************************************/
void delay(int i)
{
	while(i--);	
}
/*******************************************************************************
* 函 数 名         : DigDisplay
* 函数功能		   : 数码管动态扫描函数，循环扫描8个数码管显示
*******************************************************************************/
void DigDisplay()
{
	u8 i;
	for(i=0;i<4;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
		
			case(0):
				k1=0;k2=1;k3=1;k4=1; break;//显示第1位
			case(1):
				k1=1;k2=0;k3=1;k4=1; break;//显示第2位
			case(2):
				k1=1;k2=1;k3=0;k4=1; break;//显示第3位
			case(3):
				k1=1;k2=1;k3=1;k4=0; break;//显示第4位
				
		}
		P0=DisplayData[i];//发送段码
		delay(500); //间隔一段时间扫描	
		P0=0x00;//消隐
	}
}
void Timer0Init()
{
	TMOD|=0X01;//选择为定时器0模式，工作方式1，仅用TR0打开启动。

	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;	
	ET0=1;//打开定时器0中断允许
	EA=1;//打开总中断
	TR0=1;//打开定时器			
}
void oLedDisplay(int temp) 	 //lcd显示
{
    
  	unsigned char i=0, datas[] = {5, 4, 6, 6, 6}; //定义数组
	float tp;  
	if(temp< 0)				//当温度值为负数
  	{
		
	  OLED_ShowChar(90,0,'-',16);	//显示负
		//因为读取的温度是实际温度的补码，所以减1，再取反求出原码
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	
		//留两个小数点就*100，+0.5是四舍五入，因为C语言浮点数转换为整型的时候把小数点
		//后面的数自动去掉，不管是否大于0.5，而+0.5之后大于0.5的就是进1了，小于0.5的就
		//算由�0.5，还是在小数点后面。
 
  	}
 	else
  	{			
	  	
	    OLED_ShowChar(90,0,'+',16); 		//显示正
		
		tp=temp;//因为数据处理有小数点所以将温度赋给一个浮点型变量
		//如果温度是正的那么，那么正数的原码就是补码它本身
		temp=tp*0.0625*100+0.5;	
		//留两个小数点就*100，+0.5是四舍五入，因为C语言浮点数转换为整型的时候把小数点
		//后面的数自动去掉，不管是否大于0.5，而+0.5之后大于0.5的就是进1了，小于0.5的就
		//算加上0.5，还是在小数点后面。
	}
	datas[0] = temp / 10000;
	datas[1] = temp % 10000 / 1000;
	datas[2] = temp % 1000 / 100;
	datas[3] = temp % 100 / 10;
	datas[4] = temp % 10;


	
	 OLED_ShowNum(99,0,datas[1],1,16); //十位
	OLED_ShowNum(108,0,datas[2],1,16); //十位
	OLED_ShowChar(117,0,'.',16);
	OLED_ShowNum(120,0,datas[3],1,16); //十位
	
 
	 
}
void Timer1Init()
{
	TMOD|=0X10;//选择为定时器1模式，工作方式1，仅用TR1打开启动。

	TH1=0XFC;	//给定时器赋初值，定时1ms
	TL1=0X18;	
	ET1=1;//打开定时器1中断允许
	EA=1;//打开总中断
	TR1=1;//打开定时器			
}

/*******************************************************************************
* 函数名         : main
* 函数功能		   : 主函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/

void main()
{
	
	Int0Configuration();
    OLED_Init();			//初始化OLED  
	OLED_Clear();
//	Ds1302Init();
	Timer0Init();
	Timer1Init();
   Ds18b20Init();
  
   
	while(1)
{  
	  if(key5==0)
	 {
	 m5++;
	  if(m5>1)m5=0;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 } 		
   Ds1302ReadTime();
  
    //OLED_ShowNum(0,1,+TIME[0]/16,1,32);
	 // OLED_ShowNum(16,1,(TIME[0]&0x0f),1,32);
	  OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);
	  
	 
	  
	  OLED_ShowNum(0,0,2,1,16);


	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);


	  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);

	    OLED_DrawBMP(0,6,16,8,xinqi[9]);
  		 OLED_DrawBMP(17,6,33,8,xinqi[10]);

		 oLedDisplay(Ds18b20ReadTemp());
		  if(m3==30){	OLED_Clear();m3=20;}
 if(((TIME1[1]/16)==(TIME[1]/16))&&((TIME1[1]&0x0f)==(TIME[1]&0x0f))&&((TIME1[0]/16)==(TIME[0]/16))&&((TIME1[0]&0x0f)==(TIME[0]&0x0f))&&((TIME1[2]/16)==(TIME[2]/16))&&((TIME1[2]&0x0f)==(TIME[2]&0x0f)))
	  {s2=5;}
	  if(((TIME2[1]/16)==(TIME[1]/16))&&((TIME2[1]&0x0f)==(TIME[1]&0x0f))&&((TIME2[0]/16)==(TIME[0]/16))&&((TIME2[0]&0x0f)==(TIME[0]&0x0f))&&((TIME2[2]/16)==(TIME[2]/16))&&((TIME2[2]&0x0f)==(TIME[2]&0x0f)))
	  {s2=5;}
	  if(((TIME3[1]/16)==(TIME[1]/16))&&((TIME3[1]&0x0f)==(TIME[1]&0x0f))&&((TIME3[0]/16)==(TIME[0]/16))&&((TIME3[0]&0x0f)==(TIME[0]&0x0f))&&((TIME3[2]/16)==(TIME[2]/16))&&((TIME3[2]&0x0f)==(TIME[2]&0x0f)))
	  {s2=5;}
	  if(((TIME4[1]/16)==(TIME[1]/16))&&((TIME4[1]&0x0f)==(TIME[1]&0x0f))&&((TIME4[0]/16)==(TIME[0]/16))&&((TIME4[0]&0x0f)==(TIME[0]&0x0f))&&((TIME4[2]/16)==(TIME[2]/16))&&((TIME4[2]&0x0f)==(TIME[2]&0x0f)))
	  {s2=5;}
	  if(((TIME5[1]/16)==(TIME[1]/16))&&((TIME5[1]&0x0f)==(TIME[1]&0x0f))&&((TIME5[0]/16)==(TIME[0]/16))&&((TIME5[0]&0x0f)==(TIME[0]&0x0f))&&((TIME5[2]/16)==(TIME[2]/16))&&((TIME5[2]&0x0f)==(TIME[2]&0x0f)))
	  {s2=5;}
	  if(((TIME6[1]/16)==(TIME[1]/16))&&((TIME6[1]&0x0f)==(TIME[1]&0x0f))&&((TIME6[0]/16)==(TIME[0]/16))&&((TIME6[0]&0x0f)==(TIME[0]&0x0f))&&((TIME6[2]/16)==(TIME[2]/16))&&((TIME6[2]&0x0f)==(TIME[2]&0x0f)))
	  {s2=5;}
	///////按键///////////////////////////////////
	
	if(m5==1&&q1==1)
	{ 

	 while(((m1==1)||(m2==1)||(m4==1)||(m5==1))&&(q1==1))
	 {

	  if(key5==0)
	 {
	 m5++;
	  if(m5>1)m5=0;
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
	 


		  Ds1302ReadTime();
  	if(key3==0)
	 {
	 m3=5;Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 } 
		 
    //OLED_ShowNum(0,1,+TIME[0]/16,1,32);
	 // OLED_ShowNum(16,1,(TIME[0]&0x0f),1,32);
	  OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);
	  
	 
	  
	  OLED_ShowNum(0,0,2,1,16);


	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);


	  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);

	    OLED_DrawBMP(0,6,16,8,yangxinqi[7]); //菜
  		 OLED_DrawBMP(17,6,33,8,yangxinqi[8]);	 //单

		 oLedDisplay(Ds18b20ReadTemp());
		 

		 if(m3==5)
		 {	  m4=0;m5=0;
		 	OLED_Clear();
		  	
		   while(m3==5)
		   {
			
			    OLED_DrawBMP(32,1,48,3,yangxinqi[12]);  //	阳更改时间
				OLED_DrawBMP(49,1,67,3,yangxinqi[13]);
				OLED_DrawBMP(68,1,85,3,yangxinqi[14]);
				OLED_DrawBMP(86,1,102,3,yangxinqi[15]);
				
				OLED_DrawBMP(32,4,48,6,yangxinqi[4]); 	 //	 阴设置闹钟
				OLED_DrawBMP(49,4,67,6,yangxinqi[5]);
				OLED_DrawBMP(68,4,85,6,yangxinqi[6]);
				OLED_DrawBMP(86,4,102,6,yangxinqi[9]);

				
	          OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);
		 if(key3==0)
	 {
	 q2=2;q5=1;
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }	
		  if(key4==0)
	 {
	 m4++;Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	 m5++;Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }
	 
	  if(m4>3)m4=0;  if(m5>3)m5=0;
	  if(q5==0&&q2==0) {	OLED_Clear();q5=10;q2=10;}
		if(q2==2)
		{	OLED_Clear();
		while(q2==2)
		{				 OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);

		//按键12扫描
		if(key1==0)
	 {
	 q4--; if(q4<0)q4=13;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
			if(key2==0)
	 {
	 q4++;	   if(q4>12)q4=0;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
		
		   while(q5==1)
		   {
				  	if(key1==0)
	 {
	 q4--; if(q4<0)q4=13;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
			if(key2==0)
	 {
	 q4++;	   if(q4>13)q4=0;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
		         switch(q4)	{
				 
				 	  case(0):	  OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);
	  
		  if(key4==0)
	 {
	 TIME[6]= TIME[6]+16;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[6]= TIME[6]-16;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  //OLED_ShowNum(17,0,'!',1,16);
	  OLED_ShowChar(17,0,'*',16);
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);break;
					  case(1): OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);

			 if(key4==0)
	 {
	 TIME[6]= TIME[6]+1;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[6]= TIME[6]-1;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  //OLED_ShowNum(26,0,'!',1,16);
		OLED_ShowChar(26,0,'*',16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);break;
					  case(2):	OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);

				   if(key4==0)
	 {
	 TIME[4]= TIME[4]+16;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[4]= TIME[4]-16;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		 OLED_ShowChar(44,0,'*',16);
		//OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);	break;
					  case(3):	 OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);
				  	   if(key4==0)
	 {
	 TIME[4]= TIME[4]+1;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[4]= TIME[4]-1;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		 OLED_ShowChar(53,0,'*',16);
	//	OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);break;
					  case(4):	OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);
					    if(key4==0)
	 {
	 TIME[3]= TIME[3]+16;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[3]= TIME[3]-16;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		 OLED_ShowChar(71,0,'*',16);
	//	OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);	break;
					  case(5):	OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);
						  if(key4==0)
	 {
	 TIME[3]= TIME[3]+1;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[3]= TIME[3]-1;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		 OLED_ShowChar(80,0,'*',16);
	/*	OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);*/
	  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);break;
					  case(6):	OLED_DrawBMP(9,2,21,5,yangshuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);

					  if(key4==0)
	 {
	 TIME[2]= TIME[2]+16;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[2]= TIME[2]-16;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);	break;
					  case(7):	OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,yangshuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);
					 	  if(key4==0)
	 {
	 TIME[2]= TIME[2]+1;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[2]= TIME[2]-1;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);	break;
					  case(8):	OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,yangshuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);
				  	  if(key4==0)
	 {
	 TIME[1]= TIME[1]+16;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[1]= TIME[1]-16;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);	
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]); break;
					  case(9):	OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,yangshuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);

				  	  if(key4==0)
	 {
	 TIME[1]= TIME[1]+1;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[1]= TIME[1]-1;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }

	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);	break;
				 	  case(10):	OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,yangshuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);
			  	  if(key4==0)
	 {
	 TIME[0]= TIME[0]+16;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[0]= TIME[0]-16;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);	
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);	break;
					  case(11):	OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,yangshuzi[TIME[0]&0x0f]);
				 	  if(key4==0)
	 {
	 TIME[0]= TIME[0]+1;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[0]= TIME[0]-1;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);	 break;
					  case(13):	OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);
	  if(key3==0){q2=0;q5=0;}


	   OLED_DrawBMP(0,6,16,8,yangxinqi[16]); //阳返回
  		          OLED_DrawBMP(17,6,33,8,yangxinqi[17]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);	
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);   break;
		  case(12):	OLED_DrawBMP(9,2,21,5,shuzi[TIME[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME[0]&0x0f]);
	  if(key4==0)
	 {
	 TIME[5]= TIME[5]+1;Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME[5]= TIME[5]-1;	Ds1302Init();	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }




	  OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	OLED_ShowNum(0,0,2,1,16);		   
	  OLED_ShowNum(8,0,0,1,16);
	  OLED_ShowNum(17,0,(TIME[6]/16),1,16);
	
	  OLED_ShowNum(26,0,(TIME[6]&0x0f),1,16);
	    OLED_ShowChar(35,0,'/',16);
		OLED_ShowNum(44,0,(TIME[4]/16),1,16);
		OLED_ShowNum(53,0,(TIME[4]&0x0f),1,16);
		OLED_ShowChar(62,0,'/',16);
		OLED_ShowNum(71,0,(TIME[3]/16),1,16);
		OLED_ShowNum(80,0,(TIME[3]&0x0f),1,16);	
		  OLED_DrawBMP(111,6,127,8,xinqi[TIME[5]&0x07]);
	  OLED_DrawBMP(77,6,93,8,xinqi[0]);
 	   OLED_DrawBMP(94,6,110,8,xinqi[8]);   break;			 
					  
				 
				 
		}		 
				 
				 
				 
				 
				 
				 
				 }
		
		}

		 }

		

		      while((((m4==1)&&(m5==0))||((m4==0)&&(m5==2)))&&(m4!=3)&&(m5!=3)&&(m3==5))
			  {	 
			  
			  
			   OLED_DrawBMP(32,4,48,6,yangxinqi[4]); 	 //	 阴设置闹钟
				OLED_DrawBMP(49,4,67,6,yangxinqi[5]);
				OLED_DrawBMP(68,4,85,6,yangxinqi[6]);
				OLED_DrawBMP(86,4,102,6,yangxinqi[9]);

			  	 OLED_DrawBMP(32,1,48,3,yangxinqi[0]); //阴更改时间
				OLED_DrawBMP(49,1,67,3,yangxinqi[1]);
				OLED_DrawBMP(68,1,85,3,yangxinqi[2]);
				OLED_DrawBMP(86,1,102,3,yangxinqi[3]);

			  	 OLED_DrawBMP(0,6,16,8,yangxinqi[16]); //阳返回
  		          OLED_DrawBMP(17,6,33,8,yangxinqi[17]);



				  if(key3==0)
	 {	m3=30;
	 		 }	
				   if(key4==0)
	 {
	 m4++;Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	 m5++;Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 } 	 
		if(m3==2)q1=1;
		
		
			   if(m4>3)m4=0;  if(m5>3)m5=0;
			  }
			  



			  	while((((m4==0)&&(m5==1))||((m4==2)&&(m5==0)))&&(m5!=3)&&(m4!=3) )
				{
				 	  OLED_DrawBMP(32,4,48,6,yangxinqi[18]); 	 //	 阴设置闹钟
				OLED_DrawBMP(49,4,67,6,yangxinqi[19]);
				OLED_DrawBMP(68,4,85,6,yangxinqi[20]);
				OLED_DrawBMP(86,4,102,6,yangxinqi[21]);
				  
			    OLED_DrawBMP(32,1,48,3,yangxinqi[0]); 	//阴更改时间
				OLED_DrawBMP(49,1,67,3,yangxinqi[1]);
				OLED_DrawBMP(68,1,85,3,yangxinqi[2]);
				OLED_DrawBMP(86,1,102,3,yangxinqi[3]);
				 OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		        OLED_DrawBMP(17,6,33,8,yangxinqi[11]);
				if(key3==0)
	 {
	 e1=1;;Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }	
				 if(key4==0)
	 {
	 m4++;Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	 m5++;Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 } 
				if(m4>3)m4=0;  if(m5>3)m5=0;
				   if(e1==5){	OLED_Clear();e1=10;}
				 if(e1==1)
				 {		 	OLED_Clear();
							OLED_DrawBMP(9,0,21,3,shuzi[0]);
  	                     OLED_DrawBMP(22,0,34,3,shuzi[1]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,0,60,3,shuzi[0]);
  	                       OLED_DrawBMP(61,0,73,3,shuzi[2]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,0,99,3,shuzi[0]);
  	                          OLED_DrawBMP(100,0,112,3,shuzi[3]);

							  	OLED_DrawBMP(9,3,21,6,shuzi[0]);
  	                     OLED_DrawBMP(22,3,34,6,shuzi[4]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,3,60,6,shuzi[0]);
  	                       OLED_DrawBMP(61,3,73,6,shuzi[5]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,3,99,6,shuzi[0]);
  	                          OLED_DrawBMP(100,3,112,6,shuzi[6]);
				 
							    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		        OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

				 		 while(e1==1)
				 		  {		  ////12按键扫描
						  if(key1==0)
	 {
	 e2--; if(e2<0)e2=6;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
			if(key2==0)
	 {
	 e2++;	   if(e2>6)e2=0;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描

								  /////横向选择
								switch(e2){
								
								  case(0):
								  			  	OLED_DrawBMP(9,0,21,3,yangshuzi[0]);
  	                     OLED_DrawBMP(22,0,34,3,yangshuzi[1]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,0,60,3,shuzi[0]);
  	                       OLED_DrawBMP(61,0,73,3,shuzi[2]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,0,99,3,shuzi[0]);
  	                          OLED_DrawBMP(100,0,112,3,shuzi[3]);
									 if(key3==0)
	 {	a1=1;
	 		 }					
			 				if(a1==5){ OLED_Clear();a1=10;}
			 
			 	if(a1==1){	 OLED_Clear();
											   	 OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	

										  while(a1==1)
										  {
										    ////12按键扫描
						  if(key1==0)
	 {
	 a2--; if(a2<0)a2=6;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
			if(key2==0)
	 {
	 a2++;	   if(a2>6)a2=0;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
										  	 switch(a2)	{


											 case(0):OLED_DrawBMP(9,2,21,5,yangshuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);
						 if(key4==0)
	 {
	 TIME1[2]= TIME1[2]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[2]= TIME1[2]-16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(1):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,yangshuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);
									  if(key4==0)
	 {
	 TIME1[2]= TIME1[2]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[2]= TIME1[2]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(2):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,yangshuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);
										 if(key4==0)
	 {
	 TIME1[1]= TIME1[1]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[1]= TIME1[1]-16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(3):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,yangshuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);

						 if(key4==0)
	 {
	 TIME1[1]= TIME1[1]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[1]= TIME1[1]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(4):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,yangshuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);

						 if(key4==0)
	 {
	 TIME1[0]= TIME1[0]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[0]= TIME1[0]-16;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(5):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,yangshuzi[TIME1[0]&0x0f]);
								   	 if(key4==0)
	 {
	 TIME1[0]= TIME1[0]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[0]= TIME1[0]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(6):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);
						 				   
				  if(key3==0)
	 {	a1=5;
	 		 }	

	      OLED_DrawBMP(0,6,16,8,yangxinqi[16]); //阳返回
  		          OLED_DrawBMP(17,6,33,8,yangxinqi[17]);
										  
										   }
										  }
			 							  }
			 
			 
			 
			 
			 
							  	OLED_DrawBMP(9,3,21,6,shuzi[0]);
  	                     OLED_DrawBMP(22,3,34,6,shuzi[4]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,3,60,6,shuzi[0]);
  	                       OLED_DrawBMP(61,3,73,6,shuzi[5]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,3,99,6,shuzi[0]);
  	                          OLED_DrawBMP(100,3,112,6,shuzi[6]);
				 
							    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		        OLED_DrawBMP(17,6,33,8,yangxinqi[11]);
								  
								  			 
								  
								    	   break;
								   case(1):  	OLED_DrawBMP(9,0,21,3,shuzi[0]);
  	                     OLED_DrawBMP(22,0,34,3,shuzi[1]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,0,60,3,yangshuzi[0]);
  	                       OLED_DrawBMP(61,0,73,3,yangshuzi[2]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,0,99,3,shuzi[0]);
  	                          OLED_DrawBMP(100,0,112,3,shuzi[3]);
															  if(key3==0)
	 {	a1=1;
	 		 }	
			 	if(a1==5){ OLED_Clear();a1=10;}
			 
			 	if(a1==1){	 OLED_Clear();
											   	 OLED_DrawBMP(9,2,21,5,shuzi[TIME2[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME2[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME2[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME2[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME2[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME2[0]&0x0f]);


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	

										  while(a1==1)
										  {
										    ////12按键扫描
						  if(key1==0)
	 {
	 a2--; if(a2<0)a2=6;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
			if(key2==0)
	 {
	 a2++;	   if(a2>6)a2=0;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
										  	 switch(a2)	{


											 case(0):OLED_DrawBMP(9,2,21,5,yangshuzi[TIME2[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME2[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME2[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME2[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME2[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME2[0]&0x0f]);
						 if(key4==0)
	 {
	 TIME2[2]= TIME2[2]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME2[2]= TIME2[2]-16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(1):OLED_DrawBMP(9,2,21,5,shuzi[TIME2[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,yangshuzi[TIME2[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME2[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME2[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME2[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME2[0]&0x0f]);
									  if(key4==0)
	 {
	 TIME2[2]= TIME2[2]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME2[2]= TIME2[2]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(2):OLED_DrawBMP(9,2,21,5,shuzi[TIME2[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME2[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,yangshuzi[TIME2[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME2[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME2[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME2[0]&0x0f]);
										 if(key4==0)
	 {
	 TIME2[1]= TIME2[1]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME2[1]= TIME2[1]-16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(3):OLED_DrawBMP(9,2,21,5,shuzi[TIME2[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME2[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME2[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,yangshuzi[TIME2[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME2[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME2[0]&0x0f]);

						 if(key4==0)
	 {
	 TIME2[1]= TIME2[1]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME2[1]= TIME2[1]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(4):OLED_DrawBMP(9,2,21,5,shuzi[TIME2[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME2[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME2[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME2[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,yangshuzi[TIME2[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME2[0]&0x0f]);

						 if(key4==0)
	 {
	 TIME2[0]= TIME2[0]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME2[0]= TIME2[0]-16;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(5):OLED_DrawBMP(9,2,21,5,shuzi[TIME2[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME2[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME2[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME2[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME2[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,yangshuzi[TIME2[0]&0x0f]);
								   	 if(key4==0)
	 {
	 TIME2[0]= TIME2[0]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME2[0]= TIME2[0]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(6):OLED_DrawBMP(9,2,21,5,shuzi[TIME2[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME2[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME2[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME2[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME2[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME2[0]&0x0f]);
						 				   
				  if(key3==0)
	 {	a1=5;
	 		 }	

	      OLED_DrawBMP(0,6,16,8,yangxinqi[16]); //阳返回
  		          OLED_DrawBMP(17,6,33,8,yangxinqi[17]);
										  
										   }
										  }
			 							  }
			 
							  	OLED_DrawBMP(9,3,21,6,shuzi[0]);
  	                     OLED_DrawBMP(22,3,34,6,shuzi[4]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,3,60,6,shuzi[0]);
  	                       OLED_DrawBMP(61,3,73,6,shuzi[5]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,3,99,6,shuzi[0]);
  	                          OLED_DrawBMP(100,3,112,6,shuzi[6]);
				 
							    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		        OLED_DrawBMP(17,6,33,8,yangxinqi[11]);
								      break;
								   case(2):  	OLED_DrawBMP(9,0,21,3,shuzi[0]);
  	                     OLED_DrawBMP(22,0,34,3,shuzi[1]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,0,60,3,shuzi[0]);
  	                       OLED_DrawBMP(61,0,73,3,shuzi[2]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,0,99,3,yangshuzi[0]);
  	                          OLED_DrawBMP(100,0,112,3,yangshuzi[3]);
													  if(key3==0)
	 {	a1=1;
	 		 }					  	if(a1==5){ OLED_Clear();a1=10;}
			 
			 	if(a1==1){	 OLED_Clear();
											   	 OLED_DrawBMP(9,2,21,5,shuzi[TIME3[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME3[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME3[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME3[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME3[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME3[0]&0x0f]);


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	

										  while(a1==1)
										  {
										    ////12按键扫描
						  if(key1==0)
	 {
	 a2--; if(a2<0)a2=6;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
			if(key2==0)
	 {
	 a2++;	   if(a2>6)a2=0;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
										  	 switch(a2)	{


											 case(0):OLED_DrawBMP(9,2,21,5,yangshuzi[TIME3[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME3[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME3[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME3[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME3[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME3[0]&0x0f]);
						 if(key4==0)
	 {
	 TIME3[2]= TIME3[2]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME3[2]= TIME3[2]-16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(1):OLED_DrawBMP(9,2,21,5,shuzi[TIME3[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,yangshuzi[TIME3[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME3[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME3[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME3[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME3[0]&0x0f]);
									  if(key4==0)
	 {
	 TIME3[2]= TIME1[2]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME3[2]= TIME1[2]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(2):OLED_DrawBMP(9,2,21,5,shuzi[TIME3[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME3[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,yangshuzi[TIME3[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME3[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME3[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME3[0]&0x0f]);
										 if(key4==0)
	 {
	 TIME3[1]= TIME3[1]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME3[1]= TIME3[1]-16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(3):OLED_DrawBMP(9,2,21,5,shuzi[TIME3[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME3[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME3[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,yangshuzi[TIME3[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME3[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME3[0]&0x0f]);

						 if(key4==0)
	 {
	 TIME3[1]= TIME3[1]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME3[1]= TIME3[1]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(4):OLED_DrawBMP(9,2,21,5,shuzi[TIME3[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME3[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME3[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME3[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,yangshuzi[TIME3[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME3[0]&0x0f]);

						 if(key4==0)
	 {
	 TIME3[0]= TIME3[0]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME3[0]= TIME3[0]-16;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(5):OLED_DrawBMP(9,2,21,5,shuzi[TIME3[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME3[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME3[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME3[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME3[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,yangshuzi[TIME3[0]&0x0f]);
								   	 if(key4==0)
	 {
	 TIME3[0]= TIME3[0]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME3[0]= TIME3[0]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(6):OLED_DrawBMP(9,2,21,5,shuzi[TIME3[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME3[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME3[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME3[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME3[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME3[0]&0x0f]);
						 				   
				  if(key3==0)
	 {	a1=5;
	 		 }	

	      OLED_DrawBMP(0,6,16,8,yangxinqi[16]); //阳返回
  		          OLED_DrawBMP(17,6,33,8,yangxinqi[17]);
										  
										   }
										  }
			 							  }
			 
							  	OLED_DrawBMP(9,3,21,6,shuzi[0]);
  	                     OLED_DrawBMP(22,3,34,6,shuzi[4]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,3,60,6,shuzi[0]);
  	                       OLED_DrawBMP(61,3,73,6,shuzi[5]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,3,99,6,shuzi[0]);
  	                          OLED_DrawBMP(100,3,112,6,shuzi[6]);
				 
							    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		        OLED_DrawBMP(17,6,33,8,yangxinqi[11]);  break;
								   case(3):  	OLED_DrawBMP(9,0,21,3,shuzi[0]);
  	                     OLED_DrawBMP(22,0,34,3,shuzi[1]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,0,60,3,shuzi[0]);
  	                       OLED_DrawBMP(61,0,73,3,shuzi[2]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,0,99,3,shuzi[0]);
  	                          OLED_DrawBMP(100,0,112,3,shuzi[3]);
										   if(key3==0)
	 {	a1=1;
	 		 }					if(a1==5){ OLED_Clear();a1=10;}
			 
			 	if(a1==1){	 OLED_Clear();
											   	 OLED_DrawBMP(9,2,21,5,shuzi[TIME4[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME4[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME4[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME4[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME4[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME4[0]&0x0f]);


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	

										  while(a1==1)
										  {
										    ////12按键扫描
						  if(key1==0)
	 {
	 a2--; if(a2<0)a2=6;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
			if(key2==0)
	 {
	 a2++;	   if(a2>6)a2=0;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
										  	 switch(a2)	{


											 case(0):OLED_DrawBMP(9,2,21,5,yangshuzi[TIME4[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME4[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME4[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME4[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME4[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME4[0]&0x0f]);
						 if(key4==0)
	 {
	 TIME4[2]= TIME4[2]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME4[2]= TIME4[2]-16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(1):OLED_DrawBMP(9,2,21,5,shuzi[TIME4[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,yangshuzi[TIME4[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME4[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME4[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME4[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME4[0]&0x0f]);
									  if(key4==0)
	 {
	 TIME4[2]= TIME4[2]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME4[2]= TIME4[2]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(2):OLED_DrawBMP(9,2,21,5,shuzi[TIME4[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME4[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,yangshuzi[TIME4[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME4[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME4[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME4[0]&0x0f]);
										 if(key4==0)
	 {
	 TIME4[1]= TIME4[1]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME4[1]= TIME4[1]-16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(3):OLED_DrawBMP(9,2,21,5,shuzi[TIME4[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME4[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME4[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,yangshuzi[TIME4[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME4[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME4[0]&0x0f]);

						 if(key4==0)
	 {
	 TIME4[1]= TIME4[1]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME4[1]= TIME4[1]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(4):OLED_DrawBMP(9,2,21,5,shuzi[TIME4[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME4[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME4[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME4[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,yangshuzi[TIME4[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME4[0]&0x0f]);

						 if(key4==0)
	 {
	 TIME4[0]= TIME4[0]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME4[0]= TIME4[0]-16;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(5):OLED_DrawBMP(9,2,21,5,shuzi[TIME4[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME4[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME4[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME4[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME4[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,yangshuzi[TIME4[0]&0x0f]);
								   	 if(key4==0)
	 {
	 TIME4[0]= TIME4[0]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME4[0]= TIME4[0]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(6):OLED_DrawBMP(9,2,21,5,shuzi[TIME4[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME4[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME4[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME4[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME4[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME4[0]&0x0f]);
						 				   
				  if(key3==0)
	 {	a1=5;
	 		 }	

	      OLED_DrawBMP(0,6,16,8,yangxinqi[16]); //阳返回
  		          OLED_DrawBMP(17,6,33,8,yangxinqi[17]);
										  
										   }
										  }
			 							  }
			 
							  	OLED_DrawBMP(9,3,21,6,yangshuzi[0]);
  	                     OLED_DrawBMP(22,3,34,6,yangshuzi[4]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,3,60,6,shuzi[0]);
  	                       OLED_DrawBMP(61,3,73,6,shuzi[5]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,3,99,6,shuzi[0]);
  	                          OLED_DrawBMP(100,3,112,6,shuzi[6]);
				 
							    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		        OLED_DrawBMP(17,6,33,8,yangxinqi[11]);  break;
								    case(4):   	OLED_DrawBMP(9,0,21,3,shuzi[0]);
  	                     OLED_DrawBMP(22,0,34,3,shuzi[1]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,0,60,3,shuzi[0]);
  	                       OLED_DrawBMP(61,0,73,3,shuzi[2]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,0,99,3,shuzi[0]);
  	                          OLED_DrawBMP(100,0,112,3,shuzi[3]);
											  if(key3==0)
	 {	a1=1;
	 		 }						 	if(a1==5){ OLED_Clear();a1=10;}
			 
			 	if(a1==1){	 OLED_Clear();
											   	 OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	

										  while(a1==1)
										  {
										    ////12按键扫描
						  if(key1==0)
	 {
	 a2--; if(a2<0)a2=6;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
			if(key2==0)
	 {
	 a2++;	   if(a2>6)a2=0;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
										  	 switch(a2)	{


											 case(0):OLED_DrawBMP(9,2,21,5,yangshuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);
						 if(key4==0)
	 {
	 TIME1[2]= TIME1[2]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[2]= TIME1[2]-16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(1):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,yangshuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);
									  if(key4==0)
	 {
	 TIME1[2]= TIME1[2]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[2]= TIME1[2]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(2):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,yangshuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);
										 if(key4==0)
	 {
	 TIME1[1]= TIME1[1]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[1]= TIME1[1]-16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(3):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,yangshuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);

						 if(key4==0)
	 {
	 TIME1[1]= TIME1[1]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[1]= TIME1[1]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(4):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,yangshuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);

						 if(key4==0)
	 {
	 TIME1[0]= TIME1[0]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[0]= TIME1[0]-16;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(5):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,yangshuzi[TIME1[0]&0x0f]);
								   	 if(key4==0)
	 {
	 TIME1[0]= TIME1[0]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[0]= TIME1[0]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(6):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);
						 				   
				  if(key3==0)
	 {	a1=5;
	 		 }	

	      OLED_DrawBMP(0,6,16,8,yangxinqi[16]); //阳返回
  		          OLED_DrawBMP(17,6,33,8,yangxinqi[17]);
										  
										   }
										  }
			 							  }
			 
							  	OLED_DrawBMP(9,3,21,6,shuzi[0]);
  	                     OLED_DrawBMP(22,3,34,6,shuzi[4]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,3,60,6,yangshuzi[0]);
  	                       OLED_DrawBMP(61,3,73,6,yangshuzi[5]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,3,99,6,shuzi[0]);
  	                          OLED_DrawBMP(100,3,112,6,shuzi[6]);
				 
							    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		        OLED_DrawBMP(17,6,33,8,yangxinqi[11]); break;
									case(5):  	OLED_DrawBMP(9,0,21,3,shuzi[0]);
  	                     OLED_DrawBMP(22,0,34,3,shuzi[1]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,0,60,3,shuzi[0]);
  	                       OLED_DrawBMP(61,0,73,3,shuzi[2]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,0,99,3,shuzi[0]);
  	                          OLED_DrawBMP(100,0,112,3,shuzi[3]);
										 if(key3==0)
	 {	a1=1;
	 		 }					 	if(a1==5){ OLED_Clear();a1=10;}
			 
			 	if(a1==1){	 OLED_Clear();
											   	 OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);

	

										  while(a1==1)
										  {
										    ////12按键扫描
						  if(key1==0)
	 {
	 a2--; if(a2<0)a2=6;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
			if(key2==0)
	 {
	 a2++;	   if(a2>6)a2=0;
	  
	 Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }//k5扫描
										  	 switch(a2)	{


											 case(0):OLED_DrawBMP(9,2,21,5,yangshuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);
						 if(key4==0)
	 {
	 TIME1[2]= TIME1[2]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[2]= TIME1[2]-16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(1):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,yangshuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);
									  if(key4==0)
	 {
	 TIME1[2]= TIME1[2]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[2]= TIME1[2]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(2):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,yangshuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);
										 if(key4==0)
	 {
	 TIME1[1]= TIME1[1]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[1]= TIME1[1]-16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(3):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,yangshuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);

						 if(key4==0)
	 {
	 TIME1[1]= TIME1[1]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[1]= TIME1[1]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(4):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,yangshuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);

						 if(key4==0)
	 {
	 TIME1[0]= TIME1[0]+16;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[0]= TIME1[0]-16;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }


	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(5):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,yangshuzi[TIME1[0]&0x0f]);
								   	 if(key4==0)
	 {
	 TIME1[0]= TIME1[0]+1;	Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();		 }
	  if(key5==0)
	 {
	  TIME1[0]= TIME1[0]-1;		Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();	 }



	    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		       OLED_DrawBMP(17,6,33,8,yangxinqi[11]);break;
											 case(6):OLED_DrawBMP(9,2,21,5,shuzi[TIME1[2]/16]);
  	  OLED_DrawBMP(22,2,34,5,shuzi[TIME1[2]&0x0f]);
	  OLED_DrawBMP(35,2,47,5,yangshuzi[11]);
	   OLED_DrawBMP(48,2,60,5,shuzi[TIME1[1]/16]);
  	  OLED_DrawBMP(61,2,73,5,shuzi[TIME1[1]&0x0f]);
	  OLED_DrawBMP(74,2,86,5,yangshuzi[11]);
  	  OLED_DrawBMP(87,2,99,5,shuzi[TIME1[0]/16]);
  	  OLED_DrawBMP(100,2,112,5,shuzi[TIME1[0]&0x0f]);
						 				   
				  if(key3==0)
	 {	a1=5;
	 		 }	

	      OLED_DrawBMP(0,6,16,8,yangxinqi[16]); //阳返回
  		          OLED_DrawBMP(17,6,33,8,yangxinqi[17]);
										  
										   }
										  }
			 							  }
			 
							  	OLED_DrawBMP(9,3,21,6,shuzi[0]);
  	                     OLED_DrawBMP(22,3,34,6,shuzi[4]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,3,60,6,shuzi[0]);
  	                       OLED_DrawBMP(61,3,73,6,shuzi[5]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,3,99,6,yangshuzi[0]);
  	                          OLED_DrawBMP(100,3,112,6,yangshuzi[6]);
				 
							    OLED_DrawBMP(0,6,16,8,yangxinqi[10]); //阴返回
  		        OLED_DrawBMP(17,6,33,8,yangxinqi[11]);  break;
									case(6):   	OLED_DrawBMP(9,0,21,3,shuzi[0]);
  	                     OLED_DrawBMP(22,0,34,3,shuzi[1]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,0,60,3,shuzi[0]);
  	                       OLED_DrawBMP(61,0,73,3,shuzi[2]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,0,99,3,shuzi[0]);
  	                          OLED_DrawBMP(100,0,112,3,shuzi[3]);
								
							  	OLED_DrawBMP(9,3,21,6,shuzi[0]);
  	                     OLED_DrawBMP(22,3,34,6,shuzi[4]);
	                         // OLED_DrawBMP(35,1,47,4,yangshuzi[11]);
	                        OLED_DrawBMP(48,3,60,6,shuzi[0]);
  	                       OLED_DrawBMP(61,3,73,6,shuzi[5]);
	                        // OLED_DrawBMP(74,1,86,4,yangshuzi[11]);
  	                             OLED_DrawBMP(87,3,99,6,shuzi[0]);
  	                          OLED_DrawBMP(100,3,112,6,shuzi[6]);
				 				   
				  if(key3==0)
	 {	e1=5;
	 		 }	
							  
	   OLED_DrawBMP(0,6,16,8,yangxinqi[16]); //阳返回
  		          OLED_DrawBMP(17,6,33,8,yangxinqi[17]);
								
								
								
								
								
								
								
								
								
								
								
								
								
								
								
								
								
								}



				 
				 
				 
				 
				 
				 
				 
				 
				 		 }
				 
				 }










				}
				 
			  
			  
				
			
			  
}
	          
		
					 
				   
			
			
		


  }
		   }
		 
		 
		 
}		 
		 
		 }
	
}


/*******************************************************************************
* 函数名         : LcdDisplay()
* 函数功能		   : 显示函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/

void LcdDisplay()
{	OLED_ShowNum(0,0,'0'+TIME[2]/16,4,16);
	
	LcdWriteData('0'+TIME[2]/16);				//时
	LcdWriteData('0'+(TIME[2]&0x0f));				 
	LcdWriteData('-');
	LcdWriteData('0'+TIME[1]/16);				//分
	LcdWriteData('0'+(TIME[1]&0x0f));	
	LcdWriteData('-');
	LcdWriteData('0'+TIME[0]/16);				//秒
	LcdWriteData('0'+(TIME[0]&0x0f));

	LcdWriteCom(0x80);
	LcdWriteData('2');
	LcdWriteData('0');
	LcdWriteData('0'+TIME[6]/16);			//年
	LcdWriteData('0'+(TIME[6]&0x0f));
	LcdWriteData('-');
	LcdWriteData('0'+TIME[4]/16);			//月
	LcdWriteData('0'+(TIME[4]&0x0f));
	LcdWriteData('-');
	LcdWriteData('0'+TIME[3]/16);			//日
	LcdWriteData('0'+(TIME[3]&0x0f));
	LcdWriteCom(0x8D);
	LcdWriteData('0'+(TIME[5]&0x07));	//星期			 
}
/*******************************************************************************
* 函数名         : Int0Configuration()
* 函数功能		   : 配置外部中断0
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/

void Int0Configuration()
{
	//设置INT0
	IT0=1;//跳变沿出发方式（下降沿）
	EX0=1;//打开INT0的中断允许。
	EA=1;//打开总中断	
}
/*******************************************************************************
* 函 数 名         : void Timer0() interrupt 1
* 函数功能		   : 定时器0中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Timer0() interrupt 1
{
	static int i;
	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;
	i++;
	if(i==3)
	{  
	  
	  	   

		 DisplayData[1]=smgduan[TIME[1]/16];
	  DisplayData[0]=smgduan[TIME[1]&0x0f];//分
	  DisplayData[3]=smgduan[TIME[2]/16];
	   DisplayData[2]=smgduan[TIME[2]&0x0f];
	 DigDisplay();	
	dp=1;k1=0;k2=0;		// 
		if((s2==5)&&(s3==5))
		   		   {  beep=~beep;}
	
		   /*	if((TIME1[1]==TIME[1])&&(TIME1[0]==TIME[0])&&(TIME1[2]==TIME[2]))
		   		   {  	s2=5;s3=5;
				    if(s2==5)
				   {
				   	   beep=~beep;
					   
					   
					   
					    }
				   
				   
				   }  

		if((i==1000)&&(s2==5))	{
	
	     s1++;
		 if(s1==10){s1=0;s2=10;	}

			 }	*/
		  		  
		  
		   	     

		   
		 

		   
		   
		   	
		 i=0;  	

	 	}	 

   	
	
}

void Timer1() interrupt 3
{
	static int i;
	TH1=0XFC;	//给定时器赋初值，定时1ms
	TL1=0X18;
	i++;
	if(i==1000)
	{			 //
		if( s2==5)
		{ s3=5;s1++;	
			  
			  if(s1>10)
			 { s2=10;s3=10; s1=0;}
		
		}

		
	   i=0;
			
	}	
}

/*******************************************************************************
* 函数名         : Delay10ms
* 函数功能		   : 延时函数，延时10ms
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void Delay10ms(void)   //误差 0us
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}
