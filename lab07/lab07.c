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

void wypisz(uint8_t b,int command_mode)
{
	uint8_t temp = LCD_PORT;
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
	_delay_ms(20);
    wypisz(0b00000110,LCD_COMMAND);
	_delay_ms(20);
    wypisz(0b00001100,LCD_COMMAND);
	_delay_ms(20);
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

void klawiatura(){
	uint8_t i,wyjscie = 0x00;
	for (i=4 ; i<8 ; i++){
	    PORTA = ~_BV(i); // u¿ycie makra
	        stan = PINA & 0x0F; // odczytanie stanu PINA i maskowanie
	        switch(stan) { // dopasowanie stanu PINA do dzia³ania programu
	            case 0b00001111: 
	                wyjscie = 0; // obliczone stany na wyjœciu
	                break;
	            case 0b00001110:
	                wyjscie = i-3;
	                break;
	            case 0b00001101:
	                wyjscie = 4+i-3;
	                break;
	            case 0b00001011:
	                wyjscie = 8+i-3;
	                break;
	            case 0b00000111:
	                wyjscie = 12+i-3;
	                break;
	            default:
	                break;
	        };
		PORTC = wyjscie; // wyœwietlenie stanu na linijce LED
		switch(PORTC){
		case 1:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("1",1);
			break;
		case 2:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("2",1);
			break;
		case 3:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("3",1);
			break;
		case 4:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("4",1);
			break;
		case 5:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("5",1);
			break;
		case 6:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("6",1);
			break;
		case 7:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("7",1);
			break;
		case 8:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("8",1);
			break;
		case 9:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("9",1);
			break;
		case 10:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("10",2);
			break;
		case 11:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("11",2);
			break;
		case 12:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("12",2);
			break;
		case 13:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("13",2);
			break;
		case 14:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("14",2);
			break;
		case 15:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("15",2);
			break;
		case 16:
			czyscLCD_y(13);
			ustawKursor(1,14);
			napisz("16",2);
			break;
		}
    }
}
int main(void)
{
	DDRA = 0xF0; // ustawienie czêœci portu A w stan wejœcia
    PORTA = 0x0F; // podci¹gniêcie portu A
    DDRC |= 0x1F; // ustawienie 5 bitów portu C w stan wyjœcia
    PORTC = 0x01; // stan pocz¹tkowy portu C
    TCCR0 |= 1 << WGM01 | 1 << CS00 | 1 << CS02; // tryb CTC i preskaler 1024
    TIMSK |= 1 << OCIE0; // flaga przepe³nienia
    OCR0 = 5; // czas 5ms 
    TCNT0 = 0; // wartoœæ startowa timera
    sei();
	inicjalizacjaLCD();
	char tekst1[]={'M','a','k','s','y','m','i','u','k',' ','K','.'};
	char tekst2[]={'P','r','z','y','c','i','s','k',' ','n','r',':'};
	
	czyscLCD();
	ustawKursor(0,0);
	napisz(tekst1,12);
	ustawKursor(1,0);
	napisz(tekst2,12);
    while(1){
	}
}
/* zad 1 
int main(void)
{
	inicjalizacjaLCD();
	char tekst[]={'H','e','l','l','o',' ','W','o','r','l','d','!'};
	//czyscLCD();
    while(1){
	    ustawKursor(1,1);
		_delay_ms(500);
	    napisz(tekst);   //wyswietlenie tekstu
		_delay_ms(500);
		czyscLCD_y(1);
		
    }
}
*/
