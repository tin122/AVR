#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/atomic.h>

void port_ini (void) // инициализация портов (неиспользуемые ножки?)
{
	// определяем выходные
	DDRA = 0b11111111; // выход PA0-PA7-светодиоды
	PORTA = 0b11111111; //отключаем светодиоды
	
	DDRB = (1<<PB0)|(1<<PB1)|(1<<PB2); // выходы на оптику и блокировки драйвера: ERR, DR_PPN_OFF_MCU, DR_INV_OFF_MCU;
	//PORTB = (1<<PB0); //проверен ERR
	
	// определяем входные
	DDRC = 0b00000000; // входы: RESET, ERR_DR_PPN, ERR_DR_INV;
	PORTC |= (1<<PC3); // свободный пин 21 в PullUp 
	
	DDRD = (1<<PD2)|(1<<PD3); // входы: I_MAX, ERR_NAPR_1;
	
	//DDRE = (1<<PE0); //входы: ERR_NAPR_2; // по ТЗ не востребован
}
	
void interrupt_ini (void) // инициализация прерываний 
{
	//? General Interrupt Flag – GIFR
	
	//------------INT 0,1 -- MCUCR --- 
	GICR |= (1<<INT0)|(1<<INT1); // разрешаем прерывание на выводах
	//-I_MAX
	MCUCR &= ~((1<<ISC01)|(1<<ISC00)); //0 0 The low level of INT0 generates an interrupt request // срабатывание по минимальному уровню, обрабатываются асинхронно
	//-ERR_NAPR_1
	//MCUCR &= ~((1<<ISC11)|(1<<ISC10)); //0 0 The low level of INT1 generates an interrupt request.
	
	/*//----------- INT2, EMCUCR ---  //ERR_NAPR_2; - по ТЗ не востребован
	GICR |= (1<<INT2); по ТЗ не востребован 
	//ERR_NAPR_2  // перепадов сигнала, также происходит асинхронно //x
	GICR &= ~(1<<INT2);  // отключаем вывод перед вненсением изменений
	EMCUCR |= (1<<ISC2); // срабатывание по восходящему фронту
	GIFR |= (1<<INTF1);   //  сброс флага // !Правильно: GIFR = (1<<INTF1) по radidetector, отладить!.
	GICR |= (1<<INT2);   // включаем прерывание на выводе
	*/
	
	//----------- PCINT 10,9,8 ---
	GICR |= (1<<PCIE1); //включаем группу выводов 8-15
	// включаем вывода - Pin Change Mask Register
	//RESET_1
	PCMSK1 |=(1<<PCINT8);  
	//ERR_DR_PPN
	PCMSK1 |=(1<<PCINT9);
	//ERR_DR_IN
	PCMSK1 |=(1<<PCINT10);	
}
void timer0_ini (void) // задействован на вывод индикации ERR
{
	#define TIMER0_OFF() TCCR0|=(1<<CS02)|(1<<CS01)|(1<<CS00); // 2 - No clock source
	#define TIMER0_ON()  TCCR0|= (1<<CS02)|(1<<CS00);          // 1024 prescaler
	#define TOGGLE_OC0() TCCR0|=(1<<FOC0); // изменить состояние вывода OC0
	
	#define timer0_f_100Hz()  OCR0=77; //  100 Hz
	#define timer0_f_250Hz()  OCR0=30; //  250 Hz
	#define timer0_f_500Hz()  OCR0=14; //  500 Hz
	#define timer0_f_1000Hz() OCR0=7;  // 1000 Hz
	#define timer0_f_2000Hz() OCR0=3;  // 2000 Hz
			
	//ERR_DR_PPN - 2 kHz //установки
	//ERR_DR_INV - 1 kHz
	//I_MAX - 500 Hz
	//ERR_NAPR_1 - 250 Hz
	//ERR_NAPR_2 - 100 HZ
	//fOCn = (fclk_I/O) / (2 * N *  (1 + OC Rn))
	
	TCCR0 |=(1<<WGM01); //Mode CTC
	TCCR0 |=(1<<COM00); //Toggle OC0 on Compare Match.
	
	//----------- варианты отключения ----// всегда ли будет еденица на ножке при отключении таймера !?
	//TCCR0 &=~(1<<COM00); // 1 - отключаем ножку
		
	//timer0_f_100Hz();
	//TIMER0_ON(); //TCCR0 |= (1<<CS02)|(1<<CS00); // 1024 prescaler
}

//-------- обработчики прерываний ------------------------
//выводы на блокировку
#define DR_PPN_OFF_MCU_SET()      PORTB|=   0b00000010;
#define DR_INV_OFF_MCU_SET()      PORTB|=  0b00000100;
#define DR_ALL_OFF_MCU_UNSET() PORTB&=~0b00000110;
//индикации
#define LED_HL1_RABOTA_MCU PORTA&=~0b00000001;
#define LED_HL2_ERR_NAPR_2 PORTA&=~0b00000010;
#define LED_HL3_ERR_NAPR_1 PORTA&=~0b00000100;
#define LED_HL4_I_MAX      PORTA&=~0b00001000;
#define LED_HL5_ERR_DR_INV PORTA&=~0b00010000;
#define LED_HL6_ERR_DR_PPN PORTA&=~0b00100000;
#define LED_HL7_ERR        PORTA&=~0b01000000;
#define LED_HL8_RESET_1    PORTA&=~0b10000000;
#define LED_CLR_ERR        PORTA|= 0b01111111;

ISR(INT0_vect) // IMAX
{
	DR_INV_OFF_MCU_SET();
	DR_PPN_OFF_MCU_SET();
	LED_HL4_I_MAX;
	LED_HL7_ERR;
}

ISR(INT1_vect) //ERR_NAPR_1
{
	DR_PPN_OFF_MCU_SET()
	DR_INV_OFF_MCU_SET();
	LED_HL3_ERR_NAPR_1;
	LED_HL7_ERR;
}
//------------------------
//#define KEY1 _BV(PC0) //вариант
#define IN_RESET_1     (1<<PINC0)
#define IN_ERR_DR_PPN  (1<<PINC1)
#define IN_ERR_DR_INV  (1<<PINC2)
#define ALL_IN_ERR     (IN_ERR_DR_PPN|IN_ERR_DR_INV)

ISR(PCINT1_vect) //RESET_1, ERR_DR_PPN, ERR_DR_INV
{
	asm("nop");
	
	if (PINC&ALL_IN_ERR)
	{
		asm("nop");
				
		if (PINC&IN_ERR_DR_PPN) //ERR_DR_PPN
		{
			DR_PPN_OFF_MCU_SET();
			LED_HL6_ERR_DR_PPN;
			LED_HL7_ERR;
		} 
		else if (PINC&IN_ERR_DR_INV) //ERR_DR_INV
		{
			DR_INV_OFF_MCU_SET();
			LED_HL5_ERR_DR_INV;
			LED_HL7_ERR;
		}
	} 
	else if (PINC&IN_RESET_1)
	{ 
		asm("nop");
		
		if (PINC&IN_RESET_1) 
		{
			//снимаем защиты DR и INV
			TIMER0_OFF(); //отключаем ERR
			LED_CLR_ERR;//отключаем светодиоды
		} 
	}
	else
	{
	}	
}



int main(void)
{
   port_ini();
   //timer0_ini();
   interrupt_ini();
   asm("nop");
   sei(); //Разрешаем прерывания глобально // cli();
   
    while (1) 
    {
    }
}

