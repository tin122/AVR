#ifndef EEPROM_H_
#define EEPROM_H_

#include "main.h"

void EEPROM_write(unsigned int uiAddress, unsigned char ucData);
unsigned char EEPROM_read(unsigned int uiAddress);
void EEPROM_write_word(unsigned int uiAddress, uint16_t ucData); //����� 2�-������� ��������
uint16_t EEPROM_read_word(unsigned int uiAddress); //������ 2�-������� ��������

#endif /* EEPROM_H_ */