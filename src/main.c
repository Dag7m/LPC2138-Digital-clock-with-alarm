#include <NXP/iolpc2148.h>
#include "lcd.h"
#include <stdio.h>

#define SET  (1<<11)
#define INC  (1<<12)
#define DEC  (1<<13)
#define SAVE (1<<14)
#define VIEW (1 << 15)
#define BUZZER (1 << 16)  



unsigned int day = 1, month = 1, year = 2025;
unsigned int alarm_hour = 11, alarm_minute = 16, alarm_second = 0;
int view_mode = 0;  // 0 = show time, 1 = show alarm



int edit_mode = 0;
int edit_index = 0;
int prev_set = 1, prev_inc = 1, prev_dec = 1, prev_save = 1;

void delay_ms(unsigned int ms) {
    for (unsigned int i = 0; i < ms * 150; i++);
}

void rtc_init() {
    // Enable RTC clock
    PCONP |= (1 << 9); // Power up RTC
    CCR = 0x00; // Disable RTC before setting

    //default time & date
    SEC  = 30;
    MIN  = 15;
    HOUR = 11;
    DOM  = 1;
    MONTH = 1;
    YEAR = 2025;
    DOW  = 0;  

    CCR = 0x11; // Enable RTC and reset counters
}

void display_time_date() {
  
  if(edit_mode){
    char buf[21];

      lcd_gotoxy(0, 0);
      sprintf(buf, "%02d:%02d:%02d",HOUR, MIN, SEC);
      lcd_puts(buf);

      lcd_gotoxy(0, 1);
      sprintf(buf, "%02d/%02d/%02dA%02d:%02d", DOM, MONTH, YEAR, alarm_hour, alarm_minute);
      lcd_puts(buf);
  }else{
    char buf[17];

    lcd_gotoxy(0, 0);
    sprintf(buf, "%02d:%02d:%02d", HOUR, MIN, SEC);
    lcd_puts(buf);

    lcd_gotoxy(0, 1);
    sprintf(buf, "%02d/%02d/%04d", DOM, MONTH, YEAR);
    lcd_puts(buf);
  }
    
}

void update_cursor_position() {
    const unsigned char cursor_pos[] = {
        0, 1, 3, 4, 6, 7,       // hh:mm:ss
        0x40, 0x41, 0x43, 0x44, 0x46, // dd/mm/yyyy
        0x4B, 0x4C , 0x4E, 0x4F 
    };
    lcd_cmd(0x80 + cursor_pos[edit_index]);
    lcd_cmd(0x0F); // Cursor ON & blink
}

void clear_cursor() {
    lcd_cmd(0x0C); // Cursor OFF
}

void increment_field() {
    switch (edit_index) {
        case 0: HOUR = (HOUR + 10) % 24; break;
        case 1: HOUR = (HOUR + 1) % 24; break;
        case 2: MIN = (MIN + 10) % 60; break;
        case 3: MIN = (MIN + 1) % 60; break;
        case 4: SEC = (SEC + 10) % 60; break;
        case 5: SEC = (SEC + 1) % 60; break;
        case 6:
        case 7: DOM = (DOM % 31) + 1; break;
        case 8:
        case 9: MONTH = (MONTH % 12) + 1; break;
        case 10: YEAR++; break;
        
        //alarm
        case 11: alarm_hour = (alarm_hour + 10) % 24; break;
        case 12: alarm_hour = (alarm_hour + 1) % 24; break;
        case 13: alarm_minute = (alarm_minute + 10) % 60; break;
        case 14: alarm_minute = (alarm_minute + 1) % 60; break;
   
    }
}

void decrement_field() {
    switch (edit_index) {
        case 0: HOUR = (HOUR >= 10) ? HOUR - 10 : 23; break;
        case 1: HOUR = (HOUR > 0) ? HOUR - 1 : 23; break;
        case 2: MIN = (MIN >= 10) ? MIN - 10 : 59; break;
        case 3: MIN = (MIN > 0) ? MIN - 1 : 59; break;
        case 4: SEC = (SEC >= 10) ? SEC - 10 : 59; break;
        case 5: SEC = (SEC > 0) ? SEC - 1 : 59; break;
        case 6:
        case 7: DOM = (DOM > 1) ? DOM - 1 : 31; break;
        case 8:
        case 9: MONTH = (MONTH > 1) ? MONTH - 1 : 12; break;
        case 10: if (YEAR > 2000) YEAR--; break;
        // Alarm fields:
        case 11: alarm_hour = (alarm_hour >= 10) ? alarm_hour - 10 : 23; break;
        case 12: alarm_hour = (alarm_hour > 0) ? alarm_hour - 1 : 23; break;
        case 13: alarm_minute = (alarm_minute >= 10) ? alarm_minute - 10 : 59; break;
        case 14: alarm_minute = (alarm_minute > 0) ? alarm_minute - 1 : 59; break;
    }
}

//validation

int is_leap_year(unsigned int y) {
    return ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0));
}

unsigned int get_max_day(unsigned int m, unsigned int y) {
    switch (m) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            return is_leap_year(y) ? 29 : 28;
        default:
            return 31; // fallback safety
    }
}

void validate_date() {
    unsigned int max_day = get_max_day(MONTH, YEAR);
    if (DOM > max_day) {
        DOM = max_day;
    }
}

//validations_done


void apply_edited_time() {
  
    validate_date();  // <-- validate before applying
    CCR = 0x00; // Disable RTC
    SEC = SEC;
    MIN = MIN;
    HOUR = HOUR;
    DOM = DOM;
    MONTH = MONTH;
    YEAR = YEAR;
    CCR = 0x11; // Enable RTC and reset counters
}

void check_buttons() {
    int cur_set = IO0PIN & SET;
    int cur_inc = IO0PIN & INC;
    int cur_dec = IO0PIN & DEC;
    int cur_save = IO0PIN & SAVE;
    int cur_view = IO0PIN & VIEW;
    
    
    static int prev_view = 1;

    if (!cur_view && prev_view) {
        view_mode = !view_mode;
        lcd_clear();
        delay_ms(300);
    }
    prev_view = cur_view;


    if (!cur_set && prev_set) {
        if (!edit_mode) {
            edit_mode = 1;
            edit_index = 0;
        } else {
            edit_index++;
            if (edit_index > 14) edit_index = 14;
        }
        delay_ms(250);
    }

    if (!cur_inc && prev_inc && edit_mode) {
        increment_field();
        delay_ms(200);
    }

    if (!cur_dec && prev_dec && edit_mode) {
        decrement_field();
        delay_ms(200);
    }

    if (!cur_save && prev_save) {
        if (edit_mode) {
            apply_edited_time();
        }
        edit_mode = 0;
        clear_cursor();
        lcd_clear();
        delay_ms(300);
        CCR = 0x11;
    }
    
    if(edit_mode){
          CCR = 0x00;
          view_mode = 0;
    }
    

    prev_set = cur_set;
    prev_inc = cur_inc;
    prev_dec = cur_dec;
    prev_save = cur_save;
}

//alarm
void display_alarm_time() {
    char buf[17];
    
    if(alarm_hour != 99){
      int cur_total_sec = HOUR * 3600 + MIN * 60 + SEC;
      int alarm_total_sec = alarm_hour * 3600 + alarm_minute * 60 + alarm_second;

      int remaining_sec;

      if (alarm_total_sec >= cur_total_sec)
          remaining_sec = alarm_total_sec - cur_total_sec;
      else
          remaining_sec = (24 * 3600 - cur_total_sec) + alarm_total_sec;

      int rem_hour = remaining_sec / 3600;
      int rem_minute = (remaining_sec % 3600) / 60;
      int rem_second = remaining_sec % 60;

      lcd_gotoxy(0, 0);
      sprintf(buf, "%02d:%02d:%02d", alarm_hour, alarm_minute, alarm_second);
      lcd_puts(buf);

      lcd_gotoxy(0, 1);
      sprintf(buf, "Rem. %02d:%02d:%02d", rem_hour, rem_minute, rem_second);
      lcd_puts(buf);
    }
    else{
        lcd_gotoxy(0, 0);
        sprintf(buf, "%02d:%02d:%02d", alarm_hour, alarm_minute, alarm_second);
        lcd_puts(buf);

        lcd_gotoxy(0, 1);
        sprintf(buf, "alarm disabled!");
        lcd_puts(buf);
    }
}


// trigger alarm 
void trigger_alarm() {
    IO0SET = BUZZER;   // turn buzzer ON
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("   ALARM !!!");
    lcd_gotoxy(0, 1);
    lcd_puts("les's WAKE UP!");

    delay_ms(6000);     // alarm sound duration
    IO0CLR = BUZZER;    // turn buzzer OFF

    alarm_hour = 99;  // reset alarm
}




int main() {
    IO0DIR &= ~(SET | INC | DEC | SAVE); // Buttons as input
    IO0DIR |= BUZZER;
    IO0CLR = BUZZER;  // buzzer off initially
    lcd_init();
    rtc_init();
    
    lcd_puts("Da clock");
    delay_ms(300);

    while (1) {
        check_buttons();

        
        //lcd clear
        if (view_mode == 0) {
            display_time_date();
        } else {
            display_alarm_time();
        }
        
        if (!edit_mode) {
              if (SEC == alarm_second && MIN == alarm_minute && HOUR == alarm_hour) {
                  trigger_alarm();
              // Reset alarm
              lcd_clear();// Or set invalid hour to disable alarm
          }
      }

        if (edit_mode){
            while (edit_mode){
              update_cursor_position();
              lcd_cursor_on();
              delay_ms(100);
              lcd_cursor_off();
              delay_ms(100);
              check_buttons();
              if (view_mode == 0) {
              display_time_date();
          } else {
              display_alarm_time();
          }
          }
        }
    }
}
