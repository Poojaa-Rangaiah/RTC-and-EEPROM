#include "RTC_API_header.h"
#define _XTAL_FREQ 16000000
void main(void) {
    init();
    i2c_init(10000);
    setup();                            //Writes only once 
    while(1) {
        update();                       //reads the updated time
        hour1=(dec2bcd(hour)&0x1F)-6;   //converts to conventional mode
        lcd_cmd(0x80);
        for(i=0; i<4; i++)
            lcd_data(msg1[i]);
        lcd_data(hour1/10+'0');
        lcd_data(hour1%10+'0');
        lcd_data('-');
        lcd_data(min/10+'0');
        lcd_data(min%10+'0');
        lcd_data('-');
        lcd_data(sec/10+'0');
        lcd_data(sec%10+'0');
        if(!(dec2bcd(hour)&0x20))       //checks if B5 is ~AM/PM
            for(i=0; i<3; i++)
                lcd_data(mode1[i]);     //AM
        else
            for(i=0; i<3; i++)
                lcd_data(mode2[i]);     //PM
        
        lcd_cmd(0xC0);
        for(i=0; i<4; i++)
            lcd_data(msg2[i]);
        lcd_data(date/10+'0');
        lcd_data(date%10+'0');
        lcd_data('-');
        lcd_data(month/10+'0');
        lcd_data(month%10+'0');
        lcd_data('-');
        lcd_data(year/10+'0');
        lcd_data(year%10+'0');
        if(dec2bcd(hour)&0x52) {    //Update the day when it is 12AM
            day++;
            if(day>7)
                day=1;
        }
        days(day);
        __delay_ms(100);        //only when __delay_ms() used, I2C works
    }
}