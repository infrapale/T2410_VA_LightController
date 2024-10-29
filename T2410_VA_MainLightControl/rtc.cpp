#include "main.h"
#include "io.h"
#include <PCF85063A.h>
#include <time.h>
#include "RTC_NXP.h"
#include "RTClib.h"
#include "atask.h"

#define PCF85063_REG_RAM_BYTE  (3u) 

typedef struct 
{
    tm      new_time;
    DateTime  now;
} rtc_ctrl_st;

PCF85063A rtc;
rtc_ctrl_st rtc_ctrl;

extern main_ctrl_st main_ctrl;

void rtc_time_machine(void);

void set_time(void) {
  /*  !!!! "strptime()" is not available in Arduino's "time.h" !!!!
  const char* current_time  = "2023-4-7 05:25:30";
  const char* format  = "%Y-%m-%d %H:%M:%S";
  struct tm	tmv;
  strptime( current_time, format, &tmv );
  */

  //struct tm now_tm;

  rtc_ctrl.new_time.tm_year = 2024 - 1900;
  rtc_ctrl.new_time.tm_mon = 11 - 1;  // It needs to be '3' if April
  rtc_ctrl.new_time.tm_mday = 28;
  rtc_ctrl.new_time.tm_hour = 20;
  rtc_ctrl.new_time.tm_min = 00;
  rtc_ctrl.new_time.tm_sec = 00;

  rtc.set(&rtc_ctrl.new_time);

  Serial.println("RTC got time information");
}

void rtc_set_main_ctrl_time(void) 
{
  rtc_ctrl.new_time.tm_year = main_ctrl.time.year -1900;
  rtc_ctrl.new_time.tm_mon  = main_ctrl.time.month -1;
  rtc_ctrl.new_time.tm_mday = main_ctrl.time.day;
  rtc_ctrl.new_time.tm_hour = main_ctrl.time.hour;
  rtc_ctrl.new_time.tm_min  = main_ctrl.time.minute;
  rtc_ctrl.new_time.tm_sec  = 00;

  rtc.set(&rtc_ctrl.new_time);

  Serial.println("RTC set to main time");
}


void  rtc_apply_epoc_time(uint32_t epoc_time)
{
    //uint32_t epoc = epoc_time;
    const DateTime now =epoc_time;
    // rtc_ctrl.new_time.tm_year = year(epoc);
    // rtc_ctrl.new_time.tm_mon = month(epoc);  
    // rtc_ctrl.new_time.tm_mday = day(epoc);
    // rtc_ctrl.new_time.tm_hour = hour(epoc);
    // rtc_ctrl.new_time.tm_min = minute(epoc);
    // rtc_ctrl.new_time.tm_sec = second(epoc);
    // rtc.set(&rtc_ctrl.new_time);
    
    // rtc.set(now);   TODO
}

atask_st rtc_task_handle           = {"RTC TimeMachine", 1000,0, 0, 255, 0, 1, rtc_time_machine};


void rtc_set_ram_byte(uint8_t u8)
{
    rtc.write_r8(PCF85063_REG_RAM_BYTE, u8);
    Serial.printf("******  rtc_set_ram_byte(%d)\n",u8);
    Serial.flush();
}

uint8_t rtc_get_ram_byte(void)
{
    return rtc.read_r8(PCF85063_REG_RAM_BYTE);
}



void rtc_initialize(void)
{
  atask_add_new(&rtc_task_handle);
  
  if (rtc.oscillator_stop() || true) 
  {
      Serial.println("==== oscillator_stop detected :( ====");
      set_time();
  } 
  else 
  {
      Serial.println("---- RTC has beeing kept running! :) ----");
  }
  //rtc_set_ram_byte(69);
}

void rtc_time_machine(void)
{

  DateTime now = rtc.time(NULL);

  // Serial.printf("RAM byte= %d\n", rtc.read_r8(PCF85063_REG_RAM_BYTE));
  
  main_ctrl.time.year = now.year();
  main_ctrl.time.month = now.month();
  main_ctrl.time.day = now.day(); 
  main_ctrl.time.hour= now.hour();
  main_ctrl.time.minute = now.minute();
  main_ctrl.time.second = now.second();

//   Serial.printf("%04d-%02d-%02d %02d:%02d:%02d\n", 
//       main_ctrl.time.year, main_ctrl.time.month, main_ctrl.time.day,
//       main_ctrl.time.hour, main_ctrl.time.minute, main_ctrl.time.second);
}

// rtc_get_time(void)
// {
//   return rtc_ctrl.now;
// }
