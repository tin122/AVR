#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>
#define  F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#include "eeprom.h"

void port_ini(void);
void init_PWM_timer_1(void);

#endif /* MAIN_H_ */