
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>

#define F_CPU 1000000L

#define LCD_DDR DDRB
#define LCD_PORT PORTB
#define LCD_RS 1
#define LCD_EN 0
#define LCD_DB4 4
#define LCD_DB5 5
#define LCD_DB6 6
#define LCD_DB7 7

#define LCD_COMMAND 1
#define LCD_DATA 0

volatile pozX, pozY;

volatile uint8_t stan = 0;

void wypisz(uint8_t b){
	
	LCD_PORT |= _BV(LCD_EN);
	LCD_PORT = (b & 0xF0)|(LCD_PORT & 0x0F);
	LCD_PORT &= ~(_BV(LCD_EN));
	//asm volatile("nop");
	LCD_PORT |= _BV(LCD_EN);
	LCD_PORT = ((b & 0x0F)<<4)|(LCD_PORT & 0x0F);
	LCD_PORT &= ~(_BV(LCD_EN));
	//_delay_ms(1);

}

void czyscLCD(){

	pozX = 0;
	pozY = 0;
	wypisz(0x01);
//	_delay_ms(5);

}

void inicjalizacjaLCD(){


LCD_DDR = 0xFF;

//_delay_ms(50);
LCD_PORT = 0;

wypisz(0x02);
//_delay_ms(5);

LCD_PORT &= ~_BV(LCD_RS);
LCD_PORT |= _BV(LCD_EN);

_delay_ms(5);

wypisz(0b00101000);
_delay_ms(5);

wypisz(0b00000110);
_delay_ms(5);

wypisz(0b00001100);
_delay_ms(5);

czyscLCD();

}

int odczyt_klawiatury(){

int i=4;
int odczyt=0x00;
int wartosc=0x00;



//for(i=1;i<5;i++){

	PORTA = ~_BV((i+3));
	odczyt = PINA & 0x0F;

switch(odczyt){

	case 0b00001111:
		wartosc = 0;
		break;
	
	case 0b00001110:
		wartosc = 4*i-3;
		break;

	case 0b00001101:
		wartosc = 4*i-2;
		break;
	
	case 0b00001011:
		wartosc =4*i-1;
		break;

	case 0b00000111:
		
		wartosc =4*i;
		break;
	default:
		wartosc = 0xFF; // w momencie gdy naciskamy wiecej niz 1 przycisk
		break;

}// koniec switch


PORTC = wartosc;
//if(wartosc!=0){

//break;
//}

//} //koniec for

}// koniec funkcji odczytu

void menu(int wynik){


	/*
	if(poziom == 0){
	pozycja = 1;
		if(pozycja == 1){
	
		czyscLCD_y(0);
		czyscLCD_y(1);
		char tekst[] = {'M','l','y','n','e','k',' ','R','.',' ',' ',' ',' ',' ',' ',' ','P','r','z','y','c','i','s','k',' ','n','r',':','1'};
		napiszTekst(tekst,29);
		
		if(wynik == 14){
		pozycja = 2;
		}			
	
		}
		
		if(pozycja == 2){
	
		czyscLCD_y(0);
		czyscLCD_y(1);
		char tekst[] = {'M','l','y','n','e','k',' ','R','.',' ',' ',' ',' ',' ',' ',' ','P','r','z','y','c','i','s','k',' ','n','r',':','2'};
		napiszTekst(tekst,29);
		
		if(wynik == 14){
		pozycja = 3;
		}
	
		}

		if(pozycja == 3){
	
		czyscLCD_y(0);
		czyscLCD_y(1);
		char tekst[] = {'M','l','y','n','e','k',' ','R','.',' ',' ',' ',' ',' ',' ',' ','P','r','z','y','c','i','s','k',' ','n','r',':','3'};
		napiszTekst(tekst,29);
		
	
		}
	}*/

int poziom = 0;
int pozycja = 1;

if(poziom == 0)

	if(pozycja < 3){
		if(wynik == 14){
		pozycja = pozycja + 1;
		wynik =0;
		}
	}
	if(pozycja > 1){
		if(wynik == 13){
		pozycja = pozycja - 1;
		}
	}


if(poziom == 0 && pozycja == 1){

char tekst[] = {'M','l','y','n','e','k',' ','R','.',' ',' ',' ',' ',' ',' ',' ','P','r','z','y','c','i','s','k',' ','n','r',':','1'};
		napiszTekst(tekst,29);

}

if(poziom == 0 && pozycja == 2){

char tekst[] = {'M','l','y','n','e','k',' ','R','.',' ',' ',' ',' ',' ',' ',' ','P','r','z','y','c','i','s','k',' ','n','r',':','2'};
		napiszTekst(tekst,29);

}

if(poziom == 0 && pozycja == 3){

char tekst[] = {'M','l','y','n','e','k',' ','R','.',' ',' ',' ',' ',' ',' ',' ','P','r','z','y','c','i','s','k',' ','n','r',':','3'};
		napiszTekst(tekst,29);

}


}

int wynik2;
ISR(TIMER0_COMP_vect){

//odczyt_klawiatury();
wynik2 = odczyt_klawiatury();
menu(wynik2);

}



int main(){


DDRA = 0xF0;
PORTA = 0x0F;
DDRC = 0x1F;
PORTC = 0x00;

inicjalizacjaLCD();


TCCR0 |= 1 << WGM01; // ustawienia timera w tryb CTC
TCCR0 = (1 << CS00) | (0 << CS02); // ustawianie preskalera na 1/1024
TCNT0= 10;
OCR0 = 0; // opoznienie 5ms
TIMSK |= 1 << OCIE0;

TCNT0 = 0; //zerowanie rejestru w celu rozpoczenia liczenia od 0
TIMSK |= 1 << OCIE0;

sei(); // zezwolenie na przerwania

//czyscLCD_y(0);

while(1){
//int wynik = odczyt_klawiatury();
//menu(wynik);

}

}

void ustawKursor(unsigned char w, unsigned char h){

	LCD_PORT &= ~(_BV(LCD_RS));
	wypisz((w*0x40+h)|0x80);
	LCD_PORT |= _BV(LCD_RS);
	pozX = w;
	pozY = h;
	//_delay_ms(5);

}

void czyscLCD_y(unsigned char y){

	ustawKursor(y,0);
	for(int8_t i = 0; i < y;i++){
		wypisz(0x14);
	}
	while(y++ <= 16){
		wypisz(' ');
	}
}


void napiszTekst(char *tekst, int8_t dlug){
	ustawKursor(0,0);
	int8_t i = 0;
while(i < dlug){
	if(i == 16){
	ustawKursor(1,0);
	}
	wypisz(tekst[i]);
	i++;
}

}







