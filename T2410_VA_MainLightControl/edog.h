#ifndef __EDOG_H__
#define __EDOG_H__


#define REG_ARRAY_LEN 20


#define I2C_RX_BUFF_SIZE   (16)
#define I2C_TX_BUFF_SIZE   (16)

#define I2C_ADDR    13

typedef struct
{
  char      mode;
  uint8_t   addr;
  uint8_t   reg_addr;
  uint32_t  reg_data;
  uint8_t   reg_m2s;
  uint8_t   reg_s2m;
  uint8_t   tx_buff[I2C_TX_BUFF_SIZE];
  uint8_t   rx_buff[I2C_RX_BUFF_SIZE];

} i2c_st;

typedef enum 
{
  MASTER_TO_SLAVE = 0,
  SLAVE_TO_MASTER 
} master_slave_direction_et;

typedef enum
{
  CMD_WD_UNDEF         = 0x00,
  CMD_SET_WD_INTERVAL  = 0x01,
  CMD_GET_WD_INTERVAL  = 0x02,
  CMD_SET_SLEEP_TIME   = 0x03,
  CMD_GET_SLEEP_TIME   = 0x04,
  CMD_CLEAR_WATCHDOG   = 0x05,
  CMD_SWITCH_OFF       = 0x06,
  CMD_SET_EEPROM_INDEX = 0x07,
  CMD_EEPROM_LOAD      = 0x08,
  CMD_EEPROM_SAVE      = 0x09,
  CMD_POWER_OFF_0      = 0x0A,
  CMD_POWER_OFF_1      = 0x0B,
  CMD_EXT_RESET        = 0x0C,
  CMD_EEPROM_READ      = 0x0D,
  CMD_EEPROM_WRITE     = 0x0E,
  CMD_GET_RESTARTS     = 0x0F,
  CMD_NBR_OF           = 16
 } cmd_et;

#define EEPROM_ADDR_MAIN_DATA     0x00
#define EEPROM_ADDR_RESTARTS      0x20
#define EEPROM_ADDR_USER_0        0x30
#define EEPROM_ADDR_USER_1        0x38
#define EEPROM_ADDR_USER_2        0x50
#define EEPROM_ADDR_USER_3        0x58
#define EEPROM_ADDR_USER_4        0x60
#define EEPROM_ADDR_USER_5        0x68
#define EEPROM_ADDR_USER_6        0x70
#define EEPROM_ADDR_USER_7        0x78

typedef enum
{
  EEPROM_MAIN_DATA     = 0,
  EEPROM_RESTARTS,
  EEPROM_USER_0,
  EEPROM_USER_1,
  EEPROM_USER_2,
  EEPROM_USER_3,
  EEPROM_USER_4,
  EEPROM_USER_5,
  EEPROM_USER_6,
  EEPROM_USER_7,
  EEPROM_NBR_OF
 } eeprom_index_et;



typedef enum
{
  EEPROM_FUNC_READ        = 0x01,
  EEPROM_FUNC_WRITE       = 0x02,
} eeprom_func_et;

typedef enum
{
  EEPROM_STATUS_RD_READY    = 0x01,
  EEPROM_STATUS_WR_READY    = 0x02,
} eeprom_status_et;

void edog_initialize(uint8_t i2c_addr);

uint32_t edog_get_rx_buff_uint32(uint16_t offs);

uint16_t edog_get_rx_buff_uint16(uint16_t offs);

uint8_t edog_get_rx_buff_uint8(uint16_t offs);

void edog_put_tx_buff_uint32( uint16_t offs, uint32_t u32);

void edog_put_tx_buff_uint16( uint16_t offs, uint16_t u16);

void edog_put_tx_buff_uint8( uint16_t offs, uint8_t u8);

void edog_rd_reg(uint8_t pos, uint8_t len);

void edog_select_eeprom_index(eeprom_index_et eeprom_addr_index);

void edog_read_eeprom(eeprom_index_et eeprom_addr_index);

void edog_write_eeprom(eeprom_index_et eeprom_addr_index, uint8_t *arr);

void edog_test_eeprom_write_read(void);

void edog_test_eeprom_write_read(void);

void edog_build_uint_msg(uint8_t raddr, uint32_t value, uint8_t m2s, uint8_t s2m);


void edog_build_array_msg(uint8_t raddr, uint8_t *arr, uint8_t m2s, uint8_t s2m);

void edog_build_test_data(uint8_t base_value);

void edog_read_i2c(uint8_t bytes);

void edog_set_wd_timeout(uint32_t wd_timeout);

uint32_t edog_get_wd_timeout(void);

void edog_set_sleep_time(uint32_t sleep_time);

void edog_switch_off(void);

void edog_switch_off_1(uint8_t value);

void edog_load_eeprom(void);

void edog_save_eeprom(void);

void edog_print_rx_buff(void);

void edog_print_tx_buff(void);


#endif