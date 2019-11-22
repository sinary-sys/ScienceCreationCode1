#ifndef __QQQ_H
#define __QQQ_H
#define uchar unsigned char
#define uint  unsigned int          
#include <intrins.h>
/*******************************************************/

/*****STC89C51RC���⹦�ܼĴ�������********/
sfr  isp_wdt     = 0xE1;
sfr  isp_data    = 0xE2;
sfr  isp_addrh   = 0xE3;
sfr  isp_addrl   = 0xE4;
sfr  isp_cmd     = 0xE5;
sfr  isp_trig    = 0xE6;
sfr  isp_contr   = 0xE7;
/*****************************************/

/***************STC89CXX_EEPROM������������*************************/
void STC89CXX_EEPROM_Erase(unsigned char t_addr)
{
    isp_addrh = t_addr;   //���Բ�Ҫ ����������һ���ֽڵĵ�ַ����������ַ���������׵�ַ����Ƭ�����Լ�����
    isp_addrl = 0;
    EA    =    0;         //���ж�
    isp_contr = 0x81;      
    isp_cmd = 3;          //����������Ҫĳ�ֽ�Ϊ�գ��������1������
    isp_trig = 0x46;     //����0x46���� 0xb9 ISP/IAP �����Ĵ�����ÿ�ζ���Ҫ���
    isp_trig = 0xb9;     //����0xb9�� ISP/IAP ��������������
    _nop_();
    _nop_();
    isp_contr = 0;   
    isp_cmd = 0;
    isp_trig = 0;
    isp_addrh = 0;
    isp_addrl = 0;
    EA = 1;                 // ���ж�
}
/***********************************************/


/***************STC89CXX_EEPROM�ֽ�д����*************************/
void STC89CXX_EEPROM_Char_Write(unsigned char Write_addr_h,unsigned char Write_addr_l ,unsigned char Write_data)
{
      
        isp_data = Write_data;
        isp_addrh = Write_addr_h;     //�͵�ַ��λ
        isp_addrl = Write_addr_l;    //��ַ��λ
        EA =  0;                     /* ���ж� */
        isp_contr = 0x81;      
        isp_cmd = 2;                //����������
        isp_trig = 0x46;
        isp_trig = 0xb9;
        _nop_();
        _nop_();
        isp_contr = 0;              
        isp_cmd = 0;
        isp_trig = 0;
        isp_addrh = 0;
        isp_addrl = 0;        
        EA = 1;                    /* ���ж� */
   
}
/**************************************************************/

/***************STC89CXX_EEPROM�ֽڶ�����*************************/
int STC89CXX_EEPROM_Char_Read(unsigned char Read_addr_h,unsigned char Read_addr_l)
{
  
        unsigned char Read_isp_data;

        isp_addrh = Read_addr_h;  //�͵�ַ���ֽ�
        isp_addrl = Read_addr_l;  //�͵�ַ���ֽ�(��0��ʼ��num-1Ϊֹ��
        EA = 0;                /* ���ж� */
        isp_contr = 0x81;      /*20M����0x80 orl 1 */
        isp_cmd = 1;           // 1��ʾ�ֽڶ�
        isp_trig = 0x46;       //����0x46���� 0xb9 ISP/IAP �����Ĵ�����ÿ�ζ���Ҫ���
        isp_trig = 0xb9;       //����0xb9�� ISP/IAP ��������������
        _nop_();               //��ָ��
        _nop_();               //��ָ��
        isp_contr = 0;         //��ֹISP/IAP ����
        isp_cmd = 0;           //ȥ��ISP/IAP ����
        isp_trig = 0;          //��ֹ ISP/IAP �����󴥷�
        isp_addrh = 0;         //��ַָ���EEPROM��ַ������ֹ�����
        isp_addrl = 0;        
        EA = 1;                            /* ���ж� */
        Read_isp_data=isp_data;
        return(Read_isp_data);
   
} 
/**************************************************************/

/*******************************************************************************/

#endif
