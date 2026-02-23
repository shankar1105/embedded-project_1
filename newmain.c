#include <xc.h>
#include <stdio.h>
#include <string.h>

// CONFIGURATION BITS (adjust for your PIC device)
#pragma config FOSC = HS     // High-speed oscillator
#pragma config WDTE = OFF    // Watchdog Timer disabled
#pragma config PWRTE = OFF    // Power-up Timer enabled
#pragma config BOREN = ON    // Brown-out Reset enabled
#pragma config LVP = OFF     // Low-voltage programming disabled
#pragma config CPD = OFF     // Data EEPROM memory code protection off
#pragma config WRT = OFF     // Flash Program Memory Write Enable off
#pragma config CP = OFF      // Flash Program Memory Code Protection off

#define _XTAL_FREQ 20000000  // 20 MHz crystal

// Keypad
#define ROW1   RB0
#define ROW2   RB1
#define ROW3   RB2
#define ROW4   RB3
#define COL1   RB4
#define COL2   RB5
#define COL3   RB6

// LCD
#define LCD_RS RD2
#define LCD_EN RD3
#define LCD_D4 RD4
#define LCD_D5 RD5
#define LCD_D6 RD6
#define LCD_D7 RD7

// Buzzer
#define BUZZER RA0

// Admin Interrupt
#define ADMIN_INT RB0

char enteredPIN[5];        
char storedPIN[5];         
unsigned char attemptCount = 0;
unsigned char  adminOverride = 0;

void Keypad_Init(void) {
    TRISB = 0b11111000; 
    PORTB = 0x00;       
    
    OPTION_REGbits.nRBPU = 0;
}
char Keypad_Scan(void) {
    const char keymap[4][3] = {
        {'1','2','3'},
        {'4','5','6'},
        {'7','8','9'},
        {'*','0','#'}
    };
    for (int row = 0; row < 4; row++) {
        
        PORTB &= 0xF0;
        PORTB |= (1 << row);
        __delay_us(10); 
        for (int col = 0; col < 3; col++) {
            if (PORTB & (1 << (col + 4))) {
             
                while (PORTB & (1 << (col + 4)));
                __delay_ms(20); 
                return keymap[row][col];
            }
        }
    }
    return 0; 
}

void LCD_Cmd(unsigned char cmd);
void LCD_Char(unsigned char data);
void LCD_Init(void);
void LCD_String(const char *msg);

unsigned char EEPROM_Read(unsigned char addr);
void EEPROM_Write(unsigned char addr, unsigned char data);
void  Admin_Reset(void);



void LCD_Init(void) {
    TRISD &= 0x03; 
    __delay_ms(20);
    LCD_Cmd(0x02); 
    LCD_Cmd(0x28); 
    LCD_Cmd(0x0C); 
    LCD_Cmd(0x06); 
    LCD_Cmd(0x01); 
    __delay_ms(2);
}

void LCD_Cmd(unsigned char cmd) {
  
    PORTD = (PORTD & 0x0F) | (cmd & 0xF0);
    LCD_RS = 0;
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(100);

    PORTD = (PORTD & 0x0F) | ((cmd << 4) & 0xF0);
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_ms(2);
}

void LCD_Char(unsigned char data) {
   
    PORTD = (PORTD & 0x0F) | (data & 0xF0);
    LCD_RS = 1;
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_us(100);


    PORTD = (PORTD & 0x0F) | ((data << 4) & 0xF0);
    LCD_EN = 1;
    __delay_us(1);
    LCD_EN = 0;
    __delay_ms(2);
}

void LCD_String(const char *str) {
    while (*str) LCD_Char(*str++);
}

void LCD_SetCursor(unsigned char row, unsigned char col) {
    unsigned char addr = (row == 1) ? 0x80 : 0xC0;
    LCD_Cmd(addr + col);
}

void EEPROM_Write(unsigned char address, unsigned char data) {
    while (EECON1bits.WR); 
    EEADR = address;
    EEDATA = data;
    EECON1bits.EEPGD = 0;
    EECON1bits.WREN = 1;  
    INTCONbits.GIE = 0;   
    EECON2 = 0x55;       
    EECON2 = 0xAA;
    EECON1bits.WR = 1;    
    INTCONbits.GIE = 1;   
    EECON1bits.WREN = 0;  
}

unsigned char EEPROM_Read(unsigned char address) {
    EEADR = address;
    EECON1bits.EEPGD = 0; 
    EECON1bits.RD = 1;    
    return EEDATA;
}

void Buzzer_Init(void) {
    TRISA0 = 0; 
    BUZZER = 0; 
}

void Buzzer_On(void) {
    BUZZER = 1;
}

void Buzzer_Off(void) {
    BUZZER = 0;
}

void Buzzer_Beep(unsigned int duration_ms) {
    Buzzer_On();
    __delay_ms(100);
    Buzzer_Off();
}

void Interrupt_Init(void) {
    TRISB0 = 1; 
    OPTION_REGbits.INTEDG = 0; 
    INTCONbits.INTF = 0; 
    INTCONbits.INTE = 1; 
    INTCONbits.GIE = 1; 
}

void __interrupt() ISR(void) {
    if (INTCONbits.INTF) {
        adminOverride = 1; 
        INTCONbits.INTF = 0; 
    }
}

void System_Init(void) {
    Keypad_Init();
    LCD_Init();
    Buzzer_Init();
    Interrupt_Init();
   
    for (int i = 0; i < 4; i++) {
        storedPIN[i] = EEPROM_Read((unsigned char)i);

    }
    storedPIN[4] = '\0';
   
    if (storedPIN[0] == 0xFF || storedPIN[1] == 0xFF) {
        strcpy(storedPIN, "1234");
        for (int i = 0; i < 4; i++) 
            EEPROM_Write((unsigned char)i, storedPIN[i]);


    }
}

void main(void) {
    System_Init();
    while (1) {
        LCD_Cmd(0x01);
        LCD_SetCursor(1, 0);
        LCD_String("Enter PIN:");
        LCD_SetCursor(2, 0);
        memset(enteredPIN, 0, sizeof(enteredPIN));
        int idx = 0;
        while (idx < 4) {
            char key = Keypad_Scan();
            if (key) {
                if (key >= '0' && key <= '9') {
                    enteredPIN[idx++] = key;
                    LCD_Char('*');
                } else if (key == '*') {
                   
                    idx = 0;
                    LCD_SetCursor(2, 0);
                    LCD_String("                ");
                    LCD_SetCursor(2, 0);
                } else if (key == '#') {
                   
                    break;
                }
            }
            if (adminOverride) {
                Admin_Reset();
                adminOverride = 0;
                break;
            }
        }
        enteredPIN[4] = '\0';
        if (idx == 4) {
            if (strcmp(enteredPIN, storedPIN) == 0) {
                LCD_Cmd(0x01);
                LCD_SetCursor(1, 0);
                LCD_String("Access Granted");
                Buzzer_Beep(100);
                __delay_ms(2000);
                attemptCount = 0;
                
            } else {
                attemptCount++;
                LCD_Cmd(0x01);
                LCD_SetCursor(1, 0);
                LCD_String("Wrong PIN!");
                Buzzer_Beep(500);
                __delay_ms(2000);
                if (attemptCount >= 3) {
                    LCD_Cmd(0x01);
                    LCD_SetCursor(1, 0);
                    LCD_String("LOCKED!");
                    for (int i = 0; i < 5; i++) {
                        Buzzer_Beep(200);
                        __delay_ms(200);
                    }
                    attemptCount = 0;
                }
            }
        }
    }
}
void Admin_Reset(void) {
    LCD_Cmd(0x01);
    LCD_SetCursor(1, 0);
    LCD_String("Admin Override");
    LCD_SetCursor(2, 0);
    LCD_String("New PIN:");
    char newPIN[5] = {0};
    int idx = 0;
    while (idx < 4) {
        char key = Keypad_Scan();
        if (key && key >= '0' && key <= '9') {
            newPIN[idx++] = key;
            LCD_Char('*');
        }
    }
    newPIN[4] = '\0';
   
for (unsigned char i = 0; i < 4; i++) {
    EEPROM_Write(i, newPIN[i]);
}

    strcpy(storedPIN, newPIN);
    LCD_Cmd(0x01);
    LCD_SetCursor(1, 0);
    LCD_String("PIN Changed!");
    Buzzer_Beep(200);
    __delay_ms(2000);
}