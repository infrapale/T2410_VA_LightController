/******************************************************************************
    Menu4x2.cpp
*******************************************************************************
    Key module - key code
    Left Keypad             Right Keypad
    -----     -----         -----    -----
    |1-5|     |1-1|    !    |2-5|    |2-1|
    -----     -----         -----    -----
    |1-6|     |1-2|    !    |2-6|    |2-2|
    -----     -----         -----    -----
    |1-7|     |1-3|    !    |2-7|    |2-3|
    -----     -----         -----    -----
    |1-8|     |1-4|    !    |2-8|    |2-4|
    -----     -----         -----    -----

    Left Keypad   Label and INnex
    -------     ------- 
    |A (4)|     |B (0)| 
    -------     ------- 
    |C (5)|     |D (1)| 
    -------     ------- 
    |E (6)|     |F (2)| 
    -------     ------- 
    |G (7)|     |H (3)| 
    -------     ------- 

******************************************************************************/

#include "Arduino.h"
#include "main.h"
#include "io.h"
#include "atask.h"
#include <LiquidCrystal_PCF8574.h>
#include "menu4x2.h"
#include "va_signal.h"
#include "autom.h"
#include "supervisor.h"
#include "rtc.h"

#define TIMEOUT_MENU          10000
#define TIMEOUT_UPDATE        1000
#define TIMEOUT_BACK_LIGHT    30000
#define LOGIN_CODE_LEN        3

#define KEY_INDEX_A     4
#define KEY_INDEX_B     0           
#define KEY_INDEX_C     5           
#define KEY_INDEX_D     1           
#define KEY_INDEX_E     6           
#define KEY_INDEX_F     2           
#define KEY_INDEX_G     7           
#define KEY_INDEX_H     8            

typedef struct
{
  uint8_t row;
  uint8_t col;
} menu_def_st;

typedef struct 
{
   uint8_t  level;
   uint8_t  active_key;
   uint8_t  code_array[LOGIN_CODE_LEN];
   uint32_t bl_timeout_at;
   uint32_t menu_timeout_at;
   uint32_t menu_update_at;
   time_st  time;
} menu4x2_ctrl_st;

extern LiquidCrystal_PCF8574 lcd;
extern main_ctrl_st main_ctrl;

atask_st menu_timeout_task_handle  = {"Menu Timeout   ", 1000, 0, 0, 255, 0, 1, menu4x2_timeout_task };
uint8_t login_code[LOGIN_CODE_LEN] = {KEY_INDEX_B, KEY_INDEX_A, KEY_INDEX_F};

menu4x2_ctrl_st menu4x2_ctrl;

void dummy_menu(void)
{

};

void hour_plus(void)
{
    if(++menu4x2_ctrl.time.hour > 23) menu4x2_ctrl.time.hour = 0;
}
void hour_minus(void)
{
    if(menu4x2_ctrl.time.hour > 0) menu4x2_ctrl.time.hour--;
    else menu4x2_ctrl.time.hour = 23;
}

void minute_plus_10(void)
{
    menu4x2_ctrl.time.minute +=  10;
    if(menu4x2_ctrl.time.minute > 59) menu4x2_ctrl.time.minute -= 60;
}
void minute_plus_1(void)
{
    if(++menu4x2_ctrl.time.minute > 59) menu4x2_ctrl.time.minute -=60;
}

void month_plus_1(void)
{
    if(++menu4x2_ctrl.time.month > 12) menu4x2_ctrl.time.month = 1;
}

void month_minus_1(void)
{
    if (menu4x2_ctrl.time.month > 1) menu4x2_ctrl.time.month--;
    else menu4x2_ctrl.time.month = 12;
}
void day_plus_1(void)
{
    if(++menu4x2_ctrl.time.day > 31) menu4x2_ctrl.time.day = 1;
}

void day_minus_1(void)
{
    if (menu4x2_ctrl.time.day > 1) menu4x2_ctrl.time.day--;
    else menu4x2_ctrl.time.day = 31;
}


void send_signal_event_alert(void)
{
   va_signal_set_event(VA_SIGNAL_EVENT_ALERT);
   menu4x2_show_now();
}

void send_signal_event_leave(void)
{
   va_signal_set_event(VA_SIGNAL_EVENT_LEAVE);
   menu4x2_show_now();
}

void send_signal_event_login(void)
{
   va_signal_set_event(VA_SIGNAL_EVENT_LOGIN);
   menu4x2_show_now();
}

void send_signal_event_confirm(void)
{
   va_signal_set_event(VA_SIGNAL_EVENT_CONFIRM);
   menu4x2_show_now();
}

void add_login_code_1(void)
{
    menu4x2_ctrl.code_array[0] = menu4x2_ctrl.active_key;
    menu4x2_show_now();
}
void add_login_code_2(void)
{
    menu4x2_ctrl.code_array[1] = menu4x2_ctrl.active_key;
    menu4x2_show_now();
}
void add_login_code_3(void)
{
    menu4x2_ctrl.code_array[2] = menu4x2_ctrl.active_key;
    bool correct_login = true;
    for (uint8_t i = 0; i < LOGIN_CODE_LEN; i++)
    {
        if (menu4x2_ctrl.code_array[i] != login_code[i]) correct_login = false;
    }
    if (correct_login) 
    {
        va_signal_set_event(VA_SIGNAL_EVENT_LOGIN);
        Serial.println("Login accepted");
    }
    else
    {
       Serial.println("Login failed");
    }
    menu4x2_show_now();

}
void start_time_functions(void)
{
    menu4x2_ctrl.time.year   =  main_ctrl.time.year;
    menu4x2_ctrl.time.month   =  main_ctrl.time.month;
    menu4x2_ctrl.time.day   =  main_ctrl.time.day;
    menu4x2_ctrl.time.hour   =  main_ctrl.time.hour;
    menu4x2_ctrl.time.minute   =  main_ctrl.time.minute;
    menu4x2_ctrl.time.second  =  main_ctrl.time.second;
}

void accept_new_time()
{
    main_ctrl.time.year   = menu4x2_ctrl.time.year;
    main_ctrl.time.month  = menu4x2_ctrl.time.month;
    main_ctrl.time.day    = menu4x2_ctrl.time.day;
     main_ctrl.time.hour  = menu4x2_ctrl.time.hour;
    main_ctrl.time.minute = menu4x2_ctrl.time.minute;
    main_ctrl.time.second = menu4x2_ctrl.time.second;
    rtc_set_main_ctrl_time();
}

const menu_def_st menu4x2_def[MENU_TOTAL] =
{
  {0 ,10},
  {1 ,10},
  {2 ,10},
  {3 ,10},
  {0 ,0},
  {1 ,0},
  {2 ,0},
  {3 ,0},
};



menu4x2_t menu4x2[MENU_NBR_OF] =
{
  [MENU_ROOT] =
  {
    { "          ", MENU_CAT_ACTIVE    , MENU_CHECK_OUT, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "Option    ", MENU_CAT_ACTIVE    , MENU_OPTION, dummy_menu},
    { "          ", MENU_CAT_TITLE     , MENU_CODE_1, dummy_menu},
    { "          ", MENU_CAT_DATE_TIME , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "Info      ", MENU_CAT_ACTIVE    , MENU_INFO, dummy_menu}
  },
  [MENU_OPTION] =
  {
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "Aika =    ", MENU_CAT_ACTIVE    , MENU_SET_TIME, start_time_functions},
    { "          ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "Test      ", MENU_CAT_ACTIVE    , MENU_TEST, dummy_menu},
    { "          ", MENU_CAT_TITLE     , MENU_OPTION, dummy_menu},
    { "Paivam =  ", MENU_CAT_ACTIVE    , MENU_SET_DATE, start_time_functions},
    { "          ", MENU_CAT_ACTIVE    , MENU_INFO, dummy_menu},
    { "Kuittaa   ", MENU_CAT_EMPTY     , MENU_ROOT, send_signal_event_confirm},
  },
  [MENU_SET_TIME] =
  {
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "Min + 10  ", MENU_CAT_ACTIVE    , MENU_SET_TIME, minute_plus_10},
    { "Min + 1   ", MENU_CAT_ACTIVE    , MENU_SET_TIME, minute_plus_1},
    { "Alkuun    ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_NEW_TIME , MENU_SET_TIME, dummy_menu},
    { "Tunti+1   ", MENU_CAT_ACTIVE    , MENU_SET_TIME, hour_plus},
    { "Tunti-1   ", MENU_CAT_ACTIVE    , MENU_SET_TIME, hour_minus},
    { "Hyvaksy   ", MENU_CAT_ACTIVE    , MENU_ROOT, accept_new_time},
  },
  [MENU_SET_DATE] =
  {
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "Paiva + 1 ", MENU_CAT_ACTIVE    , MENU_SET_DATE, day_plus_1},
    { "Paiva - 1 ", MENU_CAT_ACTIVE    , MENU_SET_DATE, day_minus_1},
    { "Alkuun    ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_NEW_TIME , MENU_SET_DATE, dummy_menu},
    { "KK + 1    ", MENU_CAT_ACTIVE    , MENU_SET_DATE, month_plus_1},
    { "KK - 1    ", MENU_CAT_ACTIVE    , MENU_SET_DATE, month_minus_1},
    { "Hyvaksy   ", MENU_CAT_ACTIVE    , MENU_OPTION, accept_new_time},
  },
  [MENU_CHECK_OUT] =
  {
    { "          ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "Kotona    ", MENU_CAT_ACTIVE    , MENU_ROOT, send_signal_event_login},
    { "          ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "Alkuun    ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_TITLE     , MENU_ROOT, dummy_menu},
    { "Poissa    ", MENU_CAT_ACTIVE    , MENU_ROOT, send_signal_event_leave},
    { "Sammuta   ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu}
  },
  [MENU_INFO] =
  {
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_TITLE     , MENU_ROOT, dummy_menu},
    { "Info 1    ", MENU_CAT_ACTIVE    , MENU_INFO_1, dummy_menu},
    { "Info 2    ", MENU_CAT_ACTIVE    , MENU_INFO_2, dummy_menu},
    { "Info 3    ", MENU_CAT_ACTIVE    , MENU_INFO_3, dummy_menu}
  },
  [MENU_INFO_1] =
  {
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_OPTION, dummy_menu},
    { "          ", MENU_CAT_TITLE     , MENU_INFO, dummy_menu},
    { "          ", MENU_CAT_SENSOR    , MENU_INFO, dummy_menu},
    { "          ", MENU_CAT_IS_DARK   , MENU_INFO, dummy_menu},
    { "          ", MENU_CAT_STATE     , MENU_INFO, dummy_menu}
  },
  [MENU_INFO_2] =
  {
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_OPTION, dummy_menu},
    { "          ", MENU_CAT_TITLE     , MENU_INFO, dummy_menu},
    { "          ", MENU_CAT_TBD       , MENU_INFO, dummy_menu},
    { "          ", MENU_CAT_TBD       , MENU_INFO, dummy_menu},
    { "          ", MENU_CAT_TBD       , MENU_INFO, dummy_menu}
  },
  [MENU_INFO_3] =
  {
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_ACTIVE    , MENU_OPTION, dummy_menu},
    { "          ", MENU_CAT_EMPTY     , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_TBD       , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_TBD       , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_TBD       , MENU_ROOT, dummy_menu}
  },
  [MENU_CODE_1] =
  {
    { "_XX       ", MENU_CAT_ACTIVE    , MENU_CODE_2, add_login_code_1},
    { "_XX       ", MENU_CAT_ACTIVE    , MENU_CODE_2, add_login_code_1},
    { "_XX       ", MENU_CAT_ACTIVE    , MENU_CODE_2, add_login_code_1},
    { "_XX       ", MENU_CAT_ACTIVE    , MENU_CODE_2, add_login_code_1},
    { "_XX       ", MENU_CAT_ACTIVE    , MENU_CODE_2, add_login_code_1},
    { "_XX       ", MENU_CAT_ACTIVE    , MENU_CODE_2, add_login_code_1},
    { "_XX       ", MENU_CAT_ACTIVE    , MENU_CODE_2, add_login_code_1},
    { "_XX       ", MENU_CAT_ACTIVE    , MENU_CODE_2, add_login_code_1},
  },
[MENU_CODE_2] =
  {
    { "*_X       ", MENU_CAT_ACTIVE    , MENU_CODE_3, add_login_code_2},
    { "*_X       ", MENU_CAT_ACTIVE    , MENU_CODE_3, add_login_code_2},
    { "*_X       ", MENU_CAT_ACTIVE    , MENU_CODE_3, add_login_code_2},
    { "*_X       ", MENU_CAT_ACTIVE    , MENU_CODE_3, add_login_code_2},
    { "*_X       ", MENU_CAT_ACTIVE    , MENU_CODE_3, add_login_code_2},
    { "*_X       ", MENU_CAT_ACTIVE    , MENU_CODE_3, add_login_code_2},
    { "*_X       ", MENU_CAT_ACTIVE    , MENU_CODE_3, add_login_code_2},
  },
[MENU_CODE_3] =
  {
    { "**_       ", MENU_CAT_ACTIVE    , MENU_ROOT, add_login_code_3},
    { "**_       ", MENU_CAT_ACTIVE    , MENU_ROOT, add_login_code_3},
    { "**_       ", MENU_CAT_ACTIVE    , MENU_ROOT, add_login_code_3},
    { "**_       ", MENU_CAT_ACTIVE    , MENU_ROOT, add_login_code_3},
    { "**_       ", MENU_CAT_ACTIVE    , MENU_ROOT, add_login_code_3},
    { "**_       ", MENU_CAT_ACTIVE    , MENU_ROOT, add_login_code_3},
    { "**_       ", MENU_CAT_ACTIVE    , MENU_ROOT, add_login_code_3},
    { "**_       ", MENU_CAT_ACTIVE    , MENU_ROOT, add_login_code_3},
  },

  [MENU_TEST] =
  {
    { "          ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "Kotona    ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "          ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "Alkuun    ", MENU_CAT_ACTIVE    , MENU_ROOT, dummy_menu},
    { "Login     ", MENU_CAT_ACTIVE    , MENU_ROOT, send_signal_event_login},
    { "Leave     ", MENU_CAT_ACTIVE    , MENU_ROOT, send_signal_event_leave},
    { "Alert     ", MENU_CAT_ACTIVE    , MENU_ROOT, send_signal_event_alert},
    { "Confirm   ", MENU_CAT_ACTIVE    , MENU_ROOT, send_signal_event_confirm}
  },
};


void menu4x2_reset_timeout(void)
{
  menu4x2_ctrl.menu_update_at = millis() + TIMEOUT_UPDATE;
  menu4x2_ctrl.menu_timeout_at = millis() + TIMEOUT_MENU;
  menu4x2_ctrl.bl_timeout_at = millis() + TIMEOUT_BACK_LIGHT;
}

void menu4x2_initialize(void)
{
  atask_add_new(&menu_timeout_task_handle);
  menu4x2_ctrl.level = MENU_ROOT;
  menu4x2_reset_timeout();
  lcd.setBacklight(1);
  menu4x2_show(MENU_ROOT);

}


void menu4x2_show(uint8_t mindx)
{
    char line0[40];
    //lcd.setBacklight(1);
    lcd.home();
    lcd.clear();


    for (uint8_t i = 0; i < MENU_TOTAL; i++)
    {
      lcd.setCursor(menu4x2_def[i].col, menu4x2_def[i].row);
      switch( menu4x2[menu4x2_ctrl.level][i].category )
      {
        case MENU_CAT_EMPTY:
          break;
        case MENU_CAT_ACTIVE:
          lcd.print(menu4x2[mindx][i].label);
          break;  
        case MENU_CAT_SENSOR:
          sprintf(line0, "LDR: %4d PIR: %1d", supervisor_get_ldr(), supervisor_get_pir());
          lcd.print (line0);
          // Serial.println(line0);
          break;
        case MENU_CAT_TITLE:
          switch(va_signal_get_state())
          {
            case VA_SIGNAL_STATE_START:
              sprintf(line0, "@@@ Los gehts..  @@@");
              break;
            case VA_SIGNAL_STATE_AT_HOME:
              sprintf(line0, "*** Villa Astrid ***");
              break;
            case VA_SIGNAL_STATE_COUNTDOWN:
              sprintf(line0, "Diez, nueve, ocho...");
              break;
            case VA_SIGNAL_STATE_AWAY:
              sprintf(line0, "<<< Solo en Casa >>>");
              break;
            case VA_SIGNAL_STATE_WARNING:
              sprintf(line0, "!!!! Attenzione !!!!");
              break;
            case VA_SIGNAL_STATE_ALARM:
              sprintf(line0, "!! PIIPAA  PIIPAA !!");
              break;
            case VA_SIGNAL_STATE_SENDING:
              sprintf(line0, "01010101010101010101");
              break;
            case MENU_CAT_TBD:
              sprintf(line0, "To be defined");
              lcd.print (line0);
              break;
          }
          lcd.print (line0);
          // Serial.println(line0);
          break;
        case MENU_CAT_STATE:
          sprintf(line0, "%s %02X %d", va_signal_get_state_label(), va_signal_get_cntr(), autom_get_program());
          //sprintf(line0, "%s %02X %d", "abc", signal_get_state(), autom_get_program());
          //sprintf(line0, "%02X %d", signal_get_state(), autom_get_program());
          lcd.print (line0);
          // Serial.println(line0);
          break;
        case MENU_CAT_DATE_TIME:
          sprintf(line0, "%02d-%02d-%02d %02d:%02d",
              main_ctrl.time.year,
              main_ctrl.time.month, 
              main_ctrl.time.day, 
              main_ctrl.time.hour, 
              main_ctrl.time.minute);
          lcd.print (line0);
          // Serial.println(line0);
          break;
        case MENU_CAT_NEW_TIME:
          sprintf(line0, "%02d-%02d-%02d %02d:%02d",
              menu4x2_ctrl.time.year,
              menu4x2_ctrl.time.month, 
              menu4x2_ctrl.time.day, 
              menu4x2_ctrl.time.hour, 
              menu4x2_ctrl.time.minute);
          lcd.print (line0);
          // Serial.println(line0);
          break;
        case MENU_CAT_IS_DARK:
          if (autom_get_is_dark()) sprintf(line0, "Is dark");
          else sprintf(line0, "Is light");
          lcd.print (line0);
          break;  
      }
    }
}


bool menu4x2_key_do_menu(char key)
{
  bool do_menu = false;
  uint8_t ikey = key - '1';
  if (ikey < MENU_TOTAL)
  {
    Serial.printf("menu4x2_key_do_menu: %d  %d  %d\n",menu4x2_ctrl.level, ikey,menu4x2[menu4x2_ctrl.level][ikey].category);
    if (menu4x2[menu4x2_ctrl.level][ikey].category >= MENU_CAT_ACTIVE) do_menu = true;
  }
  return do_menu;
}


void menu4x2_key_pressed(char key)
{
  uint8_t ikey = key - '1';
  uint8_t next_menu;
  if (ikey < MENU_TOTAL)
  {
      menu4x2_reset_timeout();
      lcd.setBacklight(1);
      menu4x2_ctrl.active_key = ikey;
      next_menu = menu4x2[menu4x2_ctrl.level][ikey].next_level;
      menu4x2[menu4x2_ctrl.level][ikey].cb();
      menu4x2_ctrl.level = next_menu;
      menu4x2_show(menu4x2_ctrl.level);
  }
}

void menu4x2_show_now(void)
{
    menu4x2_ctrl.menu_update_at = millis() - 1;
}

void menu4x2_timeout_task(void)
{
    if (menu4x2_ctrl.menu_update_at < millis())
    {
      menu4x2_show(menu4x2_ctrl.level);
      menu4x2_ctrl.menu_update_at = millis() + TIMEOUT_UPDATE;
    } 

    if (menu4x2_ctrl.menu_timeout_at < millis())
    {
      menu4x2_ctrl.level = MENU_ROOT;
      menu4x2_show(menu4x2_ctrl.level);
      menu4x2_ctrl.menu_timeout_at = millis() + TIMEOUT_MENU;
    } 

    if (menu4x2_ctrl.bl_timeout_at < millis())
    {
      if (digitalRead(PIN_PIR) == LOW) 
      {
          lcd.setBacklight(0);
          Serial.println("lcd.setBacklight(0)");
          menu4x2_ctrl.bl_timeout_at = millis() + TIMEOUT_BACK_LIGHT;
      }
    }

    if (digitalRead(PIN_PIR) == HIGH)
    {
      menu4x2_ctrl.bl_timeout_at = millis() + TIMEOUT_BACK_LIGHT;
      lcd.setBacklight(1);
      Serial.println("lcd.setBacklight(1)");
    }

}

