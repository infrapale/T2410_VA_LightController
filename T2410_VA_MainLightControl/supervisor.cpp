#include "main.h"
#include "io.h"
#include "signal.h"
#include "atask.h"
#include "edog.h"
#include "supervisor.h"

typedef struct
{
  uint16_t err_cntr[SUPER_ERR_NBR_OF];
  uint16_t ldr_val;
  uint8_t  pir_val;
  atask_st  *sm; 
  uint8_t  wd_pin_state;
} supervisor_st;

uint16_t err_limit[SUPER_ERR_NBR_OF] =
{
  [SUPER_ERR_GET_TIME] = 4,
  [SUPER_ERR_1]        = 1,
};

supervisor_st super;
atask_st supervisor_task_handle    = {"Supervisor     ", 1000,0, 0, 255, 0, 1, supervisor_task};

void supervisor_initialize(void)
{
  atask_add_new(&supervisor_task_handle);
 
  super.sm = &supervisor_task_handle;
  super.sm->state = 0;
  super.ldr_val = 0;
  super.wd_pin_state = 0;
  for (uint8_t i = 0; i < SUPER_ERR_NBR_OF; i++) super.err_cntr[i] = 0;
}

void supervisor_inc_cntr(super_err_et cntr_indx)
{
  super.err_cntr[cntr_indx]++;
}

void supervisor_clr_cntr(super_err_et cntr_indx)
{
  super.err_cntr[cntr_indx] = 0;
}

uint8_t supervisor_get_pir(void)
{
  return super.pir_val;
}

uint16_t supervisor_get_ldr(void)
{
  return super.ldr_val;
}

void supervisor_wdt_begin(uint32_t d_ms)
{
    if (d_ms > 8300) d_ms= 8300;
    if (io_internal_wd_is_anabled()) rp2040.wdt_begin(d_ms);
}

void supervisor_wdt_reset(void)
{
  rp2040.wdt_reset();
}


void supervisor_debug_print(void)
{
  Serial.print("Supervisor err_cntr: ");
    for (uint8_t i = 0; i < SUPER_ERR_NBR_OF; i++) 
    {
      Serial.printf("%d ", super.err_cntr[i]);
    }
  Serial.println();
}

bool supervisor_pwr_is_on(void)
{
  return super.sm->state >= 1;
}

void supervisor_task(void)
{
  static uint32_t delay_cntr = 0;
  uint8_t tindx;
  super_err_et err_cntr_at_limit = SUPER_ERR_NBR_OF;
  bool clr_edog = false;

  // WD DEBUG super.err_cntr[SUPER_ERR_GET_TIME]++;  // DEBUG -REMOVE

  supervisor_wdt_reset();  /// TODO

  switch(super.sm->state)
  {
    case 0:
      io_enable_v33(false);
      io_enable_vext(false);
      io_feed_watchdog();
      delay_cntr = millis() + 5000;
      super.sm->state++;
      break;
    case 1:
      if (millis() > delay_cntr)
      {
        io_enable_v33(true);
        io_enable_vext(true);
        io_feed_watchdog();
        super.sm->state = 10;
      } 
      supervisor_wdt_begin(8000);
      break;  
    case 10:
      super.ldr_val = analogRead(PIN_LDR);
      super.pir_val = digitalRead(PIN_PIR);
      super.sm->state = 20;
      for (uint8_t i = 0; i < SUPER_ERR_NBR_OF; i++) 
      {
        if (super.err_cntr[i] >= err_limit[i])
        {
          err_cntr_at_limit = (super_err_et)i;
          break;
        } 
      }

      if (err_cntr_at_limit < SUPER_ERR_NBR_OF)
      {
        Serial.printf("Error limit exceeded: index = %d cntr = %d\n", 
          err_cntr_at_limit, super.err_cntr[err_cntr_at_limit]);
          super.sm->state = 100;
          clr_edog = false;
      }
      else
      {
         clr_edog = true;
      }
      break;
    case 20:
      super.sm->state = 10;
      // tindx = task_check_all();
      // if (tindx < TASK_NBR_OF) 
      // {
      //   task_st  *tptr = task_get_task(tindx);
      //   Serial.printf("!!! Task counter overflow: %s %d\n", tptr->name, tptr->wd_cntr);
      //   task_print_status(true);       
      //   super.sm->state = 100;
      // } 
      // else 
      // {
      //   clr_edog = true;
      // }
      break;
     case 100:
      super.sm->state++;
      Serial.printf("!!! Waiting for Watchdog Reset\n\r");
      break;
    case 101:
      break;
    default:
      super.sm->state = 0;
      break;

  }
  // if (clr_edog)  edog_clear_watchdog();
  if (clr_edog ) 
  {
    //io_feed_watchdog();
  }
}