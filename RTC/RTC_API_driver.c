#include "RTC_API_header.h"
#define _XTAL_FREQ 16000000
#define EN RC0
#define RS RC1
//#define RS_data (unsigned char)*dataC|0x02
//#define RS_cmd (unsigned char)*dataC&0xFD
//#define EN (unsigned char)*dataC|0x01
//#define DIS (unsigned char)*dataC&~0x01
void i2c_init(unsigned long baud) {
    *SSPCON_1 = 0X28;
    *SSP_STAT = 0X00;
    *SSPCON_2 = 0X00;
    *SSP_ADD = (_XTAL_FREQ/(4*baud))-1;
}
void i2c_wait() {
    while((*SSP_STAT)&0X04 || (*SSPCON_2&0X1F));    //wait until the bus is idle
}
void i2c_start() {
    i2c_wait();
    (*SSPCON_2)|=0x01;                      //SEN
}
void i2c_rep_start() {
    i2c_wait();
    (*SSPCON_2)|=0X02;                      //RSEN
}
void i2c_stop() {
    i2c_wait();
    (*SSPCON_2)|=0x04;                      //PEN
}
unsigned char i2c_read() {
    int val;
    i2c_wait();
    (*SSPCON_2)|=0X08;                      //RCEN
    i2c_wait();
    if((*PIF1)&0x08)
        (*PIF1)&=~0x08;
    val = *SSP_BUF;
    i2c_wait();
    (*SSPCON_2)|=(((*SSPCON_2)&0x40)?0:1)&0x20; //ACKDT
    (*SSPCON_2)|=0x10;                      //ACKEN
    return val;
}
unsigned char i2c_write(unsigned char data) {
    i2c_wait();
    *SSP_BUF = data;
    if((*PIF1)&0x08)                       //SSPIF
        (*PIF1)&=~0x08;
    return (*SSPCON_2)&0x40;                //ACKSTAT
}
//void lcd_cmd(unsigned char i) {
//    *dataC &= 0xFD;
//    *dataD = i;
//    *dataC |= 0x01;
//    delay(5);   //although __delay_ms() doesn't work here
//    *dataC &= 0xFC;
//}
//void lcd_data(unsigned char i) {
//    *dataC |= 0x02;
//    *dataD = i;
//    *dataC |= 0x01;
//    delay(5);
//    *dataC &= 0xFC;
//}
void lcd_cmd(unsigned char i) {
    RS = 0;
    *dataD = i;
    EN = 1;
    delay(5);
    EN = 0;
}
void lcd_data(unsigned char i) {
    RS = 1;
    *dataD = i;
    EN = 1;
    delay(5);
    EN = 0;
}
//void lcd_cmd(unsigned char i) {
//    RS_cmd;
//    *dataD = i;
//    EN;
//    delay(5);
//    DIS;
//}
//void lcd_data(unsigned char i) {
//    RS_data;
//    *dataD = i;
//    EN;
//    delay(5);
//    DIS;
//}
void init() {
    *DRC = 0X18;
    *dataC = 0x00;
    *DRD = 0x00;
    lcd_cmd(0x38);
    delay(4);
    lcd_cmd(0x38);
    delay(4);
    lcd_cmd(0x38);
    delay(4);
    lcd_cmd(0x38);
    delay(4);
    lcd_cmd(0x0C);
    delay(4);
    lcd_cmd(0x06);
    delay(4);
    lcd_cmd(0x01);
    delay(4);
}
void days(int day) {
    switch(day) {       //To diplay day on LCD
            case 1:
                lcd_data(' '); lcd_data('S');
                lcd_data('U'); lcd_data('N');
                break;
            case 2:
                lcd_data(' '); lcd_data('M');
                lcd_data('O'); lcd_data('N');
                break;
            case 3:
                lcd_data(' '); lcd_data('T');
                lcd_data('U'); lcd_data('E');
                break;
            case 4:
                lcd_data(' '); lcd_data('W');
                lcd_data('E'); lcd_data('D');
                break;
            case 5:
                lcd_data(' '); lcd_data('T');
                lcd_data('H'); lcd_data('U');
                break;
            case 6:
                lcd_data(' '); lcd_data('F');
                lcd_data('R'); lcd_data('I');
                break;
            case 7:
                lcd_data(' '); lcd_data('S');
                lcd_data('A'); lcd_data('T');
                break;
        }
}
int dec2bcd(int temp) {
    return ((temp/10)<<4)+((temp%10));
}
int bcd2dec(int temp) {
    return ((temp>>4)*10)+((temp&0x0F));
}
void setup() {
    i2c_start();                    //S
    i2c_write(0xD0);                //Slave address with WR
    i2c_write(0x00);                //00h
    i2c_write(dec2bcd(sec));        
    i2c_write(dec2bcd(min));
    i2c_write(dec2bcd(hour)|0x60);  //12hour + PM //0x40 for AM //0x60 for PM
    i2c_write(dec2bcd(day));
    i2c_write(dec2bcd(date));
    i2c_write(dec2bcd(month));
    i2c_write(dec2bcd(year));
    i2c_stop();                     //P
}
void update() {
    i2c_start();                    //s
    i2c_write(0xD0);                //Slave address with WR
    i2c_write(0x00);                //location 00h
    i2c_stop();                     //P
//    i2c_rep_start();
    i2c_start();                    //s
    i2c_write(0xD1);                //Slave address with RD
    sec = bcd2dec(i2c_read());      //reads at 00h
    min = bcd2dec(i2c_read());      //01h
    hour = bcd2dec(i2c_read());     //02h
    day = bcd2dec(i2c_read());
    date = bcd2dec(i2c_read());
    month = bcd2dec(i2c_read());
    year = bcd2dec(i2c_read());
    i2c_stop();
    i2c_start();                    //To secure data from garbage data
    i2c_write(0xD1);
    i2c_read();
    i2c_stop();
}
void delay(unsigned char del) {
    del*=(_XTAL_FREQ/4000.0);
    while(--del);
}