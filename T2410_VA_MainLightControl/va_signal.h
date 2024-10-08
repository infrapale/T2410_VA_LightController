#ifndef __VA_SIGNAL_H__
#define __VA_SIGNAL_H__
#define NBR_COLOR_SEQ   3

typedef enum
{
  RGB_BLACK   = ((0x00 << 16) | (0x00 << 8) | 0x00),
  RGB_RED     = ((0xff << 16) | (0x00 << 8) | 0x00),
  RGB_GREEN   = ((0x00 << 16) | (0xff << 8) | 0x00),
  RGB_BLUE    = ((0x00 << 16) | (0x00 << 8) | 0xff),
  RGB_WHITE   = ((0xff << 16) | (0xff << 8) | 0xff),
  RGB_MAGENTA = ((0xff << 16) | (0x00 << 8) | 0xff),
  RGB_CYAN    = ((0x00 << 16) | (0xff << 8) | 0xff),
  RGB_YELLOW  = ((0xff << 16) | (0xFF << 8) | 0x00),
} rgb_colors_et;

typedef enum
{
  RGB_INDX_BLACK = 0,
  RGB_INDX_RED ,
  RGB_INDX_GREEN,
  RGB_INDX_BLUE,
  RGB_INDX_WHITE,
  RGB_INDX_MAGENTA,
  RGB_INDX_CYAN,
  RGB_INDX_YELLOW,
  RGB_INDX_NBR_OF
} rgb_color_indx_et;

typedef enum
{
  VA_SIGNAL_STATE_START     = 0x00,
  VA_SIGNAL_STATE_AT_HOME   = 0x10,
  VA_SIGNAL_STATE_COUNTDOWN = 0x20,
  VA_SIGNAL_STATE_AWAY      = 0x30,
  VA_SIGNAL_STATE_WARNING   = 0x40,
  VA_SIGNAL_STATE_ALARM     = 0x50,
  VA_SIGNAL_STATE_SENDING   = 0x60,
} va_signal_state_et;

typedef enum
{
  VA_SIGNAL_INDEX_START = 0,
  VA_SIGNAL_INDEX_AT_HOME,
  VA_SIGNAL_INDEX_COUNTDOWN,
  VA_SIGNAL_INDEX_AWAY,
  VA_SIGNAL_INDEX_WARNING,
  VA_SIGNAL_INDEX_ALARM,
  VA_SIGNAL_INDEX_SENDING,
  VA_SIGNAL_INDEX_NBR_OF
} va_signal_index_et;

typedef enum
{
  VA_SIGNAL_EVENT_UNDEFINED = 0,
  VA_SIGNAL_EVENT_LOGIN,
  VA_SIGNAL_EVENT_LOGOUT,
  VA_SIGNAL_EVENT_LEAVE,
  VA_SIGNAL_EVENT_ALERT,
  VA_SIGNAL_EVENT_CONFIRM,
  VA_SIGNAL_EVENT_SENDING,
  VA_SIGNAL_EVENT_TIMEOUT,
  VA_SIGNAL_EVENT_NBR_OF
} va_signal_event_et;

typedef struct
{
  uint8_t color;
  uint8_t duration;
} va_signal_pattern_st;

typedef va_signal_pattern_st va_signal_seq[NBR_COLOR_SEQ];

void va_signal_initialize(void);

void va_signal_update(void);

void va_signal_set_event(va_signal_event_et va_signal_event);

//void va_signal_set_state(va_signal_state_et new_state);

uint16_t va_signal_get_state(void);

void va_signal_set_state(uint16_t new_state);

uint8_t va_signal_get_state_index(void);

char *va_signal_get_state_label(void);

uint16_t va_signal_get_cntr(void);

//void va_signal_return_state(void);

void va_signal_state_machine(void);


#endif