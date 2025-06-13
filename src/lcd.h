#ifndef LCD_H
#define LCD_H

void lcd_init(void);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_puts(const char* str);
void lcd_gotoxy(unsigned int x, unsigned int y);
void lcd_clear(void);
void lcd_cursor_on(void);
void lcd_cursor_off(void);

#endif
