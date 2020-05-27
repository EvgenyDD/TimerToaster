/* Includes ------------------------------------------------------------------*/
#include <ioavr.h>
#include <intrinsics.h>
//#include <stdlib.h>
//#include <math.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FREQ 8000000UL
#define RELAY PORTD_Bit4
#define LEDS PORTD_Bit6


/* Private macro -------------------------------------------------------------*/
#define _delay_us(us)          __delay_cycles((FREQ/1000000)*(us))
#define _delay_ms(ms)          __delay_cycles((FREQ/1000)*(ms))
#define _delay_s(s)            __delay_cycles((FREQ)*(s))

#define BitIsSet(reg, bit) ((reg & (1<<(bit))) != 0)


/* Private variables ---------------------------------------------------------*/
__eeprom __no_init unsigned char Eminute, Esecond;

__flash unsigned char segments[10] = { //abhdgefc
 40,190,35,38,180,100,
 96,62,32,36
};

unsigned char dot = 0;
unsigned char digit[3] = {0,0,0};
unsigned char mode = 0, minute = 0, second = 0;


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Display
* Description    : display data on display
*******************************************************************************/
void Display(void)
{
  static unsigned char i = 0;
    i++;
    if(i == 3) i = 0;
    
    //d0 d1 d2
    PORTD &= ~((1<<0)|(1<<1)|(1<<2));
    if(BitIsSet(dot,i)) 
    {
        unsigned char temp = segments[digit[i]];
        temp &= ~(1<<5);
        PORTB = temp;
    }
    else
        PORTB = segments[digit[i]];

//_delay_ms(2);    
    if(i==0) PORTD_Bit2 = 1;
    if(i==1) PORTD_Bit0 = 1;
    if(i==2) PORTD_Bit1 = 1;
        
}


/*******************************************************************************
* Function Name  : LEDProcess
* Description    : make the LEDs flashing
*******************************************************************************/
void LEDProcess(void)
{
    static unsigned int counter = 0;
    static unsigned char countflash = 0;
    
    if( mode == 1)
    {
        counter++;
        if( counter > 3000 && mode == 1)
        {
          counter = 0;
          countflash++;
          
          if(countflash >= minute + 1 )
          {
            PORTD ^= (1<<6);
            countflash = 0;
          } 
        }
    }
}


/*******************************************************************************
* Function Name  : Sound
* Description    : play melody
* Input          : melody number
*******************************************************************************/
void Sound(unsigned char type)
{
  static unsigned int count=0; 
  static unsigned char mcount=0, end=1, typeCurr=0;
  
    if(end == 1 && type < 5) 
    {
      typeCurr = type;
      mcount = 0;
      end = 0;
    }
  
    if(end == 0)
    {
        count++;
        
        if(count == 4000) // время между  
        {
            mcount++;
            count = 0;
        }
        
        //if(mcount > 3) mcount = 0;
        
        if(typeCurr == 1) //begin
        {
            if(mcount == 0) OCR0A = 0;
            if(mcount == 1) OCR0A = 50;//10;
            if(mcount == 2) OCR0A = 60;//12;
            if(mcount == 3) OCR0A = 70;//20;
            if(mcount == 4) {OCR0A = 0;mcount=0;end = 1;}
        }
        
        if(typeCurr == 2) //end
        {
            if(mcount == 0) OCR0A = 0;
            if(mcount == 1) OCR0A = 36;//15;
            if(mcount == 2) OCR0A = 28;//12;
            if(mcount == 3) OCR0A = 20;//10;
            if(mcount == 4) {OCR0A = 0;mcount=0;end = 1;}
        }
        
        if(typeCurr == 3) //minute
        {   
            if(mcount == 0) OCR0A = 0;
            if(mcount == 1) OCR0A = 20;
            if(mcount == 2) OCR0A = 20;
            if(mcount == 3) OCR0A = 20;
            if(mcount == 4) OCR0A = 0;
            if(mcount == 5) OCR0A = 0;
            if(mcount == 6) OCR0A = 0;
            if(mcount == 7) OCR0A = 0;
            if(mcount == 8) OCR0A = 0;
            if(mcount == 9) {OCR0A = 0;mcount=0;end = 1;}
        }
        
        if(typeCurr == 4) //10sec
        {
            if(mcount == 0) OCR0A = 0;
            if(mcount == 1) OCR0A = 20;
            if(mcount == 2) OCR0A = 0;
            if(mcount == 3) OCR0A = 20;
            if(mcount == 4) OCR0A = 0;
            if(mcount == 5) OCR0A = 0;
            if(mcount == 6) OCR0A = 0;
            if(mcount == 7) OCR0A = 0;
            if(mcount == 8) OCR0A = 0;
            if(mcount == 9) {OCR0A = 0;mcount=0;end = 1;}
        }
        
        if(typeCurr == 0) {OCR0A = 0;mcount=0;end = 1;}
    }
}
    
/*******************************************************************************
* Function Name  : main
* Description    : main routine
*******************************************************************************/
int main( void )
{
//I/O Initialization    
    DDRB = 255;
    DDRD = 247;
    PORTB = 0;
    PORTD = 8;

//Timer Initialization   
    TCCR0A |= (0<<COM0B1)|(1<<COM0B0)|(1<<WGM01)|(0<<WGM00);
    TCCR0B |= (0<<WGM02)|(0<<CS02)|(1<<CS01)|(1<<CS00); 
    TCCR1A |= (0<<WGM11)|(0<<WGM10);
    TCCR1B |= (1<<WGM12)|(0<<WGM13)|(1<<CS12)|(0<<CS11)|(1<<CS10);
    TIMSK |= (1 << OCIE0B)|(1<<OCIE1A);
    OCR0B = 0;
    
//Eminute = 3;
//Esecond = 0;
mode = 1;
    minute = 1;//Eminute;
    second = 20;//Esecond;	
    RELAY = 1;
    
    __enable_interrupt();
    
    while(1)
    {
        digit[0] = minute;
        digit[1] = second / 10;
        digit[2] = second % 10;
        dot = 1<<0;                          
                          
        Display();   
        LEDProcess();
        
        static unsigned int debounce = 0;
        
        if(PIND_Bit3 == 0 && debounce == 0)
        {
          debounce = 1000;
		  
          while(PIND_Bit3 == 0);
          if(mode == 1)
            mode = 2;
          else 
          {
            mode = 1;
            RELAY = 1;
            Sound(1);
          }
            
        }
        if(debounce) debounce--;
        
        if(mode == 2)
        {
            Sound(2);
            mode = 0;
            LEDS = 1;
            RELAY = 0;
            minute = Eminute;
            second = Esecond;	
        }
          
        if(minute == 1 && second == 0) Sound(3);
        if(minute == 0 && second == 20) Sound(4);
        //OCR0A = 10;
        
        Sound(0);
        
        OCR1A = 7817;
        OCR1B = 0;    
    }
}


/*******************************************************************************
* Function Name  : time
* Description    : Timer1 Interrupt routine
*******************************************************************************/
#pragma vector = TIMER1_COMPA_vect         
__interrupt void time(void) 
{
    //mode  == 0  простой
    //      == 1  рабочий режим
    //      == 2  закончился отсчет                                                                                                 
    
    if(mode == 1) //если рабочий режим
    { 
        second--;
        
        if(/*second == 00 ||*/ second > 59)
        {
            if(minute == 0)			
            {
                mode = 2;
                LEDS = 1;
                minute = Eminute;
                second = Esecond;				
            }
            else     //minute = (1 и больше) 
            {
                minute--;
                second = 59;
            }
        } 
        
    }
}