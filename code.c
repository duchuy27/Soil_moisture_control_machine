// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator (HS))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = ON         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "pic18f4550.h"
#include "stdio.h"
#define _XTAL_FREQ 20000000     /* define _XTAL_FREQ for using internal delay */
#define RS LATE0                   /* PORTD 0 pin is used for Register Select */
#define EN LATE1                   /* PORTD 1 pin is used for Enable */
#define ldata LATD                /* PORTB is used for transmitting data to LCD */
#define vref 5.00 
#define LCD_Port TRISD              
#define LCD_Control TRISE

void LCD_Init();
void LCD_Command(char );
void LCD_Char(char x);
void LCD_String(const char *);
void LCD_String_xy(char ,char ,const char*);
void ADC_Init();
int ADC_Read(int);
void Timer0_Init_ISR();
void Timer0_Init(void);

int dem=0;
int Sensor1;
float mmhighLiquid;
int Sensor2;
float Humi;

int main(void){
    LCD_Init();
    ADC_Init();
    Timer0_Init_ISR();
    char buf[16] = {0,};
    TRISB = 0x00;
    TMR0ON=1;
    TMR0=40535;
    while(1){
        LATBbits.LATB1 = 0;
        Sensor1 = ADC_Read(0);
        mmhighLiquid = (float)(4/694.0)*Sensor1*10; //4cm/700 level   
        sprintf(buf, "%.2fmm  ", mmhighLiquid);
        LCD_Command(0x80);
        LCD_String(buf);
           
        Sensor2 = ADC_Read(1);
        Humi = 100.0 - ((Sensor2-300)/1023.0)*100.0;
        sprintf(buf, "%.2f  ", Humi);
        LCD_Command(0x88);
        LCD_String(buf);     
    }
    return 0;
}
/**************LCD INITIATE************************/
void LCD_Init(){
    __delay_ms(15);           /* 15ms,16x2 LCD Power on delay */
    LCD_Port = 0x00;       /* Set PORTD as output PORT for LCD data(D0-D7) pins */
    LCD_Control = 0x00;    /* Set PORTE as output PORT LCD Control(RS,EN) Pins */
    LCD_Command(0x38);     /* uses 2 line and initialize 5*7 matrix of LCD */
    LCD_Command(0x01);     /* clear display screen */
    LCD_Command(0x0c);     /* display on cursor off */
    LCD_Command(0x06);     /* increment cursor (shift cursor to right) */
}

void LCD_Clear(){
    	LCD_Command(0x01); /* clear display screen */
}

void LCD_Command(char cmd ){
	ldata= cmd;            /* Send data to PORT as a command for LCD */   
	RS = 0;                /* Command Register is selected */
	EN = 1;                /* High-to-Low pulse on Enable pin to latch data */ 
  NOP();
	EN = 0;
	__delay_ms(3);	
}
void LCD_Char(char dat){
	ldata= dat;            /* Send data to LCD */  
	RS = 1;                /* Data Register is selected */
	EN=1;                  /* High-to-Low pulse on Enable pin to latch data */   
	NOP();
	EN=0;
	__delay_ms(1);
}

void LCD_String(const char *msg){
	while((*msg)!=0){		
	  LCD_Char(*msg);
	  msg++;	
    	}	
}

void LCD_String_xy(char row,char pos,const char *msg){
    char location=0;
    if(row<=1){
        location=(0x80) | ((pos) & 0x0f); /*Print message on 1st row and desired location*/
        LCD_Command(location);
    }else{
        location=(0xC0) | ((pos) & 0x0f); /*Print message on 2nd row and desired location*/
        LCD_Command(location);    
    }  
    LCD_String(msg);
}

void Timer0_Init_ISR(){
    GIE=1; 
    PEIE=1;
    //TMR0IP=1;
    TMR0IE=1; 
    TMR0IF=0;
    Timer0_Init(); 
}

void __interrupt __priority(void){  
    TMR0=40535;
    if(Sensor1>250&&Humi<70.0){
        dem++; 
        if(dem>100 && dem<400){
            LATBbits.LATB1 = 1;
        }
        if(dem==400){
            LATBbits.LATB1 = 0;
            dem=0;  
            }
    }else{
        LATBbits.LATB1 = 0;
        dem=0;
    }
    TMR0IF=0;
}
