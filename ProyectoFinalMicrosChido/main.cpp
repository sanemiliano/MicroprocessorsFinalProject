#define F_CPU 2000000UL //1MHz
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "lcd.h"
#include "lcd.c"
#include <string.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>


#define BAUD 9600
int MYUBRR = F_CPU/16/BAUD-1;

char linea1[] = "Ventilador prendido \n\r"; // \r para que funcione en proteus
char linea2[] = "Envia un 1 si quieres moverlo a la izquierda \n\r"; // /n para que funcione en Terminal
char linea3[] = "Envia un 2 si quieres moverlo a la derecha \n\r";
char aux[] = "";
void imprimelinea(char* dato);
volatile char recepcion = (char)255; //volatile para que conserve los datos en main y en ISR

int main(void)
{
	//Configuracion del LCD
	//lcd_init(LCD_DISP_ON);
	//lcd_home();
	//char estado[16];

	//Configuracion del motor
	DDRC = 0b00000001;
	PORTC = 0b00000000;

	//Configuracion del termometro
	//DDRA = 0b00000000;
	//ADMUX |= (0<<ADLAR) | (1<<REFS0) | (1<<REFS1);
	//ADCSRA|=(1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);
	
	//Configuracion del servo motor
	TCCR1A |= (1<<COM1A1) | (1<<COM1B1) | (1<<WGM11); //set 0C1A/0C1B at bottom, non-inverting mode
	TCCR1B |= (1<<CS10) | (1<<WGM12) | (1<<WGM13);
	ICR1 = 180000;
	DDRD |= (1<<PD4) | (1<<PD5);
	int grados = 900;
	
	//Transmisicion por bluetooth
	/* Set baud rate */
	UBRRH = (uint8_t)((MYUBRR)>>8);
	UBRRL = (uint8_t)MYUBRR;
	/* Enable receiver and transmitter */
	UCSRB |=  (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);/* Formato: 8 data, 1stop bit, PARIDAD DESACTIVADA */
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
	sei();  //Actvar interrupciones

	recepcion = (int)255;
	while(recepcion != '1');
	imprimelinea(linea1);
	imprimelinea(linea2);
	imprimelinea(linea3);
	while (1)
	{
		//Lectura del termometro
		//ADCSRA|=(1<<ADSC);
		//while( ADCSRA & (1<<ADSC) );
		//int valor = ADCL+ (ADCH>>8);
		//float c = float(valor) * 0.25;
		//if(c>=32)
		//{
			//PORTC = 1;
		//}
		//else
		//{
			//PORTC = 0;
		//}
		//sprintf(estado,"%f",(float)c);
		//lcd_puts(estado);
		//lcd_putc(' ');
		//lcd_putc(' ');
		//lcd_putc(' ');
		//lcd_putc(' ');
		//lcd_putc(' ');
		//lcd_putc(' ');
		//lcd_putc(' ');

		//Lectura de instruccion blue
		if(recepcion == '1'){
			if(grados < 4000)
			grados += 100;
		}
		if(recepcion == '2'){
			if(grados >= 2000){
				grados -= 100;
			}
		}
		OCR1A = grados;
		recepcion = (char)255; // Restablecemos la entrada
		_delay_ms(200);
	}

}

ISR(USART_RXC_vect)
{
	recepcion =UDR;
}

void imprimelinea (char* dato)
{
	for (uint8_t x = 0; x < strlen(dato) ;x++)
	{
		while( !( UCSRA & (1<<UDRE)) );
		UDR = dato[x];
	}
}