#include <p18f4580.h>

//================ CONFIGURATION BITS =================/
#pragma config OSC = HS      /* High-speed oscillator */
#pragma config WDT = OFF     /* Watchdog Timer OFF */
#pragma config LVP = OFF     /* Low Voltage Programming OFF */

//================ MACROS =================/
#define LCD_RS PORTCbits.RC0   /* RS pin connected to RC0 */
#define LCD_EN PORTCbits.RC1   /* EN pin connected to RC1 */
#define LCD_DATA PORTD         /* Data lines connected to PORTD */

//================ FUNCTION PROTOTYPES =================/
void delay_ms(unsigned int ms);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_init(void);
void lcd_print(unsigned int num);
void lcd_string(const rom char *str);   /* NEW: string display */
void adc_init(void);
unsigned int adc_read(void);


//================ MAIN FUNCTION =================/
void main(void)
{
    unsigned int adc_value;

    TRISA = 0xFF;    /* PORTA as input (AN0) */

    lcd_init();      /* Initialize LCD */
    adc_init();      /* Initialize ADC */

    while(1)
    {
        adc_value = adc_read();   /* Read ADC value */

        lcd_cmd(0x80);            /* Move cursor to first line */
        lcd_string("POT value : "); /* Display label */

        lcd_print(adc_value);     /* Display ADC value */

        delay_ms(200U);            /* Delay for stability */
    }
}


//================ DELAY FUNCTION =================/
/* Generates approximate delay in milliseconds */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;

    for(i = 0U; i < ms; i++)
    {
        for(j = 0U; j < 1000U; j++)
        {
            /* Do nothing (software delay) */
        }
    }
}

//================ LCD COMMAND FUNCTION =================/
/* Sends command to LCD */
void lcd_cmd(unsigned char cmd)
{
    LCD_RS = 0U;        /* Select command mode */
    LCD_DATA = cmd;    /* Put command on data bus */

    LCD_EN = 1U;        /* Enable high */
    delay_ms(2U);       /* Small delay */
    LCD_EN = 0U;        /* Enable low (latch command) */
}

//================ LCD DATA FUNCTION =================/
/* Sends data (character) to LCD */
void lcd_data(unsigned char data)
{
    LCD_RS = 1U;        /* Select data mode */
    LCD_DATA = data;   /* Put data on bus */

    LCD_EN = 1U;        /* Enable high */
    delay_ms(2U);       
    LCD_EN = 0U;        /* Latch data */
}

//================ LCD STRING FUNCTION =================/
/* Displays string on LCD */
void lcd_string(const rom char *str)
{
    while(*str != '\0')
    {
        lcd_data(*str);   /* Send each character */
        str++;            /* Move to next character */
    }
}

//================ LCD INITIALIZATION =================/
/* Initializes LCD in 8-bit mode */
void lcd_init(void)
{
    TRISD = 0x00;           /* PORTD as output (data) */
    TRISCbits.TRISC0 = 0U;   /* RC0 as output (RS) */
    TRISCbits.TRISC1 = 0U;   /* RC1 as output (EN) */

    delay_ms(20U);           /* LCD power ON delay */

    lcd_cmd(0x38);          /* 8-bit mode, 2 lines */
    lcd_cmd(0x0E);          /* Display ON, cursor OFF */
    lcd_cmd(0x06);          /* Auto increment cursor */
    lcd_cmd(0x01);          /* Clear display */
}

//================ PRINT INTEGER ON LCD =================/
/* Displays 4-digit number (0000–1023) */
void lcd_print(unsigned int num)
{
    unsigned char d[4U];

    d[0] = (num / 1000) + '0';        /* Thousands place */
    d[1] = ((num / 100) % 10) + '0';  /* Hundreds place */
    d[2] = ((num / 10) % 10) + '0';   /* Tens place */
    d[3] = (num % 10) + '0';          /* Units place */

    lcd_data(d[0]);
    lcd_data(d[1]);
    lcd_data(d[2]);
    lcd_data(d[3]);
}

//================ ADC INITIALIZATION =================/
/* Configures AN0 as analog input */
void adc_init(void)
{
    ADCON1 = 0x0E;   /* AN0 analog, rest digital */
    ADCON2 = 0xAC;   /* Right justified, acquisition time */
    ADCON0 = 0x01;   /* Select AN0 and enable ADC */
}

//================ ADC READ FUNCTION =================/
/* Reads analog value from AN0 */
unsigned int adc_read(void)
{
    unsigned int result; 

    ADCON0bits.GO = 1U;           /* Start conversion */

    while(ADCON0bits.GO);        /* Wait until conversion complete */ 

    result = ((unsigned int)ADRESH << 8U) | ADRESL; /* Combine result */

    return result;
}