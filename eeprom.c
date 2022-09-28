#include "eeprom.h"
//---------------------------
 void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
 {
	 while(EECR & (1<<EEWE)) //���� ������������ ����� ��������� ��������� ��������� � �������
	 {}
	 EEAR = uiAddress; //������������� �����
	 EEDR = ucData; //����� ������ � �������
	 EECR |= (1<<EEMWE); //��������� ������
	 EECR |= (1<<EEWE); //����� ���� � ������
	 //asm("nop"); 
 }

 unsigned char EEPROM_read(unsigned int uiAddress)
 {
	 while(EECR & (1<<EEWE))
	 {} //���� ������������ ����� ��������� ��������� ��������� � �������
	 EEAR = uiAddress; //������������� �����
	 EECR |= (1<<EERE); //��������� �������� ���������� �� ������ � ������� ������
	 return EEDR; //���������� ���������
 }
 
 void EEPROM_write_word(unsigned int uiAddress, uint16_t ucData)
 {
	  EEPROM_write(uiAddress, (unsigned char) ucData);
	  unsigned char dt = ucData>>8;
	  EEPROM_write(uiAddress+1, dt);
 }
  
  uint16_t EEPROM_read_word(unsigned int uiAddress)
  {
	   uint16_t dt = EEPROM_read(uiAddress+1)*256;
	   asm("nop");
	   dt += EEPROM_read(uiAddress);
	   return dt;
  }
 

 