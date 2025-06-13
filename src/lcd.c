#include <NXP/iolpc2148.h>
#include "lcd.h"

#define RS (1 << 24)
#define EN (1 << 25)
#define LCD_DATA_MASK 0xFF0000  // P1.16–P1.23

void delay_m(unsigned int ms) {
    for (unsigned int i = 0; i < ms * 150; i++);
}

void lcd_enable() {
    IO1SET = EN;
    delay_m(2);
    IO1CLR = EN;
}

void lcd_cmd(unsigned char cmd) {
    IO1CLR = RS;
    IO1CLR = LCD_DATA_MASK;
    IO1SET = (cmd << 16) & LCD_DATA_MASK;
    lcd_enable();
}

void lcd_data(unsigned char data) {
    IO1SET = RS;
    IO1CLR = LCD_DATA_MASK;
    IO1SET = (data << 16) & LCD_DATA_MASK;
    lcd_enable();
}

void lcd_puts(const char* str) {
    while (*str) lcd_data(*str++);
}

void lcd_gotoxy(unsigned int x, unsigned int y) {
    unsigned char pos = (y == 1) ? 0xC0 : 0x80;
    lcd_cmd(pos + x);
}

void lcd_clear(void) {
    lcd_cmd(0x01);
    delay_m(2);
}

void lcd_cursor_on(void) {
    lcd_cmd(0x0F);  // Cursor on, blink on
}

void lcd_cursor_off(void) {
    lcd_cmd(0x0C);  // Cursor off
}

void lcd_init() {
    IO1DIR |= RS | EN | LCD_DATA_MASK;
    delay_m(20);
    lcd_cmd(0x38); // 8-bit, 2-line
    lcd_cmd(0x0C); // Display ON, cursor OFF
    lcd_cmd(0x01); // Clear display
    lcd_cmd(0x06); // Entry mode
}
