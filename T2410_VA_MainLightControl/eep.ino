#include "main.h"

#include "atask.h"
/*
typedef struct 
{
    atask_st  *th;
    uint16_t  addr;
    uint16_t  size;
    uint16_t  commit_in_sec;
} eep_st;


eep_st eep;

void eep_initialize(uint16_t e_size)
{
  eep.th = atask_get_task(TASK_EEP);
  eep.th->state = 0;
  Serial.printf("eep_initialize: state = %d\n",eep.th->state);

  // edog_initialize(EDOG_I2C_ADDR);
  //edog_set_wd_timeout(5000);
  //delay(10);
  //edog_set_sleep_time(2000);
  //edog_rd_reg(REG_ADDR_SLEEP_TIME,4);

  //edog_read_i2c(4);
  // helper_initialize_data();
  
  
}

bool eep_set_addr(uint16_t addr)
{
  bool is_ok = false;
  if (addr < eep.size)
  {
    eep.addr = addr;
    is_ok = true;
  }
  return is_ok;
}

bool eep_next(void)
{
  bool is_ok = false;
  if (eep.addr < eep.size)
  {
    eep.addr++;
    is_ok = true;
  }
  return is_ok;
}


bool eep_commit()
{
  return EEPROM.commit();
}

void eep_request_commit(uint16_t commit_in_sec)
{
    if (eep.commit_in_sec == 0)
    {
      eep.commit_in_sec = commit_in_sec;
    }
}

void eep_write_u8(uint8_t u8)
{
    EEPROM.write(eep.addr, u8);
    eep.addr++;
}

void eep_write_u16(uint16_t u16)
{
    uint8_t u8;
    u8 = (uint8_t) ((u16 >> 8) & 0xFF);
    EEPROM.write(eep.addr, u8);
    eep.addr++;
    u8 = (uint8_t) ((u16 >> 0) & 0xFF);
    EEPROM.write(eep.addr, u8);
    eep.addr++;
}

uint8_t eep_read_u8(void)
{
  uint8_t u8 = EEPROM.read(eep.addr);
  eep.addr++;
  return (u8);
}

uint16_t eep_read_u16(void)
{
  uint16_t u16;
  uint8_t u8 = EEPROM.read(eep.addr);
  u16 = (uint16_t) (u8 << 8);
  eep.addr++;
  u8 = EEPROM.read(eep.addr);
  u16 |= u8; 

  return (u16);
}

void eep_time_machine(void)
{

  // Serial.print("EEP="); Serial.print(eep.th->state);  
  // Serial.print(" commit in=");  Serial.print(eep.commit_in_sec);
  // Serial.println();

  switch(eep.th->state)
  {
    case 0:
      eep.th->state = 10;
      break;
    case 10:
      if (eep.commit_in_sec > 0) eep.th->state = 20;
      else eep.th->state = 10;
      break;
    case 20:
      if (eep.commit_in_sec > 0)
      {
        eep.commit_in_sec--;
      }
      else
      {
        eep_commit();
        Serial.println("EEP committed");
        eep.th->state = 10;
      }
      break;
  }
}

*/
