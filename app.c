
#include<util/delay.h>
#include <avr/io.h>

#include "keypad.h"
#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "std_types.h"




#define NO_TO_PASS 5



uint8 g_teck = 0 ;
uint8 gflag = 0;
uint8 counttocheck = 0 ;

void passwordCreating(void);
void errorPass(void);
void wrong(void);
void oDoorBack(void);
void oDoor(void);
void passALTER (void);




int main(void)
{

	SREG   |= (1<<7) ;

	LCD_init();

	UART_ConfigType uartConfig = {BD8,DISABLED,ONE_BIT,9600} ;

	UART_init(&uartConfig);

	passwordCreating();

	while(1)
	{
		LCD_clearScreen();

		uint8 vlu;

		LCD_moveCursor(0,0);

		LCD_displayString("+ : Open Door");

		LCD_moveCursor(1,0);

		LCD_displayString("- : Change Pass"); //display on LCD


		vlu=KEYPAD_getPressedKey();


		if(vlu=='+')
		{
			UART_sendByte('+');
			oDoor(); //open door
		}
		else if(vlu=='-')
		{
			UART_sendByte('-');
			passALTER ();
		}
	}
}

void passALTER(void)
{
	UART_sendByte('-');

	LCD_clearScreen();


	LCD_moveCursor(0,0);

	LCD_displayString("plz enter pass: ");

	LCD_moveCursor(1,0);

	sint8 vluu;
	uint8 counter=0;
	do
	{
		counter++;
		vluu=KEYPAD_getPressedKey();
		_delay_ms(250);
		if(counter<=NO_TO_PASS)
		{
			UART_sendByte(vluu);
			LCD_displayCharacter('*');
		}

	}while(vluu!= '=' ) ;

	LCD_clearScreen();

	if(UART_recieveByte())
	{
		counttocheck++;

		if(counttocheck==3)
		{
			counttocheck=0;
			wrong();
		}
		else
		{
			passALTER();
		}
	}
	else
	{
		counttocheck=0;

		passwordCreating();
	}
}


void oDoor(void)
{
	LCD_clearScreen();

	LCD_moveCursor(0,0);

	LCD_displayString("plz enter pass: ");

	LCD_moveCursor(1,0) ;

	sint8 vluu;

	uint8 counter=0;

	do
	{
		counter++;

		vluu=KEYPAD_getPressedKey();

		_delay_ms(250);

		if(counter<=NO_TO_PASS)
		{
			UART_sendByte(vluu);

			LCD_displayCharacter('*');

		}

	}while(vluu!='=');


	LCD_clearScreen();

	if(UART_recieveByte())
	{
		counttocheck++;

		if(counttocheck==3)
		{
			counttocheck=0;
			wrong();
		}
		else
		{
			oDoor();
		}
	}
	else
	{
		LCD_clearScreen();

		counttocheck=0;

		Timer1_setCallBack(oDoorBack);

		Timer1_ConfigType t1Config={0,0,PS_64_T1,NORM_T1,NONE,TOGGLE_T1};

		Timer1_init(&t1Config);

		while(gflag==0);

		gflag=0;
	}
}


void passwordCreating(void)
{
	sint8 vluu=-1;
	sint8 value =-1;

	LCD_displayString("plz enter pass:");

	LCD_moveCursor(1,0);

	uint8 count=0;

	do
	{
		count++;
		vluu=KEYPAD_getPressedKey();

		_delay_ms(250);

		if(count<=NO_TO_PASS)
		{
			UART_sendByte(vluu);
			LCD_displayCharacter('*');
		}

	}while(vluu!='=');

	LCD_clearScreen();
	LCD_moveCursor(0,0);

	LCD_displayString("plz re-enter the");
	LCD_moveCursor(1,0);

	LCD_displayString("same pass: ");

	count=0;


	do
	{
		count++;
		value=KEYPAD_getPressedKey();

		_delay_ms(250);

		if(count<=NO_TO_PASS)
		{
			UART_sendByte(value);
			LCD_displayCharacter('*');
		}

	}while(value!='=');
	for(char i=0;i<=NO_TO_PASS;i++)
	{
		if(vluu!=value)
		{
			LCD_clearScreen();
			LCD_moveCursor(0,0);
			LCD_displayString("ERROR!!");
			break;
		}
		else
		{
			continue;
		}
	}

	LCD_clearScreen();

	if(UART_recieveByte())
	{
		passwordCreating();
	}
	else
	{
		return;
	}
}


void oDoorBack(void)
{
	g_teck++;

	if(g_teck==66)
	{

		Timer1_deInit();
		g_teck=0;

		gflag=1;

	}
	else if(g_teck<30)
	{
		LCD_moveCursor(0,0);
		LCD_displayString("Door_is _ ");

		LCD_moveCursor(1,0);
		LCD_displayString("Unlocking    ");

	}


	else if(g_teck<66) //display
	{
		LCD_moveCursor(0,0);
		LCD_displayString("Door_is_  ");

		LCD_moveCursor(1,0);

		LCD_displayString("Locking    ");
	}




}


void wrong(void)
{


	Timer1_setCallBack(errorPass);

	Timer1_ConfigType t1Config={0,0,PS_64_T1,NONE,TOGGLE_T1};


	Timer1_init(&t1Config); //enable

	LCD_clearScreen();

	LCD_moveCursor(0,0);


	LCD_displayString("ERROR!!! ");

	while(gflag==0);

	gflag=0;


}



void errorPass(void)
{
	g_teck++;
	if(g_teck==120)
	{
		gflag=1;
		Timer1_deInit();
		g_teck=0;
	}
}
