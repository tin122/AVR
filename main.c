#include "main.h"
//---------------------------------------------------

//--------------------------------------------------- 
void port_ini(void)
{
	DDRA    = 0b11110000; 	// ���������/������
	PORTA  |= 0b00001111;   // �������/�������� � �������
}

void init_PWM_timer_0 (void) //�������������  � delay!
{

}

void init_CTC_timer_2 (void) //  ��� ����� ������
{
	//11 0x014 - TIMER2 COMP
	
	TCCR2 |= (1<<WGM21); // WGM21 - 1, WGM20 - 0 -- ������ CTC, ��� ��������
	TIMSK |= (1<<OCIE2); // ��� ���������� � 4Bit
	OCR2   = 0b11111111; //����� ��� ��������� �� ��������� TCNT
	TCCR2 |= (1<<CS20)|(1<<CS21)|(1<<CS22); //������ �������� ������� 1024
}
//--------- Timer 1/Timer 3 - ���������� ---------------------

void init_PWM_timer_1(void) // ������� ���������
{
	DDRD  = (1<<PD5);  //OC1A
	DDRE  = (1<<PE2);  //OC1B
	// COM1A1|COM1A0|COM1B1|COM1B0|FOC1A|FOC1B|WGM11|WGM10 --- TCCR1A
	// ICNC1  ICES1  �      WGM13  WGM12 CS12  CS11  CS10  --- TCCR1B   
	
	/* COMnA1/COMnB1   COMnA0/COMnB0
	0 0 - Normal port operation, OCnA/OCnB disconnected.
	0 1 - Toggle OCnA/OCnB on Compare Match.
	1 0 - Clear OCnA/OCnB on Compare Match (Set output to low level).
	1 1 - Set OCnA/OCnB on Compare Match (Set output to high level).
	*/
	TCCR1A |= (1<<COM1A1)|(1<<COM1A0)|(1<<COM1B1);
	TCCR1A &= ~ (1<<COM1B0);
	/*
	WGM13	WGM12	WGM11	WGM10
    0000 - ������� �����
    0001 - ��������� ���� PWM, 8-���
    0010 - ��������� ���� PWM, 9-���
    0011 - ��������� ���� PWM, 10-���
    0100 - ����� ����� ��������� (OCR1A) (����� ��� ����������)
    0101 - PWM, 8-���
    0110 - PWM, 9-���
    0111 - PWM, 10-���
    1000 - ��������� ���� � ������� PWM (ICR1)
    1001 - ��������� ���� � ������� PWM (OCR1A)
    1010 - ��������� ���� PWM (ICR1)
    1011 - ��������� ���� � ������� PWM (OCR1A)
    1100 - ����� ����� ��������� (ICR1) (����� ��� ����������)
    1101 - ������
    1110 - PWM (ICR1)
    1111 - PWM (OCR1A) */

 	TCCR1A &= ~(1<<WGM10); // ��������� ���� � �������
 	TCCR1A &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	TCCR1B |= (1<<WGM13);
	/*
	CS12 CS11 CS10 - clkI/O/1 (No prescaling)
	*/
	TCCR1B |=(1<<CS10);
	TCCR1B &= ~(1<<CS11);
	TCCR1B &= ~(1<<CS12);
	
	OCR1A=50;
	OCR1B=50;
	ICR1=1000;
	
}

void init_PWM_timer_3(void) // �� ��������� ������
{
	DDRD  = (1<<PD4);  //OC3A
	DDRB  = (1<<PB4);  //OC3B
	
	TCCR3A |= (1<<COM3A1)|(1<<COM3A0)|(1<<COM3B1);
	TCCR3A &= ~ (1<<COM3B1);
	
	TCCR3A &= ~(1<<WGM10); // ��������� ���� � �������
	TCCR3A &= ~(1<<WGM11);
	TCCR3B &= ~(1<<WGM12);
	TCCR3B |= (1<<WGM13);
	
	TCCR3B |=(1<<CS10);
	TCCR3B &= ~(1<<CS11);
	TCCR3B &= ~(1<<CS12);
	
	OCR3A=5;
	OCR3B=450;
	ICR3=1000;

}
//////////////////////////////////////////////////////////////////////////
int main(void)
{ 
	port_ini();
	init_PWM_timer_1();
	//init_PWM_timer_3();
	
	//-----------������� ��������� �� ������ --------------
	//ICR1=EEPROM_read_word(1);//��������, ������� 2 �����
	unsigned int i = 0;
	
    while (1) 
    {
		if (~PINA & (1<<0)) // 1
		{
			_delay_ms(10);
			i+=10;
			_delay_ms(200);
			if (i>=50)
			{
				i=0;
				PORTA  &= ~(0b11110000);				
			}
		}
		else if (~PINA & (1<<1)) // 2
		{
			_delay_ms(100);
			PORTA  |= (0b00100000);
			i += 2;
		}
		else if (~PINA & (1<<2)) // 3
		{
			_delay_ms(100);
			PORTA  |= (0b01000000);
			i += 3;
		}
		else if (~PINA & (1<<3)) // 4
		{
			_delay_ms(100);
			PORTA  |= (0b10000000);
			i += 4;
		}
		
		switch (i)
		{
			case 10:
			PORTA  |= (0b00010000);
			break;
					
			case 20:
			PORTA  |= (0b00100000);
			break;
					
			case 30:
			PORTA  |= (0b01000000);
			break;
					
			case 40:
			PORTA  |= (0b10000000);
			break;
			//------ ����������� ����������--------------
			case 13:
			PORTA  &=~(0b01000000);
			_delay_ms(100);
			PORTA  |= (0b01000000);
			ICR1  += 10;
			i-=3;
			break;
			
			case 14:
			PORTA  &=~(0b10000000);
			_delay_ms(100);
			PORTA  |= (0b10000000);
			ICR1  -= 10;
			i-=4;
			break;
			
			case 23:
			PORTA  &=~(0b01000000);
			_delay_ms(100);
			PORTA  |= (0b01000000);
			_delay_ms(100);
			PORTA  &=~(0b01000000);
			OCR1A += 10;
			i-=3;
			break;
			
			case 24:
			PORTA  &=~(0b10000000);
			_delay_ms(100);
			PORTA  |= (0b10000000);
			_delay_ms(100);
			PORTA  &=~(0b10000000);
			OCR1A -= 10;
			i-=4;
			break;
			
			case 33:
			PORTA  &=~(0b01000000);
			_delay_ms(100);
			PORTA  |= (0b01000000);
			_delay_ms(100);
			PORTA  &=~(0b01000000);
			OCR1B += 10;
			i-=3;
			break;
			
			case 34:
			PORTA  &=~(0b01000000);
			_delay_ms(100);
			PORTA  |= (0b01000000);
			_delay_ms(100);
			PORTA  &=~(0b01000000);
			OCR1B -= 10;
			i-=3;
			break;	
		}
		//------ ����� -----------
		if (OCR1A>1000)
		{
			OCR1A = 5;
		}
		if (OCR1B>1000)
		{
			OCR1B = 5;
		}
		
			
	} //while end
	
} //main end

