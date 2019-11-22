#ifndef __QQQ_H
#define __QQQ_H
#define uchar unsigned char
#define uint  unsigned int          
#include <intrins.h>
/*******************************************************/

/*****STC89C51RC特殊功能寄存器定义********/
sfr  isp_wdt     = 0xE1;
sfr  isp_data    = 0xE2;
sfr  isp_addrh   = 0xE3;
sfr  isp_addrl   = 0xE4;
sfr  isp_cmd     = 0xE5;
sfr  isp_trig    = 0xE6;
sfr  isp_contr   = 0xE7;
/*****************************************/

/***************STC89CXX_EEPROM扇区擦除程序*************************/
void STC89CXX_EEPROM_Erase(unsigned char t_addr)
{
    isp_addrh = t_addr;   //可以不要 扇区里任意一个字节的地址都是扇区地址，无需求首地址，单片机会自己处理
    isp_addrl = 0;
    EA    =    0;         //关中断
    isp_contr = 0x81;      
    isp_cmd = 3;          //扇区擦除，要某字节为空，必须擦除1个扇区
    isp_trig = 0x46;     //先送0x46再送 0xb9 ISP/IAP 触发寄存器，每次都需要如此
    isp_trig = 0xb9;     //送完0xb9后 ISP/IAP 立即被触发启动
    _nop_();
    _nop_();
    isp_contr = 0;   
    isp_cmd = 0;
    isp_trig = 0;
    isp_addrh = 0;
    isp_addrl = 0;
    EA = 1;                 // 开中断
}
/***********************************************/


/***************STC89CXX_EEPROM字节写程序*************************/
void STC89CXX_EEPROM_Char_Write(unsigned char Write_addr_h,unsigned char Write_addr_l ,unsigned char Write_data)
{
      
        isp_data = Write_data;
        isp_addrh = Write_addr_h;     //送地址高位
        isp_addrl = Write_addr_l;    //地址低位
        EA =  0;                     /* 关中断 */
        isp_contr = 0x81;      
        isp_cmd = 2;                //送扇区命令
        isp_trig = 0x46;
        isp_trig = 0xb9;
        _nop_();
        _nop_();
        isp_contr = 0;              
        isp_cmd = 0;
        isp_trig = 0;
        isp_addrh = 0;
        isp_addrl = 0;        
        EA = 1;                    /* 开中断 */
   
}
/**************************************************************/

/***************STC89CXX_EEPROM字节读程序*************************/
int STC89CXX_EEPROM_Char_Read(unsigned char Read_addr_h,unsigned char Read_addr_l)
{
  
        unsigned char Read_isp_data;

        isp_addrh = Read_addr_h;  //送地址高字节
        isp_addrl = Read_addr_l;  //送地址低字节(从0开始到num-1为止）
        EA = 0;                /* 关中断 */
        isp_contr = 0x81;      /*20M，是0x80 orl 1 */
        isp_cmd = 1;           // 1表示字节读
        isp_trig = 0x46;       //先送0x46再送 0xb9 ISP/IAP 触发寄存器，每次都需要如此
        isp_trig = 0xb9;       //送完0xb9后 ISP/IAP 立即被触发启动
        _nop_();               //空指令
        _nop_();               //空指令
        isp_contr = 0;         //禁止ISP/IAP 操作
        isp_cmd = 0;           //去除ISP/IAP 命令
        isp_trig = 0;          //防止 ISP/IAP 命令误触发
        isp_addrh = 0;         //地址指向非EEPROM地址区，防止误操作
        isp_addrl = 0;        
        EA = 1;                            /* 开中断 */
        Read_isp_data=isp_data;
        return(Read_isp_data);
   
} 
/**************************************************************/

/*******************************************************************************/

#endif
