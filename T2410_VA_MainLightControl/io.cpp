#include "Arduino.h"
#include "io.h"

typedef struct
{
    uint8_t wd_state;
} io_st;

io_st io = {HIGH};

// TinyPico Common Functions

void io_enable_vext(bool enable)
{
    if (enable)  digitalWrite(PIN_VEXT_EN, LOW);
    else digitalWrite(PIN_VEXT_EN, HIGH);
}

void io_enable_v33(bool enable)
{
    if (enable)  digitalWrite(PIN_V33_EN, LOW);
    else digitalWrite(PIN_V33_EN, HIGH);
}

void io_ext_clr(bool do_clear)
{
    // an inverting transistor is required
    if (do_clear)  digitalWrite(PIN_EXT_CLR, HIGH);
    else digitalWrite(PIN_EXT_CLR, LOW);
}

void io_feed_watchdog(void)
{
    if (io.wd_state == LOW) io.wd_state = HIGH;
    else io.wd_state = LOW;
    digitalWrite(PIN_EDOG_CLR, io.wd_state);
}

bool io_internal_wd_is_anabled(void)
{
    return (digitalRead(PIN_WD_ENABLE) == HIGH);
}

void io_initialize_tiny_pico(void)
{
    analogReadResolution(12);
    pinMode(PIN_PIR, INPUT_PULLUP);
    pinMode(PIN_PWM2A, OUTPUT);
    pinMode(PIN_PWM2B, OUTPUT);
    pinMode(PIN_WD_ENABLE, INPUT_PULLUP);

    pinMode(PIN_EDOG_CLR, OUTPUT);
    pinMode(PIN_VEXT_EN, OUTPUT);
    pinMode(PIN_V33_EN, OUTPUT);
    pinMode(PIN_EXT_CLR, OUTPUT);
    io.wd_state = LOW;  
}
