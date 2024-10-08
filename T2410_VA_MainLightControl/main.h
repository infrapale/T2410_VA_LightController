#ifndef __MAIN_H__
#define __MAIN_H__

#include "Arduino.h"
#define APP_NAME "T2410_VA_LightController"
#define RFM_SEND_INTERVAL 2000


typedef enum {
  TASK_READ_KEY = 0,
  TASK_SEND_RFM,
  TASK_MENU_TIMEOUT,
  TASK_VA_SIGNAL,
  TASK_VA_SIGNAL_STATE,
  TASK_AUTOM,
  TASK_SUPERVISOR,
  TASK_DEBUG,
  TASK_EEP,
  TASK_RTC,
  TASK_NBR_OF
} tasks_et;

typedef enum {
  SEMA_SERIAL2 = 0,
  SEMA_XXX
} sema_et;


typedef enum {
  STATUS_AT_HOME = 0,
  STATUS_AWAY
} status_et;

typedef struct
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} time_st;


typedef struct
{
  uint16_t state;
  uint16_t restart_cntr;
  time_st time;
  uint16_t light;
} main_ctrl_st;

#endif