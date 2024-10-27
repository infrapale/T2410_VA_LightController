#include "Arduino.h"
#include <Wire.h>
#include "edog.h"
#include "helper.h"

uint8_t tarr1[I2C_TX_BUFF_SIZE];
uint8_t tarr2[I2C_TX_BUFF_SIZE];

typedef struct 
{
    eeprom_index_et eeprom_addr_index;
} edog_ctrl_st;

i2c_st i2c;
edog_ctrl_st edog_ctrl;


void edog_put_tx_buff_uint32( uint16_t offs, uint32_t u32)
{
    i2c.tx_buff[offs + 0 ] = (u32 >> 24) & 0x000000FF;
    i2c.tx_buff[offs + 1 ] = (u32 >> 16) & 0x000000FF;
    i2c.tx_buff[offs + 2 ] = (u32 >> 8) & 0x000000FF;
    i2c.tx_buff[offs + 3 ] = (u32 >> 0) & 0x000000FF;
}

void edog_put_tx_buff_uint16( uint16_t offs, uint16_t u16)
{
    i2c.tx_buff[offs + 0 ] = (u16 >> 8) & 0x000000FF;
    i2c.tx_buff[offs + 1 ] = (u16 >> 0) & 0x000000FF;
}
void edog_put_tx_buff_uint8( uint16_t offs, uint8_t u8)
{
    i2c.tx_buff[offs + 0 ] = (u8 >> 0) & 0x000000FF;
}

uint32_t edog_get_rx_buff_uint32(uint16_t offs)
{
    uint32_t u32 = 0;
    u32 |= ((uint32_t)i2c.rx_buff[offs + 0 ] << 24) & 0x0000000FFUL;
    u32 |= ((uint32_t)i2c.rx_buff[offs + 1 ] << 16) & 0x0000000FFUL;
    u32 |= ((uint32_t)i2c.rx_buff[offs + 2 ] << 8)  & 0x0000000FFUL;
    u32 |= ((uint32_t)i2c.rx_buff[offs + 3 ] << 0)  & 0x0000000FFUL;
    return u32;
}

uint16_t edog_get_rx_buff_uint16(uint16_t offs)
{
    uint16_t u16 = 0;
    u16 |= ((uint16_t)i2c.rx_buff[offs + 0 ] << 8)  & 0x0000000FFUL;
    u16 |= ((uint16_t)i2c.rx_buff[offs + 1 ] << 0)  & 0x0000000FFUL;
    return u16;
}

uint8_t edog_get_rx_buff_uint8(uint16_t offs)
{
    uint8_t u8 = 0;
    u8 = ((uint8_t)i2c.rx_buff[offs + 0 ] << 0)  & 0x0000000FFUL;
    return u8;
}


void edog_build_test_data(uint8_t base_value)
{
  for( uint8_t i = 0; i < 8; i++)
  {
    tarr1[i] = (base_value + i) ;
    tarr2[i] = base_value + i*4;
  }
}

  

void edog_initialize(uint8_t i2c_addr)
{
  Serial.printf("edog_initialize(0x%2X)\n",i2c_addr);
  i2c.addr = i2c_addr;
  edog_ctrl.eeprom_addr_index = EEPROM_MAIN_DATA;
  //edog_build_test_data(0x20);
  edog_test_eeprom_write_read();
}

void edog_print_rx_buff(void)
{
  Serial.print("i2c.rx: ");
  helper_print_hex_arr(i2c.rx_buff,9);
}

void edog_print_tx_buff(void)
{
  Serial.print("i2c.tx: ");
  helper_print_hex_arr(i2c.tx_buff,9);
}

void edog_receive_i2c(void)
{
  if(i2c.reg_m2s > 0)
  {
    Wire.beginTransmission(i2c.addr); 
    Wire.write( i2c.tx_buff, i2c.reg_m2s + 1)  ;      
    Wire.endTransmission();
  }  
}

void edog_read_i2c(uint8_t bytes)
{
    Wire.requestFrom(i2c.addr, bytes);   
    uint8_t i = 0; 
    while(Wire.available())    
    { 
      int c = Wire.read();
      i2c.rx_buff[i++] = (uint8_t) c;
    }
}

void edog_send_i2c(void)
{
  {
    Wire.beginTransmission(i2c.addr); 
    Wire.write( i2c.tx_buff, i2c.reg_m2s + 1)  ;      
    Wire.endTransmission();
  }  
}

// void edog_set_read_pos(uint8_t pos)
// {
//   Serial.println("Set read pos");
//   edog_build_uint_msg(REG_ADDR_SET_RD_POS, pos, 1, 0);
//   edog_send_i2c();
// }

// void edog_rd_reg(uint8_t pos, uint8_t len)
// {
//   edog_set_read_pos(pos);
//   delay(1);
//   Wire.requestFrom(i2c.addr, len); 
//   Wire.requestFrom(i2c.addr, i2c.reg_s2m);   
//   uint8_t i = 0; 
//   while(Wire.available())    
//   { 
//     int c = Wire.read();
//     i2c.rx_buff[i++] = (uint8_t) c;
//   }
//   edog_print_rx_buff();
// }
 

void edog_send_receive(void)
{

  Serial.print("send_receive: "); 
  // Serial.print(i2c.addr, HEX); 
  Serial.print(" reg_m2s="); Serial.print(i2c.reg_m2s);
  Serial.print(" reg_s2m="); Serial.print(i2c.reg_s2m);
  Serial.println(" ");
  Serial.flush();

  Wire.beginTransmission(i2c.addr); 
  Wire.write( i2c.tx_buff, i2c.reg_m2s + 1)  ;      
  Wire.endTransmission();

  if(i2c.reg_s2m > 0)
  {
    // delay(1);
    //Wire.beginTransmission(i2c.addr); 
    Wire.requestFrom(i2c.addr, i2c.reg_s2m);   
    uint8_t i = 0; 
    while(Wire.available())    
    { 
      int c = Wire.read();
      i2c.rx_buff[i++] = (uint8_t) c;
    }

    //Wire.endTransmission();
  }
}

void edog_build_uint_msg(uint8_t raddr, uint32_t value, uint8_t m2s, uint8_t s2m)
{
  uint8_t aindx = 0;    //[0] is reserved for register address
  i2c.reg_addr = raddr;
  i2c.reg_data = value;
  i2c.reg_m2s = m2s;
  i2c.reg_s2m = s2m;

  i2c.tx_buff[aindx++] = i2c.reg_addr;
  if (i2c.reg_m2s == 4)
  {
    i2c.tx_buff[aindx++] = (i2c.reg_data >> 24) & 0x000000FF;
    i2c.tx_buff[aindx++] = (i2c.reg_data >> 16) & 0x000000FF;
  }
  if (i2c.reg_m2s > 1)
  {
    i2c.tx_buff[aindx++] = (i2c.reg_data >> 8) & 0x000000FF;
  }
  i2c.tx_buff[aindx] = (i2c.reg_data >> 0) & 0x000000FF;
}

void edog_build_array_msg(uint8_t raddr, uint8_t *arr, uint8_t m2s, uint8_t s2m)
{
  uint8_t aindx = 0;    //[0] is reserved for register address
  i2c.reg_addr = raddr;
  i2c.reg_m2s = m2s;
  i2c.reg_s2m = s2m;
  // Serial.printf("i2c.reg_addr %02X %02X\n", i2c.reg_addr, i2c.tx_buff[0]); Serial.flush();
  i2c.tx_buff[aindx++] = i2c.reg_addr;
  for (uint8_t i = 0; i < m2s; i++)
  {
    i2c.tx_buff[aindx++] = arr[i];
  }
}


void edog_set_wd_timeout(uint32_t wd_timeout)
{
  Serial.printf("Watchdog timeout = %d\n\r", wd_timeout);
  edog_build_uint_msg(CMD_SET_WD_INTERVAL, wd_timeout, 4, 0);
  edog_send_receive();
}

void edog_set_sleep_time(uint32_t sleep_time)
{
  Serial.printf("Sleep time = %d\n\r",sleep_time);
  edog_build_uint_msg(CMD_SET_SLEEP_TIME, sleep_time, 4, 0);
  edog_send_receive();
}

void edog_clear_watchdog(void)
{
  // Serial.println("Clear watchdogSleep time = ");
  edog_build_uint_msg(CMD_CLEAR_WATCHDOG, 1, 1, 0);
  edog_send_receive();
}

void edog_switch_off(void)
{
  Serial.println("Goto Sleep");
  edog_build_uint_msg(CMD_SWITCH_OFF, 1, 1, 0);
  edog_send_receive();
}

// void edog_switch_off_1(uint8_t value)
// {
//   Serial.println("Switch off 1");
//   edog_build_uint_msg(REG_ADDR_SWITCH_OFF_1, value, 1, 0);
//   edog_send_receive();
// }

void edog_load_eeprom(void)
{
  Serial.println("Load EEPROM Data");
  edog_build_uint_msg(CMD_EEPROM_LOAD, 1, 1, 0);
  edog_send_i2c();
}

void edog_save_eeprom(void)
{
  Serial.println("Save EEPROM Data");
  edog_build_uint_msg(CMD_EEPROM_SAVE, 1, 1, 0);
  edog_send_i2c();
}

void edog_select_eeprom_index(eeprom_index_et eeprom_addr_index)
{
  edog_ctrl.eeprom_addr_index = eeprom_addr_index;
  Serial.printf("Select EEPROM index %d\n",edog_ctrl.eeprom_addr_index);
  edog_build_uint_msg(CMD_SET_EEPROM_INDEX, (uint8_t)edog_ctrl.eeprom_addr_index, 1, 0);
  edog_send_i2c();
}


void edog_read_eeprom(eeprom_index_et eeprom_addr_index)
{
  Serial.printf("Read EEPROM @ %d\n", eeprom_addr_index);
  edog_select_eeprom_index(eeprom_addr_index);

  edog_build_uint_msg(CMD_EEPROM_LOAD, 0, 0, 0);
  edog_send_i2c();
  delay(10);

  delay(5);
  edog_build_uint_msg(CMD_EEPROM_READ, 1, 0, 8);

  edog_send_receive();  
  //edog_print_rx_buff();
}

void edog_write_eeprom(eeprom_index_et eeprom_addr_index, uint8_t *arr)
{
  Serial.printf("Write EEPROM \n");
  edog_select_eeprom_index(eeprom_addr_index);
 
  delay(5);
  edog_build_array_msg(CMD_EEPROM_WRITE, arr, 8, 0);
  edog_send_i2c();
  delay(10);

  // edog_build_uint_msg(CMD_EEPROM_SAVE, 0, 0, 0);  obsolete!!
  // edog_send_i2c();
  // //edog_print_tx_buff();
}

// void edog_write_eeprom_buff(uint16_t addr)
// {
//   Serial.printf("Write EEPROM @ %04X", addr);
 
//   edog_build_uint_msg(REG_ADDR_EEPROM_ADDR, (uint32_t) addr, 2, 0);
//   edog_send_i2c();
  
//   delay(1);
//   i2c.reg_addr = REG_ADDR_EEPROM_WRITE;
//   i2c.reg_m2s = 9;
//   i2c.reg_s2m = 0;
//   i2c.tx_buff[0] = i2c.reg_addr;
//   edog_send_i2c();
//   delay(1);

//   edog_build_uint_msg(REG_ADDR_EEPROM_SAVE, 0, 1, 0);
//   edog_send_i2c();
//   delay(10);
//   edog_print_tx_buff();
// }


void edog_test_eeprom_write_read(void)
{
  eeprom_index_et eeprom_addr_index;

  Serial.printf("edog_test_eeprom_write_read ------\n");
  for (uint8_t iter=0; iter<2; iter++)
  {
    Serial.printf("edog_test_eeprom_write_read test iteration %d\n", iter);
    
    eeprom_addr_index = EEPROM_USER_4;
    Serial.printf("> read index= %d\n", eeprom_addr_index);
    edog_read_eeprom(eeprom_addr_index);
    edog_print_rx_buff();

    eeprom_addr_index = EEPROM_USER_5;
    Serial.printf("> read index= %d\n", eeprom_addr_index);
    edog_read_eeprom(eeprom_addr_index);
    edog_print_rx_buff();

    eeprom_addr_index = EEPROM_USER_4;
    Serial.printf("> write index= %d\n", eeprom_addr_index);
    edog_build_test_data(0x40);
    edog_write_eeprom(eeprom_addr_index, tarr1);
    edog_print_tx_buff();

    eeprom_addr_index = EEPROM_USER_5;
    Serial.printf("> write index= %d\n", eeprom_addr_index);
    edog_build_test_data(0x50);
    edog_write_eeprom(eeprom_addr_index, tarr1);
    edog_print_tx_buff();

    delay(100);
  }
}

