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
char menuProgram=0;
char menuPom=0;

void wypisz(uint8_t b,int command_mode)
{
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
    LCD_PORT = 0;   //ustawienie pocz�tkowego stanu niskiego na wszystkich liniach
    LCD_PORT &= ~(_BV(LCD_RS));  //rozpocz�cie wysylania komendy
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
    wypisz((w*0x40+h) | 0x80,LCD_COMMAND);
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

void klawiatura(){
	uint8_t i,wyjscie = 0x00;
	for (i=4 ; i<8 ; i++){
	    PORTA = ~_BV(i); // u�ycie makra
	    stan = PINA & 0x0F; // odczytanie stanu PINA i maskowanie
		switch(stan) { // dopasowanie stanu PINA do dzia�ania programu
		   case 0b00001111: 
		        break;
		   case 0b00001110: // up
		   		if (i==7) {
					if(liniaMenu!=1 && menuProgram!=1)
		            	liniaMenu--;
					if(liniaMenu!=1 && menuPom==1)
						liniaMenu=3;
					if(liniaMenu!=1 && menuProgram==1){
						menu2();
						liniaMenu--;
					} else
						menu();
				} 
		        break;
		   case 0b00001101: // down
		   		if (i==7) {
					if(liniaMenu!=3 && idMenu==0){
		            	liniaMenu++;
						menu();
					} else if(idMenu==1 && menuProgram==1 && liniaMenu!=2){
						menu2();
						liniaMenu++;
					}
				}
		        break;
		   case 0b00001011: // clear(escape) 
		   		if (i==7) {
					if(idMenu!=0){
			   			idMenu--;
					} else if(menuProgram==1){
						liniaMenu=2;
						menuProgram=0;
					} else if(menuPom==1){
						menuPom=0;
					} else if(menuPom==0)
						menu();
				} 
		        break;
		   case 0b00000111: // enter
		   		if (i==7) {
					if (idMenu!=2)
			   			idMenu++;
					menu();	
					if(menuPom==1 && idMenu==2 && liniaMenu==3)
						menu1();
				}					
		        break;
		   default:
		        break;
				
		};	
	}
}

void menu(){
	if (idMenu == 0){
		menuProgram=0;
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
	} else if(idMenu==1 && liniaMenu==1 && menuProgram==0){
		menu1();
		menuPom=1;
	} else if(idMenu==1 && liniaMenu==2 && menuProgram==0){
		menuProgram=1;
		liniaMenu==1;
		menu2();
	} else if(idMenu==1 && liniaMenu==3 && menuProgram==0){
		menuPom=1;
		menu3();
	} 
		
}
void menu1(){
	czyscLCD();
	ustawKursor(0,0);
	napisz("1.Przyciski",11);
}
void menu2(){
	switch(liniaMenu){
	case 1:
		czyscLCD();
		ustawKursor(0,0);
		napisz("2.1 Timer",9);
		ustawKursor(1,0);
		napisz("2.2 Stoper",10);
		ustawKursor(1,15);
		napisz("*",1);
	break;
	case 2:
		czyscLCD();
		ustawKursor(0,0);
		napisz("2.1 Timer",9);
		ustawKursor(1,0);
		napisz("2.2 Stoper",10);
		ustawKursor(0,15);
		napisz("*",1);
	break;
	default:
		break;
	};
}
void menu3(){
	czyscLCD();
	ustawKursor(0,0);
	napisz("Menu1",5);
}
int main(void)
{
	DDRA = 0xF0; // ustawienie cz�ci portu A w stan wej�cia
    PORTA = 0x0F; // podci�gni�cie portu A
    DDRC = 0x0F; // ustawienie 5 bit�w portu C w stan wyj�cia
    PORTC = 0x00; // stan pocz�tkowy portu C
    TCCR0 |= 1 << WGM01 | 1 << CS00 | 1 << CS02; // tryb CTC i preskaler 1024
    TIMSK |= 1 << OCIE0; // flaga przepe�nienia
    OCR0 = 10; // czas 5ms 
    TCNT0 = 0; // warto�� startowa timera
    sei();
	inicjalizacjaLCD();
	czyscLCD();
    while(1){
	}
}



