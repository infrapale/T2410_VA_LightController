#ifndef __RTC_H__
#define __RTC_H__

void rtc_initialize(void);

void rtc_time_machine(void);

void rtc_set_main_ctrl_time(void);

void rtc_set_ram_byte(uint8_t u8);

uint8_t rtc_get_ram_byte(void);

#endif