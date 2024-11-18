#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "main.h"
#include "io.h"
#include "va_signal.h"
#include "autom.h"
#include "atask.h"
#include "eep.h"
#include "helper.h"
#include "rtc.h"



#define VA_SIGNAL_LEAVE_COUNTDOWN_SEC  30
#define VA_SIGNAL_LEAVE_COUNTDOWN_MS   (VA_SIGNAL_LEAVE_COUNTDOWN_SEC * 1000)

typedef struct
{
  va_signal_event_et event;
  va_signal_state_et state_request;
  // atask_st  *sm;
  relay_prog_et relay_prog;
  uint8_t one_pix_state;
  uint8_t seq_indx;
  uint8_t seq_cntr;
  uint16_t return_state;
  uint32_t sm_millis;
  uint32_t count_down_ms;
  uint16_t cntr;
} va_signal_st;




extern main_ctrl_st main_ctrl;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel one_pix = Adafruit_NeoPixel(1, PIN_NEOPIXEL_1, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

uint32_t va_signal_color[RGB_INDX_NBR_OF] = 
{
  RGB_BLACK,
  RGB_RED,
  RGB_GREEN,
  RGB_BLUE,
  RGB_WHITE,
  RGB_MAGENTA,
  RGB_CYAN,
  RGB_YELLOW,
} ;

va_signal_seq va_signal_pattern[VA_SIGNAL_INDEX_NBR_OF] = 
{
  [VA_SIGNAL_INDEX_START]      = {{RGB_INDX_YELLOW , 5}, {RGB_INDX_BLACK, 5},  {RGB_INDX_BLACK, 0}},
  [VA_SIGNAL_INDEX_AT_HOME]    = {{RGB_INDX_GREEN  , 2}, {RGB_INDX_BLACK, 5},  {RGB_INDX_BLACK, 0}},
  [VA_SIGNAL_INDEX_COUNTDOWN]  = {{RGB_INDX_YELLOW , 2}, {RGB_INDX_GREEN, 5},  {RGB_INDX_BLACK, 0}},
  [VA_SIGNAL_INDEX_AWAY]       = {{RGB_INDX_RED , 4},    {RGB_INDX_YELLOW, 4}, {RGB_INDX_BLACK, 10}},
  [VA_SIGNAL_INDEX_WARNING]    = {{RGB_INDX_RED , 1},    {RGB_INDX_BLUE, 1},   {RGB_INDX_BLACK, 2}},
  [VA_SIGNAL_INDEX_ALARM]      = {{RGB_INDX_RED , 2},    {RGB_INDX_BLACK, 5},  {RGB_INDX_BLACK, 0}},
  [VA_SIGNAL_INDEX_SENDING]    = {{RGB_INDX_CYAN , 2},   {RGB_INDX_BLACK, 2},  {RGB_INDX_BLACK, 0}},
};

char state_label[VA_SIGNAL_INDEX_NBR_OF][10] =
{
// 0123456789  
  "Los gehts",
  "En Casa  ",
  "Countdown",
  "Solo     ",
  "Warning  ",
  "La Alarma",
  "Transmit "
};



atask_st signal_task_handle        = {"Signal fast    ", 100, 0, 0, 255, 0, 1, va_signal_update};
atask_st signal_state_task_handle  = {"Signal state   ", 1000, 0, 0, 255, 0, 1, va_signal_state_machine};

va_signal_st va_signal;

void va_signal_set_event(uint16_t state);
void va_signal_set_relay_prog(uint16_t state);



uint16_t va_signal_get_state(void)
{
   return (signal_state_task_handle.state);
}

uint8_t va_signal_get_state_index(void)
{
   return ((uint8_t)signal_state_task_handle.state >> 4);
}


void va_signal_set_state(uint16_t new_state)
{
    signal_state_task_handle.state = new_state;
    main_ctrl.state = signal_state_task_handle.state;
    rtc_set_ram_byte((uint8_t)main_ctrl.state);
    va_signal_set_relay_prog(signal_state_task_handle.state);
    Serial.printf("va_signal_set_state %d -> %d -> %d\n",new_state,main_ctrl.state, rtc_get_ram_byte());
}

void va_signal_save_return_state(void)
{
    va_signal.return_state = signal_state_task_handle.state;
}

void va_signal_return_state(void)
{
    va_signal_set_state(va_signal.return_state);
}

char *va_signal_get_state_label(void)
{
   uint8_t indx = (signal_state_task_handle.state >> 4) & 0x0F;
   return state_label[indx];
}

uint16_t va_signal_get_cntr(void)
{
    return va_signal.cntr;
}

void  va_signal_load_state(void)
{
    main_ctrl.state = rtc_get_ram_byte();
    signal_state_task_handle.state = main_ctrl.state;
}

void va_signal_send_state_to_24h(uint16_t state_index)
{
    SerialClock.printf("<C1MS:%d>\r\n", state_index);
}


void va_signal_initialize(void)
{
    uint32_t color_u32;
    
    atask_add_new(&signal_task_handle);
    atask_add_new(&signal_state_task_handle);

    va_signal.one_pix_state = 0;
    va_signal.event = VA_SIGNAL_EVENT_UNDEFINED;
    // va_signal.sm = atask_get_task(TASK_VA_SIGNAL_STATE);

    //signal_state_task_handle.state = 0;
    //va_signal_set_state(main_ctrl.state);
    va_signal_load_state();
    va_signal_set_relay_prog(signal_state_task_handle.state);    
    va_signal_send_state_to_24h(va_signal_get_state_index());

    //va_signal_set_event(signal_state_task_handle.state);

    va_signal.seq_indx = 0;
    va_signal.seq_cntr = 0;
    va_signal.cntr = 0;
    va_signal.relay_prog = RELAY_PROG_UNDEF;
    
    color_u32 =  RGB_MAGENTA; 
    one_pix.begin();
    one_pix.setBrightness(255);
    //one_pix.Color(255 ,0, 0);
    one_pix.setPixelColor(0, color_u32);
    one_pix.show(); // Initialize all pixels to 'off'
}



void va_signal_update(void)
{
    uint8_t ind_indx = signal_state_task_handle.state >> 4;
    if(++va_signal.seq_cntr > va_signal_pattern[ind_indx][va_signal.seq_indx].duration)
    {
        if(++va_signal.seq_indx >= NBR_COLOR_SEQ) va_signal.seq_indx = 0;
        if (va_signal_pattern[ind_indx][va_signal.seq_indx].duration == 0) va_signal.seq_indx = 0;
        va_signal.seq_cntr = 0;
        one_pix.setPixelColor(0, va_signal_color[va_signal_pattern[ind_indx][va_signal.seq_indx].color]);
        one_pix.show(); // Initialize all pixels to 'off'

    }

    // one_pix.setPixelColor(0, va_signal_color[va_signal.one_pix_state]);
    // one_pix.show(); // Initialize all pixels to 'off'
    // if(++va_signal.one_pix_state >= RGB_INDX_NBR_OF) va_signal.one_pix_state = 0;      


}


void va_signal_set_event(va_signal_event_et event)
{
    va_signal.event = event;
    Serial.printf("Set event %d\n\r",va_signal.event);
    switch(event)
    {
        case VA_SIGNAL_EVENT_LOGIN:
            va_signal_set_state(VA_SIGNAL_STATE_AT_HOME);
            break;
        case VA_SIGNAL_EVENT_LOGOUT:
            va_signal_set_state(VA_SIGNAL_STATE_COUNTDOWN);
            va_signal.count_down_ms = millis() +10000;
            break;
        case   VA_SIGNAL_EVENT_LEAVE:
            va_signal_set_state(VA_SIGNAL_STATE_AWAY);
            break;
        case  VA_SIGNAL_EVENT_ALERT:
            va_signal_set_state(VA_SIGNAL_STATE_ALARM);
            break;
        case VA_SIGNAL_EVENT_SENDING:
            if (signal_state_task_handle.state != VA_SIGNAL_STATE_SENDING) va_signal_save_return_state();
            va_signal_set_state(VA_SIGNAL_STATE_SENDING);
            va_signal.count_down_ms = millis() + 5000;
            break;
        case VA_SIGNAL_EVENT_TIMEOUT:
            
            break;
        default:
            break;    
    }
}


void va_signal_set_relay_prog(uint16_t state)
{
    relay_prog_et r_prog = RELAY_PROG_UNDEF;

    switch(state)
    {
        case VA_SIGNAL_STATE_START:
            r_prog = RELAY_PROG_UNDEF;
            break;
        case VA_SIGNAL_STATE_AT_HOME:
            r_prog = RELAY_PROG_AT_HOME;
            break;
        case   VA_SIGNAL_STATE_COUNTDOWN:
            break;
        case   VA_SIGNAL_STATE_AWAY:
            r_prog = RELAY_PROG_AWAY;
            break;
        case   VA_SIGNAL_STATE_WARNING:
            r_prog = RELAY_PROG_WARNING;
            break;
        case   VA_SIGNAL_STATE_ALARM:
            r_prog = RELAY_PROG_ALARM;
            break;
        case   VA_SIGNAL_STATE_SENDING:
            break;
    }
    if(r_prog != RELAY_PROG_UNDEF)
    {
        autom_set_program(r_prog);
    }
}


void va_signal_state_machine(void)
{
    static uint8_t prev_state = 255;
    static uint8_t prev_event = 255;
    if(millis() > va_signal.sm_millis)
    {
      //Serial.print('*');
      if (va_signal.event ==  VA_SIGNAL_EVENT_UNDEFINED)
      {
          //Serial.print('-');
          va_signal.event = VA_SIGNAL_EVENT_TIMEOUT;
          va_signal.sm_millis = 0xFFFFFFFF;
      }
    }   
    if ((va_signal.event != prev_event) || (signal_state_task_handle.state != prev_state))
    {
        Serial.printf("State %d, Event = %d\n\r", signal_state_task_handle.state, va_signal.event);
        //Serial.printf("millis %d, sm_millis %d\n\r", millis(), va_signal.sm_millis);
    }
    switch(signal_state_task_handle.state)
    {
      case VA_SIGNAL_STATE_START:  // Starting ...
        va_signal.sm_millis = millis() + 1000;
        //va_signal_set_relay_prog(signal_state_task_handle.state);
        va_signal_set_state(VA_SIGNAL_STATE_AT_HOME);
        break;
      case VA_SIGNAL_STATE_AT_HOME:   // At home state
        switch(va_signal.event)
        {
          case VA_SIGNAL_EVENT_SENDING:
            va_signal.sm_millis = millis() + 5000;
            signal_state_task_handle.state = VA_SIGNAL_STATE_SENDING;
            break;
          case VA_SIGNAL_EVENT_LEAVE:
            va_signal.sm_millis = millis() + VA_SIGNAL_LEAVE_COUNTDOWN_MS;
            va_signal.cntr = VA_SIGNAL_LEAVE_COUNTDOWN_SEC;
            signal_state_task_handle.state = VA_SIGNAL_STATE_COUNTDOWN;
            break;          
          case VA_SIGNAL_EVENT_ALERT:
            va_signal.relay_prog = RELAY_PROG_WARNING ;
            va_signal_set_state(VA_SIGNAL_STATE_WARNING);
            //va_signal_set_relay_prog(signal_state_task_handle.state);
            break;
        }        
        break;

      case VA_SIGNAL_STATE_COUNTDOWN:  // Countdown
        if (millis() > va_signal.count_down_ms)
        {
            va_signal_set_event(VA_SIGNAL_EVENT_LEAVE);
        }
        break;
      case VA_SIGNAL_STATE_AWAY:
        switch(va_signal.event)
        {
          case VA_SIGNAL_EVENT_LOGIN:
            va_signal.sm_millis = millis() + 30000;
            va_signal_set_state(VA_SIGNAL_STATE_AT_HOME);
            //va_signal_set_relay_prog(signal_state_task_handle.state);
            break;          
          case VA_SIGNAL_EVENT_ALERT:
            signal_state_task_handle.state = VA_SIGNAL_STATE_ALARM;
            va_signal_set_state(VA_SIGNAL_STATE_ALARM);
            //va_signal_set_relay_prog(signal_state_task_handle.state);
            autom_set_program(RELAY_PROG_ALARM);
            break;
        }
      case VA_SIGNAL_STATE_ALARM:
        switch(va_signal.event)
        {
          case VA_SIGNAL_EVENT_CONFIRM:
            va_signal_set_state(VA_SIGNAL_STATE_AT_HOME);
            //va_signal_set_relay_prog(signal_state_task_handle.state);
            break;          
        }
        break;
      case VA_SIGNAL_STATE_WARNING:
        switch(va_signal.event)
        {
          case VA_SIGNAL_EVENT_CONFIRM:
          case VA_SIGNAL_EVENT_TIMEOUT:
            va_signal_set_state(VA_SIGNAL_STATE_AT_HOME);
            //va_signal_set_relay_prog(signal_state_task_handle.state);
            break;          
        }
        break;

      case VA_SIGNAL_STATE_SENDING:
        if (millis() > va_signal.count_down_ms)
        {
          va_signal_return_state();
        }
        if (va_signal.event == VA_SIGNAL_EVENT_TIMEOUT) 
        {
          va_signal_set_state(VA_SIGNAL_STATE_AT_HOME);
        }         
        break;
      default:
        Serial.printf("!! Incorrect Signal State !!: %02X", signal_state_task_handle.state);
        va_signal_set_state(VA_SIGNAL_STATE_START);
        //va_signal_set_relay_prog(signal_state_task_handle.state);
        break; 
    }
    va_signal.event = VA_SIGNAL_EVENT_UNDEFINED;
    prev_state = signal_state_task_handle.state;
    prev_event = va_signal.event;

}