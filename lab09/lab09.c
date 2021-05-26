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

volatile posX,posY;
volatile uint8_t stan=0;
char idMenu = 0;
char liniaMenu=1;
char menuProgram=1;
char menuPom=0;
char progTrwa=0;
char wykryjPrzycisk=0;
void wypisz(uint8_t b,int command_mode)
{
	//uint8_t temp = LCD_PORT;
	if(command_mode){
		LCD_PORT &= ~(_BV(LCD_RS)); //przestawia na lini RS wartosc na 0 po to by wyslac komende a nie dane
	} else {
		LCD_PORT |= _BV(LCD_RS);//przestawia linie RS na wartosc 1 odpowiadajaca wprowadzaniu danych
	}
    LCD_PORT |= _BV(LCD_EN);    //wlaczenie linii ENABLE
    LCD_PORT = (b & 0xF0) | (LCD_PORT & 0x0F);  //wyslanie 4 starszych bitow
    LCD_PORT &= ~(_BV(LCD_EN)); //potwierdzenie wyslania danych poprzez opadniecie ENABLE
    asm volatile("nop");    //oczekiwanie jednego cyklu
    LCD_PORT |= _BV(LCD_EN);
    LCD_PORT = ((b & 0x0F) << 4) | (LCD_PORT & 0x0F);   //wyslanie 4 mlodszych bitow
    LCD_PORT &= ~(_BV(LCD_EN));
    _delay_ms(20);  //odczekanie czasu na potwierdzenie wyslania danych
}
void czyscLCD()
{
	posX =0;
	posY=0;
    //LCD_PORT &= ~(_BV(LCD_RS));
    wypisz(0x01,LCD_COMMAND);   //wysylamy polecenie wyczyszczenia
    //LCD_PORT |= _BV(LCD_RS);    
    _delay_ms(5);
}

void inicjalizacjaLCD()
{
    LCD_DDR = (0xF0) | (_BV(LCD_RS)) | (_BV(LCD_EN));   //ustawienie kierunku wyjsciowego dla wszystkich lini
	_delay_ms(2);
    LCD_PORT = 0;   //ustawienie pocz¹tkowego stanu niskiego na wszystkich liniach
    LCD_PORT &= ~(_BV(LCD_RS));  //rozpoczêcie wysylania komendy
    wypisz(0x02,LCD_COMMAND);
	_delay_ms(2);
	//LCD_PORT &= ~(_BV(LCD_RS));
    //LCD_PORT |= _BV(LCD_EN);
	wypisz(0b00101000,LCD_COMMAND);
	_delay_ms(2);
    wypisz(0b00000110,LCD_COMMAND);
	_delay_ms(2);
    wypisz(0b00001100,LCD_COMMAND);
	_delay_ms(2);
    czyscLCD();
}

void ustawKursor(unsigned char w, unsigned char h)
{
    //LCD_PORT &= ~(_BV(LCD_RS));
    wypisz((w*0x40+h) | 0x80,LCD_COMMAND);
    //LCD_PORT |= _BV(LCD_RS);
	posX = w;
	posY = h;
    _delay_ms(5);

}

void napisz(char *tekst, int8_t dlugosc)
{
    int8_t i=0;
    while (i<dlugosc)   //przejscie po wszystkich znakach tekstu
    {
        if (i==16)  //jezeli trafi na 16 znak (ostatni), to przesun kursor na poczatek 2 linii
        {
            ustawKursor(1,0);
        }
        wypisz(tekst[i],LCD_DATA);   //wypisz i-ty znak tekstu
        i++;
    }
}

void czyscLCD_y(unsigned char y){
	ustawKursor(posX,0);
	for (uint8_t i=0; i<y ; i++)
		wypisz(0x14,LCD_COMMAND);
	while(y++ <= 16)
		wypisz(' ',LCD_DATA);
}
 
ISR(TIMER0_COMP_vect) { // przerwanie timer0
	klawiatura();   
}
/*
ISR(TIMER2_COMP_vect) { // przerwanie timer2
}
*/
void klawiatura(){
	uint8_t i,wyjscie = 0x00;
	for (i=4 ; i<8 ; i++){
	    PORTA = ~_BV(i); // u¿ycie makra
	    stan = PINA & 0x0F; // odczytanie stanu PINA i maskowanie
		switch(stan) { // dopasowanie stanu PINA do dzia³ania programu
		   case 0b00001111: 
		        break;
		   case 0b00001110: // up
		   		if (i==7) {
					if(liniaMenu!=1 && idMenu==0){
		            	liniaMenu--;
						menu();
					} else if(idMenu==1 && menuProgram==2 && liniaMenu!=1){
						liniaMenu--;
						menu2();
					} else if(idMenu==1 && menuProgram==3 && liniaMenu!=1){
						liniaMenu--;
						menu3();	
					}
				} else if (i==4 && wykryjPrzycisk==1){
					wyjscie=0x01;
					czyscLCD_y(15);
					ustawKursor(0,15);
					napisz("1",1);
				} 
		        break;
		   case 0b00001101: // down
		   		if (i==7) {
					if(liniaMenu!=3 && idMenu==0){
		            	liniaMenu++;
						menu();
					} else if(idMenu==1 && menuProgram==2 && liniaMenu!=2){
						liniaMenu++;
						menu2();
						
					} else if(idMenu==1 && menuProgram==3 && liniaMenu!=2){
						liniaMenu++;
						menu3();
						
					}
				} else if (i==4 && wykryjPrzycisk==1){
					wyjscie=0x02;
					czyscLCD_y(15);
					ustawKursor(0,15);
					napisz("4",1);
				}
		        break;
		   case 0b00001011: // clear(escape) 
				if (i==7) {
					wykryjPrzycisk=0;
					progTrwa=0;
					//menuPom=0;
					if(menuProgram==2){
						menuProgram=1;
						liniaMenu=2;
					} else if(menuProgram==3){
						menuProgram=1;
						liniaMenu=3;
					}						
					if(idMenu!=0)
			   			idMenu--;
					if (menuProgram==1)
						menu();
					else if (menuProgram==2)
						menu2();
				} else if (i==4 && wykryjPrzycisk==1){
					wyjscie=0x04;
					czyscLCD_y(15);
					ustawKursor(0,15);
					napisz("7",1);
				}
		        break;
		   case 0b00000111: // enter
		   		if (i==7) {
					if (idMenu!=2){
			   			idMenu++;
					}
					if (menuProgram==1)
						menu();
					else if (menuProgram==2)
						menu2();
					else if (menuProgram==3)
						menu3();
					if (menuProgram==3 && idMenu==2 && liniaMenu==1){
						menuProgram=1;
						menu1();
						idMenu=1;	
					}
					if (menuProgram==3 && idMenu==2 && liniaMenu==2){
						idMenu=1;
						diody();
						PORTC=0x00;	
					}
				}					
		        break;
		   default:
		        break;	
		};	
		//PORTC = liniaMenu;
	}
}

void menu(){
	if (idMenu == 0){
		menuProgram=1;
		menuPom=0;
		switch(liniaMenu){
		case 1:
			czyscLCD();
			ustawKursor(0,0);
			napisz("Menu1",5);
			ustawKursor(1,0);
			napisz("Menu2",5);
			ustawKursor(0,15);
			napisz("*",1);
			break;
		case 2:
			czyscLCD();
			ustawKursor(0,0);
			napisz("Menu1",5);
			ustawKursor(1,0);
			napisz("Menu2",5);
			ustawKursor(1,15);
			napisz("*",1);
			break;
		case 3:
			czyscLCD();
			ustawKursor(0,0);
			napisz("Menu2",5);
			ustawKursor(1,0);
			napisz("Menu3",5);
			ustawKursor(1,15);
			napisz("*",1);
			break;
		default:
			break;
		}; 
	} else if(idMenu==1 && menuProgram==1){
		switch(liniaMenu){
		case 1:
			menu1();
			break;
		case 2:
			menuProgram=2;
			liniaMenu=1;
			menu2();
			break;
		case 3:
			menuProgram=3;
			liniaMenu=1;
			menu3();
			break;
		default:
			break;
		}; 	
	} 
		
}
void menu1(){
	menuProgram=1;
	liniaMenu=1;
	czyscLCD();
	ustawKursor(0,0);
	napisz("1. Przyciski",12);
}
void menu2(){
	switch(liniaMenu){
	case 1:
		czyscLCD();
		ustawKursor(0,0);
		napisz("2.1 Timer",9);
		ustawKursor(1,0);
		napisz("2.2 Stoper",10);
		ustawKursor(0,15);
		napisz("*",1);
	break;
	case 2:
		czyscLCD();
		ustawKursor(0,0);
		napisz("2.1 Timer",9);
		ustawKursor(1,0);
		napisz("2.2 Stoper",10);
		ustawKursor(1,15);
		napisz("*",1);
	break;
	default:
		break;
	};
}
void menu3(){
	switch(liniaMenu){
	case 1:
		czyscLCD();
		ustawKursor(0,0);
		napisz("Menu 1",6);
		ustawKursor(1,0);
		napisz("3.1 Diody",9);
		ustawKursor(0,15);
		napisz("*",1);
	break;
	case 2:
		czyscLCD();
		ustawKursor(0,0);
		napisz("Menu 1",6);
		ustawKursor(1,0);
		napisz("3.1 Diody",9);
		ustawKursor(1,15);
		napisz("*",1);
	break;
	default:
		break;
	};
}

void diody(){ //zad3 lab1
	PORTC=0b10000000; // ustawienie stanu bitu 7
    for (int i = 7; i >= 0 ; i--){
       _delay_ms(300);
       PORTC^=_BV(i-1); ;
    }
}

int main(void)
{
	DDRA = 0xF0; // ustawienie czêœci portu A w stan wejœcia
    PORTA = 0x0F; // podci¹gniêcie portu A
    DDRC = 0xFF; // ustawienie 5 bitów portu C w stan wyjœcia
    PORTC = 0x00; // stan pocz¹tkowy portu C
	// timer0
    TCCR0 |= 1 << WGM01 | 1 << CS00 | 1 << CS02; // tryb CTC i preskaler 1024
    TIMSK |= 1 << OCIE0;// | 1<<OCIE2; // flaga przepe³nienia
    OCR0 = 10; // czas 5ms 
    TCNT0 = 0; // wartoœæ startowa timera
	/* timer2
	TIMSK |= 1<<OCIE2;
	TCCR2 |= 1 << WGM01 | 1 << CS00 | 1 << CS02;
	OCR2=10;
	TCNT2=0;
	*/
    sei();
	inicjalizacjaLCD();
	//char tekst1[]={'M','a','k','s','y','m','i','u','k',' ','K','.'};
	//char tekst2[]={'P','r','z','y','c','i','s','k',' ','n','r',':'};
	
	czyscLCD();
    while(1){
	}
}



