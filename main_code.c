#include "AT89S52.h"

#define SEGMENT_PORT Port0
#define D1 Port2_P0
#define D2 Port2_P1
#define D3 Port2_P2
#define D4 Port2_P3
#define D5 Port2_P4
#define D6 Port2_P5
#define SDA Port1_P0
#define SCL Port1_P1

// Button pins (configure these as per your Proteus wiring)
#define BTN_SET  Port3_P0
#define BTN_INC  Port3_P1
#define BTN_NEXT Port3_P2

// Variables
unsigned char hr, min, sec;
unsigned char sec_tens, sec_units, min_tens, min_units, hr_tens, hr_units;
unsigned int a = 0, b = 0;
bit mode = 0;       // 0 = normal, 1 = time set
unsigned char position = 0;  // 0 = sec, 1 = min, 2 = hr

char ack;
unsigned char seg_code[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

// Delay
void delay_us(unsigned int us) { while(us--); }
void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++) for(j = 0; j < 1275; j++);
}

// I2C Communication
void I2C_Start() { SDA=1; SCL=1; delay_us(5); SDA=0; delay_us(5); SCL=0; }
void I2C_Stop() { SDA=0; SCL=1; delay_us(5); SDA=1; delay_us(5); }
void I2C_Write(unsigned char dat) {
    unsigned char i;
    for(i=0; i<8; i++) {
        SDA = (dat & 0x80);
        SCL = 1; delay_us(5); SCL = 0; dat <<= 1;
    }
    SDA=1; SCL=1; delay_us(5); SCL=0;
}
unsigned char I2C_Read(unsigned char ack) {
    unsigned char i, dat=0;
    SDA=1;
    for(i=0;i<8;i++) {
        SCL=1; dat<<=1;
        if(SDA) dat|=1;
        SCL=0; delay_us(5);
    }
    SDA = (ack) ? 0 : 1;
    SCL=1; delay_us(5); SCL=0; SDA=1;
    return dat;
}

unsigned char BCD_to_Decimal(unsigned char bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}
unsigned char Decimal_to_BCD(unsigned char dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

void RTC_ReadTime(unsigned char *hr, unsigned char *min, unsigned char *sec) {
    I2C_Start(); I2C_Write(0xD0); I2C_Write(0x00);
    I2C_Start(); I2C_Write(0xD1);
    *sec = BCD_to_Decimal(I2C_Read(1));
    *min = BCD_to_Decimal(I2C_Read(1));
    *hr  = BCD_to_Decimal(I2C_Read(0));
    I2C_Stop();
}

void RTC_SetTime(unsigned char hr, unsigned char min, unsigned char sec) {
    I2C_Start(); I2C_Write(0xD0); I2C_Write(0x00);
    I2C_Write(Decimal_to_BCD(sec));
    I2C_Write(Decimal_to_BCD(min));
    I2C_Write(Decimal_to_BCD(hr));
    I2C_Stop();
}

// Timer interrupt for display
void timer_interrupt() interrupt 1 {
    TH0 = 0xDB; TL0 = 0xFE;
    Port2 = 0x00;

    sec_tens = sec / 10;
    sec_units = sec % 10;
    min_tens = min / 10;
    min_units = min % 10;
    hr_tens = hr / 10;
    hr_units = hr % 10;
		if (b==100) {b=0; Port2_P6 = Port2_P6 ^ 1;}
    if (a == 1)      { D1=1; D2=D3=D4=D5=D6=0; SEGMENT_PORT = seg_code[sec_units]; }
    else if (a == 2) { D2=1; D1=D3=D4=D5=D6=0; SEGMENT_PORT = seg_code[sec_tens]; }
    else if (a == 3) { D3=1; D1=D2=D4=D5=D6=0; SEGMENT_PORT = seg_code[min_units]; }
    else if (a == 4) { D4=1; D1=D2=D3=D5=D6=0; SEGMENT_PORT = seg_code[min_tens]; }
    else if (a == 5) { D5=1; D1=D2=D3=D4=D6=0; SEGMENT_PORT = seg_code[hr_units]; }
    else if (a == 6) { D6=1; D1=D2=D3=D4=D5=0; SEGMENT_PORT = seg_code[hr_tens]; }
    else a=0;

    a++;
		b++;
}

// Button handling
void check_buttons() {
    static bit last_set = 1, last_inc = 1, last_next = 1;

    if(BTN_SET == 0 && last_set == 1) {
        delay_ms(20);
        if(BTN_SET == 0) {
            mode = !mode;
            if(mode == 0) RTC_SetTime(hr, min, sec);
        }
    }
    last_set = BTN_SET;

    if(mode) {
        if(BTN_NEXT == 0 && last_next == 1) {
            delay_ms(20);
            if(BTN_NEXT == 0) position = (position + 1) % 3;
        }
        last_next = BTN_NEXT;

        if(BTN_INC == 0 && last_inc == 1) {
            delay_ms(20);
            if(BTN_INC == 0) {
                switch(position) {
                    case 0: sec = (sec + 1) % 60; break;
                    case 1: min = (min + 1) % 60; break;
                    case 2: hr  = (hr + 1)  % 24; break;
                }
            }
        }
        last_inc = BTN_INC;
    }
}

void timer_start() {
    TL0 = 0xDB;
    TH0 = 0xFE;
    TMOD = 0x01;
    ET0 = 1;
    EA = 1;
    TR0 = 1;
}

void main() {
    timer_start();
    while(1) {
        check_buttons();
        if(!mode) RTC_ReadTime(&hr, &min, &sec);
        delay_ms(50);
    }
}
